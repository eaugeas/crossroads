#include "stream_buffer.hpp"

#include <algorithm>
#include <string.h>

size_t StreamBuffer::compact() noexcept {
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

size_t StreamBuffer::extend(const size_t len) noexcept {
  size_t writable = this->writable();
  size_t extendable = std::min(len, writable);
  m_woffset += extendable;
  return extendable;
}

Status StreamBuffer::provide(uint8_t **src,
                             const size_t intent,
                             size_t *pbytes) noexcept {
  *pbytes = writable();

  bool should_compact = (*pbytes < intent && compactable() + *pbytes >= intent) ||
                        intent == 0;
  if (should_compact) {
    compact();
    *pbytes = writable();
  }

  *src = woffset();
  *pbytes = writable();

  return OK;
}

Status StreamBuffer::write(const uint8_t *src,
                           const size_t len,
                           size_t *wbytes) noexcept {
  size_t writable = this->writable();
  size_t copiable;

  if (writable < len) {
    writable += compact();
  }

  copiable = std::min(writable, len);
  memcpy(m_woffset, src, copiable);

  *wbytes = copiable;
  m_woffset += copiable;

  return OK;
}

size_t StreamBuffer::recover(const size_t len) noexcept {
  size_t recoverable = std::min(len, this->recoverable());
  m_roffset -= recoverable;
  return recoverable;
}

size_t StreamBuffer::consume(const size_t len) noexcept {
  size_t consumable = std::min(len, this->readable());
  m_roffset += consumable;
  return consumable;
}

Status StreamBuffer::peek(const uint8_t **dst,
                            const size_t intent,
                            size_t *pbytes) noexcept {
  (void)(intent);
  *dst = m_roffset;
  *pbytes = readable();

  return OK;
}

Status StreamBuffer::read(uint8_t *dst,
                            const size_t len,
                            size_t *rbytes) noexcept {
  size_t copiable;

  copiable = std::min(readable(), len);
  memcpy(dst, m_roffset, copiable);

  m_roffset += copiable;
  *rbytes = copiable;

  return OK;
}
