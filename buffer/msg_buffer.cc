#include "msg_buffer.hpp"

size_t MsgBuffer::compactable() const noexcept {
  size_t compactable = 0;
  const uint8_t *offset = m_mem;

  while (offset < m_woffset) {
    size_t len = rmsgsize(offset);
    offset += len + m_header_size;
    compactable += len + m_header_size;
  }

  return compactable;
}

size_t MsgBuffer::compact() noexcept {
  if (m_mem == m_roffset) {
    return 0;

  } else {
    size_t compactable = this->compactable();
    size_t readable = this->readable();

    if (readable > 0) {
      memmove(m_mem, m_roffset, readable);
    }

    m_roffset = m_mem;
    m_woffset = readable + m_roffset;

    return compactable;
  }
}

size_t MsgBuffer::extend(const size_t len) noexcept {
  if (len <= writable()) {
    wmsgsize(m_woffset, len);
    m_woffset += len + m_header_size;
    return len;

  } else {
    return 0;
  }
}

Status MsgBuffer::provide(uint8_t **src, const size_t intent, size_t *pbytes) noexcept {
  (void)(intent);
  *pbytes = writable();
  if (*pbytes < intent && compactable() + *pbytes >= intent) {
    compact();
    *pbytes = writable();
  }

  *src = woffset();
  *pbytes = writable();

  return OK;
}

Status MsgBuffer::write(const uint8_t *src, const size_t len, size_t *wbytes) noexcept {
  size_t writable = this->writable();
  *wbytes = 0;

  if (writable < len) {
    writable += compact();
    if (writable < len) {
      return OK;
    }

  }

  memcpy(woffset(), src, len);
  *wbytes = extend(len);
  return OK;
}

size_t MsgBuffer::consume(const size_t len) noexcept {
  size_t consumed = 0;

  for (;;) {
    size_t consumable = readable();
    if (consumable == 0 || consumed + consumable > len) {
      return consumed;
    }

    m_roffset += consumable + m_header_size;
    consumed += consumable;
  }

  return consumed;
}

Status MsgBuffer::peek(const uint8_t **dst,
                       const size_t intent,
                       size_t *pbytes) noexcept {
  (void)(intent);
  *pbytes = readable();
  *dst = roffset();

  return OK;
}

Status MsgBuffer::read(uint8_t *dst,
                       const size_t len,
                       size_t *rbytes) noexcept {
  size_t readable = this->readable();
  if (readable == 0 || readable > len) {
    *rbytes = 0;
    return OK;
  }

  memcpy(dst, m_roffset + m_header_size, readable);
  m_roffset += readable + m_header_size;
  *rbytes = readable;
  return OK;
}

