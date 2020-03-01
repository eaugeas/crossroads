// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "file_stream.hpp"

#include "status.hpp"

FileStream FileStream::open(
    const char *name,
    int flags,
    mode_t mode) {
  return FileStream(name, flags, mode);
}

std::unique_ptr<FileStream> FileStream::open_ptr(
    const char *name,
    int flags,
    mode_t mode) {
  return std::make_unique<FileStream>(name, flags, mode);
}

FileStream FileStream::open_read(const char *name) {
  return open(name, O_RDONLY | O_CLOEXEC, 0666);
}
std::unique_ptr<FileStream> FileStream::open_read_ptr(const char *name) {
  return open_ptr(name, O_RDONLY | O_CLOEXEC, 0666);
}

FileStream FileStream::open_write(const char *name) {
  return open(name, O_WRONLY | O_CREAT | O_CLOEXEC | O_TRUNC, 0666);
}

std::unique_ptr<FileStream> FileStream::open_write_ptr(const char *name) {
  return open_ptr(name, O_WRONLY | O_CREAT | O_CLOEXEC | O_TRUNC, 0666);
}

#ifdef __linux__
FileStream FileStream::create_tmp(const char *dir) {
  return open(dir, O_TMPFILE | O_WRONLY | O_CLOEXEC, 0666);
}

std::unique_ptr<FileStream> FileStream::create_tmp_ptr(const char *dir) {
  return open_ptr(dir, O_TMPFILE | O_WRONLY | O_CLOEXEC, 0666);
}
#endif

Status FileStream::write(const uint8_t *src,
                         size_t len,
                         size_t *wbytes) noexcept {
  ssize_t res = ::write(m_fd, src, len);

  if (res < 0) {
    m_err = errno;
    *wbytes = 0;
    return FileWriteFailed;

  } else {
    *wbytes = static_cast<size_t>(res);
    return OK;
  }
}

Status FileStream::read(uint8_t *dst,
                        size_t len,
                        size_t *rbytes) noexcept {
  ssize_t res = ::read(m_fd, dst, len);

  if (res < 0) {
    m_err = errno;
    *rbytes = 0;
    return FileReadFailed;

  } else {
    *rbytes = static_cast<size_t>(res);
    return OK;
  }
}
