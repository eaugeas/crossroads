// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "buffered_reader.hpp"
#include "io/copy.hpp"

static inline Status load_buffer(Source *source, Buffer *buffer) {
  size_t cpbytes;
  return copy(source, buffer, &cpbytes);
}

static inline Status _read(
    Buffer *buffer,
    Source *source,
    uint8_t *dst,
    const size_t len,
    size_t *rbytes) {
  if (buffer->readable() < len) {
    auto status = load_buffer(source, buffer);
    if (status->error()) {
      return status;
    }
  }

  return buffer->read(dst, len, rbytes);
}

static inline Status _peek(
    Buffer *buffer,
    Source *source,
    const uint8_t **dst,
    const size_t intent,
    size_t *pbytes) {
  if (buffer->readable() == 0 ||
      buffer->readable() < intent ||
      intent == 0) {
    auto status = load_buffer(source, buffer);
    if (status->error()) {
      return status;
    }
  }

  return buffer->peek(dst, intent, pbytes);
}

Status BufferedReader::read(
    uint8_t *dst,
    const size_t len,
    size_t *rbytes) noexcept {
  return _read(m_buffer.get(), m_source.get(), dst, len, rbytes);
}

Status BufferedReader::peek(
    const uint8_t **dst,
    const size_t intent,
    size_t *pbytes) noexcept {
  return _peek(m_buffer.get(), m_source.get(), dst, intent, pbytes);
}

size_t BufferedReader::consume(const size_t len) noexcept {
  return m_buffer->consume(len);
}

Status RecovererBufferedReader::read(
    uint8_t *dst,
    const size_t len,
    size_t *rbytes) noexcept {
  return _read(m_buffer.get(), m_source.get(), dst, len, rbytes);
}

Status RecovererBufferedReader::peek(
    const uint8_t **dst,
    const size_t intent,
    size_t *pbytes) noexcept {
  return _peek(m_buffer.get(), m_source.get(), dst, intent, pbytes);
}

size_t RecovererBufferedReader::recover(size_t len) noexcept {
  return m_buffer->recover(len);
};

size_t RecovererBufferedReader::recoverable() const noexcept {
  return m_buffer->recoverable();
};

size_t RecovererBufferedReader::consume(size_t len) noexcept {
  return m_buffer->consume(len);
}

