// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "buffered_writer.hpp"

#include "io/copy.hpp"

static inline Status flush_buffer(
    Sink *sink,
    Buffer *buffer) {
  size_t fbytes;
  return copy(buffer, sink, &fbytes);
}

Status BufferedWriter::write(
    const uint8_t *source,
    const size_t len,
    size_t *wbytes) noexcept {
  Status status = OK;
  size_t bbytes;
  size_t remaining = len;

  *wbytes = 0;

  while (remaining > 0) {
    if (m_buffer->writable() < remaining) {
      status = flush_buffer(m_sink.get(), m_buffer.get());
      if (status->error()) {
        return status;
      }
    }

    status = m_buffer->write(source, remaining, &bbytes);
    if (status->error()) {
      return status;
    }

    source += bbytes;
    remaining -= bbytes;
    *wbytes += bbytes;
  }

  return status;
}

Status BufferedWriter::provide(
    uint8_t **source,
    size_t intent,
    size_t *pbytes) noexcept {
  if (m_buffer->writable() == 0 ||
      m_buffer->writable() < intent) {
    auto status = flush_buffer(m_sink.get(), m_buffer.get());
    if (status->error()) {
      return status;
    }
  }

  return m_buffer->provide(source, intent, pbytes);
}

size_t BufferedWriter::extend(size_t length) noexcept {
  return m_buffer->extend(length);
}

Status BufferedWriter::flush(size_t *fbytes) noexcept {
  return copy(m_buffer.get(), m_sink.get(), fbytes);
}

