// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_BUFFEREDREADER_H_
#define IO_BUFFEREDREADER_H_

#include <memory>

#include "buffer.hpp"
#include "stream_buffer.hpp"

#include "io/reader.hpp"
#include "io/recoverer.hpp"
#include "io/source.hpp"

class BufferedReader final : public Reader {
 public:
  BufferedReader(std::unique_ptr<Source> &&source,
                 std::unique_ptr<Buffer> &&buffer):
      m_source(std::move(source)),
      m_buffer(std::move(buffer)) { }

  BufferedReader(const BufferedReader &reader) = delete;
  BufferedReader(BufferedReader &&reader):
      m_source(std::move(reader.m_source)),
      m_buffer(std::move(reader.m_buffer)) { }

  BufferedReader& operator=(const BufferedReader &reader) = delete;
  BufferedReader& operator=(const BufferedReader && reader) = delete;

  Status read(uint8_t *dst, const size_t len, size_t *rbytes) noexcept override;
  Status peek(const uint8_t **dst, const size_t intent, size_t *pbytes) noexcept override;
  size_t consume(size_t len) noexcept override;

 private:
  std::unique_ptr<Source> m_source;
  std::unique_ptr<Buffer> m_buffer;
};

class RecovererBufferedReader final : public RecovererReader {
 public:
  RecovererBufferedReader(std::unique_ptr<Source> &&source,
                          std::unique_ptr<StreamBuffer> &&buffer):
      m_source(std::move(source)),
      m_buffer(std::move(buffer)) { }

  RecovererBufferedReader(const BufferedReader &reader) = delete;
  RecovererBufferedReader(RecovererBufferedReader &&reader):
      m_source(std::move(reader.m_source)),
      m_buffer(std::move(reader.m_buffer)) { }

  RecovererBufferedReader& operator=(const RecovererBufferedReader &reader) = delete;
  RecovererBufferedReader& operator=(const RecovererBufferedReader && reader) = delete;

  Status read(uint8_t *dst, const size_t len, size_t *rbytes) noexcept override;
  Status peek(const uint8_t **dst, const size_t intent, size_t *pbytes) noexcept override;
  size_t consume(size_t len) noexcept override;
  size_t recover(size_t len) noexcept override;
  size_t recoverable() const noexcept override;

 private:
  std::unique_ptr<Source> m_source;
  std::unique_ptr<StreamBuffer> m_buffer;
};

#endif  // IO_BUFFEREDREADER_H_
