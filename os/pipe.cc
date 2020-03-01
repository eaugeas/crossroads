// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "pipe.hpp"

Status Pipe::write(const uint8_t *src,
                   size_t len,
                   size_t *wbytes) noexcept {
  *wbytes = 0;

  while (len > 0) {
    ssize_t res = ::write(m_fd[1], src, len);

    switch (res) {
      case -1:
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          m_wait_write_event = true;
          return OK;

        } else {
          m_err = errno;
          return PipeWriteFailed;
        }
      case 0:
        return OK;

      default:
        *wbytes += res;
        len -= res;
        break;
    }
  }

  return OK;
}

Status Pipe::read(uint8_t *dst,
                  size_t len,
                  size_t *rbytes) noexcept {
  *rbytes = 0;

  while (len > 0) {
    ssize_t res = ::read(m_fd[0], dst, len);

    switch (res) {
      case -1:
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          m_wait_read_event = true;
          return OK;
        } else {
          m_err = errno;
          return PipeReadFailed;
        }

      case 0:
        return OK;

      default:
        *rbytes += res;
        len -= res;
        break;
    }
  }

  return OK;
}

