// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_BUFFEREDWRITER_H_
#define IO_BUFFEREDWRITER_H_

#include <memory>

#include "buffer.hpp"

#include "io/sink.hpp"
#include "io/flusher_writer.hpp"

class BufferedWriter final : public FlusherWriter {
 public:
  BufferedWriter(std::unique_ptr<Sink> &&sink,
                 std::unique_ptr<Buffer> &&buffer):
      m_sink(std::move(sink)),
      m_buffer(std::move(buffer)) { }

  BufferedWriter(const BufferedWriter &writer) = delete;
  BufferedWriter(BufferedWriter &&writer):
      m_sink(std::move(writer.m_sink)),
      m_buffer(std::move(writer.m_buffer)) { }

  BufferedWriter& operator=(const BufferedWriter &writer) = delete;
  BufferedWriter& operator=(const BufferedWriter && writer) = delete;

  Status write(const uint8_t *source,
                 size_t length,
                 size_t *wbytes) noexcept override;
  Status provide(uint8_t **source,
                   size_t intent,
                   size_t *pbytes) noexcept override;
  size_t extend(size_t length) noexcept override;
  Status flush(size_t *fbytes) noexcept override;

 private:
  std::unique_ptr<Sink> m_sink;
  std::unique_ptr<Buffer> m_buffer;
};

#endif  // IO_BUFFEREDWRITER_H_
