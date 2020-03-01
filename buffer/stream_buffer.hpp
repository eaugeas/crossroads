// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef BUFFER_STREAMBUFFER_H_
#define BUFFER_STREAMBUFFER_H_

#include "buffer.hpp"
#include "dispose_func.hpp"
#include "io/recoverer.hpp"

#include <memory>

/// StreamBuffer provides a buffer implementation in which
/// the input is treated as a stream, that is, as a stream
/// of bytes without structure.
class StreamBuffer final : public Buffer {
 public:
  explicit StreamBuffer(size_t capacity):
      StreamBuffer(new uint8_t[capacity],
                   capacity,
                   array_delete_dispose_func) { }

  StreamBuffer(uint8_t *mem,
               size_t capacity,
               DisposeFunc dispose_func):
      m_capacity(capacity),
      m_mem(mem),
      m_dispose_func(dispose_func),
      m_roffset(m_mem),
      m_woffset(m_mem) { }

  ~StreamBuffer() {
    if (m_mem) {
      m_dispose_func(m_mem);
      m_mem = nullptr;
    }
  }

  StreamBuffer(const StreamBuffer &buffer) = delete;
  StreamBuffer(StreamBuffer &&buffer) {
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

  StreamBuffer& operator=(const StreamBuffer &buffer) = delete;
  StreamBuffer& operator=(StreamBuffer &&buffer) = delete;

  /// return the number of bytes that can be made available for the buffer
  inline size_t compactable() const noexcept {
    return m_roffset - m_mem;
  }

  /// returns the total capacity of the buffer
  inline size_t capacity() const noexcept override {
    return m_capacity;
  }

  /// returns the number of bytes required to read all the
  /// bytes available in the buffer
  inline size_t readable() const noexcept override {
    return m_woffset - m_roffset;
  }

  /// returns the number of bytes that can be recovered
  /// because they have been previously `consumed`
  inline size_t recoverable() const noexcept {
    return m_roffset - m_mem;
  }

  /// returns the number of bytes that be written
  /// to the buffer
  inline size_t writable() const noexcept override {
    return m_capacity - static_cast<size_t>(m_woffset - m_mem);
  }

  /// frees unused space for the buffer
  size_t compact() noexcept;
  size_t extend(const size_t len) noexcept override;
  Status provide(uint8_t **src,
                 const size_t intent,
                 size_t *pbytes) noexcept override;
  Status write(const uint8_t *src,
               const size_t len,
               size_t *wbytes) noexcept override;

  size_t recover(const size_t len) noexcept;
  size_t consume(const size_t len) noexcept override;
  Status peek(const uint8_t **dst,
              const size_t intent,
              size_t *pbytes) noexcept override;
  Status read(uint8_t *dst,
              const size_t len,
              size_t *rbytes) noexcept override;

 private:
  inline uint8_t* roffset() const {
    return m_roffset;
  }

  inline uint8_t* woffset() const {
    return m_woffset;
  }

  size_t m_capacity;

  uint8_t *m_mem;
  DisposeFunc m_dispose_func;

  uint8_t *m_roffset;
  uint8_t *m_woffset;
};

class RecovererBuffer final : public RecovererReader {
 public:
  explicit RecovererBuffer(std::unique_ptr<StreamBuffer> &&buffer):
      m_buffer(std::move(buffer)) { }

  inline size_t recover(const size_t len) noexcept override {
    return m_buffer->recover(len);
  }

  inline Status peek(const uint8_t **dst,
                     const size_t intent,
                     size_t *pbytes) noexcept override {
    return m_buffer->peek(dst, intent, pbytes);
  }

  size_t consume(const size_t len) noexcept override {
    return m_buffer->consume(len);
  }

  inline Status read(uint8_t *dst,
                     const size_t len,
                     size_t *rbytes) noexcept override {
    return m_buffer->read(dst, len, rbytes);
  }

  inline size_t recoverable() const noexcept override {
    return m_buffer->recoverable();
  }

 private:
  std::unique_ptr<StreamBuffer> m_buffer;
};

#endif  // BUFFER_STREAMBUFFER_H_
