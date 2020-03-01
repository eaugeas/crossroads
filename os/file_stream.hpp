// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef OS_FILESTREAM_H_
#define OS_FILESTREAM_H_

#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "io/sink.hpp"
#include "io/source.hpp"

#include <memory>

class FileException {
 public:
  FileException(const char *msg, int err):
      m_msg(msg),
      m_err(err){ }

  inline const char *msg() const noexcept {
    return m_msg;
  }

  inline int err() const noexcept {
    return m_err;
  }

 private:
  const char *m_msg;
  const int m_err;
};

class FileStream final : public Sink, public Source {
 public:
  FileStream(const char *pathname, int flags, mode_t mode):
      m_err(0) {
    m_fd = ::open(pathname, flags, mode);
    if (m_fd == -1) {
      throw FileException("failed to open file", errno);
    }
  }

  ~FileStream() {
    if (m_fd > -1) {
      close(m_fd);
      m_fd = -1;
    }
  }

  FileStream(const FileStream &file) = delete;
  FileStream(FileStream &&file) {
    this->m_fd = file.m_fd;
    this->m_err = file.m_err;
    file.m_fd = -1;
  }

  FileStream& operator=(const FileStream &file) = delete;
  FileStream& operator=(FileStream &&file) = delete;

  static FileStream open(
      const char *pathname, int flags, mode_t mode);
  static std::unique_ptr<FileStream> open_ptr(
      const char *pathname, int flags, mode_t mode);

  static FileStream open_read(const char *pathname);
  static std::unique_ptr<FileStream> open_read_ptr(const char *pathname);

  static FileStream open_write(const char *pathname);
  static std::unique_ptr<FileStream> open_write_ptr(const char *pathname);

  static FileStream create(const char *pathname);
  static std::unique_ptr<FileStream> create_ptr(const char *pathname);

  static FileStream create_tmp(const char *dir);
  static std::unique_ptr<FileStream> create_tmp_ptr(const char *dir);

  inline int err() const noexcept {
    return m_err;
  }

  Status write(const uint8_t *src,
               size_t len,
               size_t *wbytes) noexcept;
  Status read(uint8_t *src,
              size_t len,
              size_t *rbytes) noexcept;
 private:
  int m_err;
  int m_fd;
};


#endif  // OS_FILESTREAM_H_

