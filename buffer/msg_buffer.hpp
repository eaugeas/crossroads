// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef BUFFER_MSGBUFFER_H_
#define BUFFER_MSGBUFFER_H_

#include <algorithm>

#include "buffer.hpp"
#include "dispose_func.hpp"
#include "memio.hpp"

/// MsgBuffer provides a buffer implementation in which
/// the input is treated as a sequence of messages. Each
/// messages has a size, and a message can only be written
/// to the buffer if the buffer has enough capacity left.
/// Likewise, a message can only be read from the buffer
/// if a big enough array is provided to copy the buffer to
class MsgBuffer final : public Buffer {
 public:
  explicit MsgBuffer(size_t capacity):
      MsgBuffer(new uint8_t[capacity],
                capacity,
                array_delete_dispose_func) { }

  MsgBuffer(uint8_t *mem,
            size_t capacity,
            DisposeFunc dispose_func):
      m_header_size(DEFAULT_HEADER_SIZE),
      m_capacity(capacity),
      m_mem(mem),
      m_dispose_func(dispose_func),
      m_roffset(m_mem),
      m_woffset(m_mem) { }

  MsgBuffer(uint8_t header_size,
            uint8_t *mem,
            size_t capacity,
            DisposeFunc dispose_func):
      m_header_size(header_size),
      m_capacity(capacity),
      m_mem(mem),
      m_dispose_func(dispose_func),
      m_roffset(m_mem),
      m_woffset(m_mem) { }

  ~MsgBuffer() {
    if (m_mem) {
      m_dispose_func(m_mem);
      m_mem = nullptr;
    }
  }

  MsgBuffer(const MsgBuffer &buffer) = delete;
  MsgBuffer(MsgBuffer &&buffer) {
    this->m_header_size = buffer.m_header_size;
    this->m_capacity = buffer.m_capacity;
    this->m_mem = buffer.m_mem;
    this->m_dispose_func = buffer.m_dispose_func;
    this->m_roffset = buffer.m_roffset;
    this->m_woffset = buffer.m_woffset;
    buffer.m_mem = nullptr;
    buffer.m_capacity = 0;
    buffer.m_roffset = nullptr;
    buffer.m_woffset = nullptr;
  }

  MsgBuffer& operator=(const MsgBuffer &buffer) = delete;
  MsgBuffer& operator=(MsgBuffer &&buffer) = delete;

  /// returns the total number of bytes available
  /// the buffer has for the usage of the user
  inline size_t capacity() const noexcept override {
    size_t capacity = m_capacity - m_header_size;
    return capacity > m_capacity ? 0 : capacity;
  }

  /// returns the number of bytes that the next message that
  /// can be read requires
  inline size_t readable() const noexcept override {
    return m_woffset < m_roffset + m_header_size ? 0 : rmsgsize(m_roffset);
  }

  /// returns the number of bytes that the next message
  /// that is written to the buffer can occuppy at most
  inline size_t writable() const noexcept override {
    size_t remaining = this->remaining();
    return remaining > m_header_size ? remaining - m_header_size : 0;
  }

  /// return the number of bytes that can be made available for the buffer
  size_t compactable() const noexcept;

  /// frees unused space for the buffer
  size_t compact() noexcept;

  size_t extend(const size_t len) noexcept override;
  Status provide(uint8_t **src, const size_t intent, size_t *pbytes) noexcept override;
  Status write(const uint8_t *src, const size_t len, size_t *wbytes) noexcept override;

  size_t consume(const size_t len) noexcept override;
  Status peek(const uint8_t **dst,
              const size_t intent,
              size_t *pbytes) noexcept override;
  Status read(uint8_t *dst,
              const size_t len,
              size_t *rbytes) noexcept override;

 private:
  static constexpr int DEFAULT_HEADER_SIZE = 4;

  inline uint8_t* roffset() const {
    return readable() < m_header_size
                        ? m_roffset
                        : m_roffset + m_header_size;
  }

  inline uint8_t* woffset() const {
    return writable() < m_header_size
                        ? m_woffset
                        : m_woffset + m_header_size;
  }

  inline size_t remaining() const {
    return m_capacity - (m_woffset - m_mem);
  }

  static inline uint32_t wmsgsize(uint8_t *ptr,
                                  const uint32_t len) noexcept {
    return writeu32(ptr, len) - ptr;
  }

  static inline uint32_t rmsgsize(const uint8_t *ptr) noexcept {
    uint32_t value;
    readu32(ptr, &value);
    return value;
  }

  uint8_t m_header_size;
  size_t m_capacity;

  uint8_t *m_mem;
  DisposeFunc m_dispose_func;

  uint8_t *m_roffset;
  uint8_t *m_woffset;
};

#endif  // BUFFER_MSGBUFFER_H_
