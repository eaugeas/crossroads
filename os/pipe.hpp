// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef OS_PIPE_H_
#define OS_PIPE_H_

#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "channel.hpp"
#include "status.hpp"

#include <memory>

class PipeException {
 public:
  PipeException(const char *msg, int err):
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

class Pipe final : public Channel {
 public:
  Pipe():
      m_wait_write_event(false),
      m_wait_read_event(false),
      m_err(0) {
    int res = pipe(m_fd);
    if (res == -1) {
      m_fd[0] = -1;
      m_fd[1] = -1;
      throw PipeException("failed to open pipe", errno);
    }
  }

  ~Pipe() {
    if (m_fd[0] > -1) {
      close(m_fd[0]);
      m_fd[0] = -1;
    }

    if (m_fd[1] > -1) {
      close(m_fd[1]);
      m_fd[1] = -1;
    }
  }

  Pipe(const Pipe &pipe) = delete;
  Pipe(Pipe &&pipe) {
    this->m_wait_write_event = pipe.m_wait_write_event;
    this->m_wait_read_event = pipe.m_wait_read_event;
    this->m_fd[0] = pipe.m_fd[0];
    this->m_fd[1] = pipe.m_fd[1];
    this->m_err = pipe.m_err;
    pipe.m_fd[0] = -1;
    pipe.m_fd[1] = -1;
  }

  Pipe& operator=(const Pipe &pipe) = delete;
  Pipe& operator=(Pipe &&pipe) = delete;

  inline int read_fd() const noexcept override {
    return m_fd[0];
  }

  inline int write_fd() const noexcept override {
    return m_fd[1];
  }

  inline bool wait_write_event() const noexcept override {
    return m_wait_write_event;
  }

  inline bool wait_read_event() const noexcept override {
    return m_wait_read_event;
  }

  inline int err() const noexcept {
    return m_err;
  }

  inline const struct sockaddr_in* local_address(
      socklen_t *len) const noexcept override {
    *len = 0;
    return nullptr;
  }

  inline const struct sockaddr_in* remote_address(
      socklen_t *len) const noexcept override {
    *len = 0;
    return nullptr;
  }

  inline Status rclose() noexcept {
    if (m_fd[0] > -1) {
      int res = close(m_fd[0]);
      if (res == -1) {
        return PipeRCloseFailed;
      }

      m_fd[0] = -1;
    }

    return OK;
  }

  inline Status wclose() noexcept {
    if (m_fd[1] > -1) {
      int res = close(m_fd[1]);
      if (res == -1) {
        return PipeWCloseFailed;
      }

      m_fd[1] = -1;
    }

    return OK;
  }

  Status write(const uint8_t *src,
               size_t len,
               size_t *wbytes) noexcept override;
  Status read(uint8_t *src,
              size_t len,
              size_t *rbytes) noexcept override;
 private:
  bool m_wait_write_event;
  bool m_wait_read_event;

  int m_err;
  int m_fd[2];
};

#endif  // OS_PIPE_H_
