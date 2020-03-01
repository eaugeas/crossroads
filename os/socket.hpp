// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef OS_SOCKET_H_
#define OS_SOCKET_H_

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <memory>

#include "aio.hpp"
#include "channel.hpp"
#include "status.hpp"

class SocketException {
 public:
  SocketException(const char *msg, int err):
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

enum class SocketDomain {
  IPv4 = AF_INET,
  IPv6 = AF_INET6
};

class Socket : public Channel {
 public:
  Socket() = default;
  virtual ~Socket() = default;

  /// wait_write_event returns true if writing to the socket asynchronously
  /// would block
  bool wait_write_event() const noexcept;
  /// wait_read_event returns true if read from the socket asynchronously
  /// would block
  bool wait_read_event() const noexcept;

  /// local_address returns the local address of the socket
  /// if bound to interface and port
  const struct sockaddr_in *local_address(socklen_t *len) const noexcept;

  /// remote_address returns the remote address the socket
  /// has last interacted with
  const struct sockaddr_in *remote_address(socklen_t *len) const noexcept;
};

class UdpSocket final : public Socket {
 public:
  explicit UdpSocket(const SocketDomain& domain):
      m_wait_write_event(false),
      m_wait_read_event(false),
      m_err(0),
      m_sockfd(-1) {
    m_sockfd = aio_socket(static_cast<int>(domain), SOCK_DGRAM, 0);
    if (m_sockfd == -1) {
      throw SocketException("failed to open socket", errno);
    }

    memset(&m_local_address, 0, sizeof(struct sockaddr_in));
    memset(&m_remote_address, 0, sizeof(struct sockaddr_in));
  }

  ~UdpSocket() {
    if (m_sockfd > -1) {
      close(m_sockfd);
      m_sockfd = -1;
    }
  }

  UdpSocket(const UdpSocket &socket) = delete;
  UdpSocket(UdpSocket &&socket) {
    this->m_sockfd = socket.m_sockfd;
    memcpy(&this->m_local_address, &socket.m_local_address, sizeof(struct sockaddr_in));
    memcpy(&this->m_remote_address, &socket.m_remote_address, sizeof(struct sockaddr_in));
    socket.m_sockfd = -1;
  }

  UdpSocket& operator=(const UdpSocket &socket) = delete;
  UdpSocket& operator=(UdpSocket &&socket) = delete;

  static UdpSocket open(const SocketDomain& domain);
  static UdpSocket open_ipv4();
  static UdpSocket open_ipv6();

  static std::unique_ptr<UdpSocket> open_ptr(const SocketDomain& domain);
  static std::unique_ptr<UdpSocket> open_ipv4_ptr();
  static std::unique_ptr<UdpSocket> open_ipv6_ptr();

  inline bool wait_write_event() const noexcept override {
    return m_wait_write_event;
  }

  inline bool wait_read_event() const noexcept override {
    return m_wait_read_event;
  }

  inline int read_fd() const noexcept override {
    return m_sockfd;
  }

  inline int write_fd() const noexcept override {
    return m_sockfd;
  }

  inline const struct sockaddr_in* local_address(
      socklen_t *len) const noexcept override {
    *len = m_local_address_len;
    return &m_local_address;
  }

  inline const struct sockaddr_in* remote_address(
      socklen_t *len) const noexcept override {
    *len = m_remote_address_len;
    return &m_remote_address;
  }

  inline void set_remote_address(const struct sockaddr_in addr,
                                 socklen_t socklen) noexcept {
    memcpy(&m_remote_address, &addr, socklen);
  }

  /// err returns the errno value in case a syscall has failed
  /// during the last syscall
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
  bool m_wait_write_event;
  bool m_wait_read_event;

  int m_err;
  int m_sockfd;

  socklen_t m_local_address_len;
  socklen_t m_remote_address_len;
  struct sockaddr_in m_local_address;
  struct sockaddr_in m_remote_address;
};

class TcpSocket final : public Socket {
 public:
  explicit TcpSocket(const SocketDomain& domain):
      m_wait_write_event(false),
      m_wait_read_event(false),
      m_err(0),
      m_sockfd(-1) {
    m_sockfd = aio_socket(static_cast<int>(domain), SOCK_STREAM, 0);
    if (m_sockfd == -1) {
      throw SocketException("failed to open socket", errno);
    }

    memset(&m_local_address, 0, sizeof(struct sockaddr_in));
    memset(&m_remote_address, 0, sizeof(struct sockaddr_in));
  }

  ~TcpSocket() {
    if (m_sockfd > -1) {
      close(m_sockfd);
      m_sockfd = -1;
    }
  }

  TcpSocket(const TcpSocket &socket) = delete;
  TcpSocket(TcpSocket &&socket) {
    this->m_sockfd = socket.m_sockfd;
    memcpy(&this->m_local_address, &socket.m_local_address, sizeof(struct sockaddr_in));
    memcpy(&this->m_remote_address, &socket.m_remote_address, sizeof(struct sockaddr_in));
    socket.m_sockfd = -1;
  }

  TcpSocket& operator=(const TcpSocket &socket) = delete;
  TcpSocket& operator=(TcpSocket &&socket) = delete;

  static TcpSocket open(const SocketDomain& domain);
  static TcpSocket open_ipv4();
  static TcpSocket open_ipv6();
  static std::unique_ptr<TcpSocket> open_ptr(const SocketDomain& domain);
  static std::unique_ptr<TcpSocket> open_ipv4_ptr();
  static std::unique_ptr<TcpSocket> open_ipv6_ptr();

  inline bool wait_write_event() const noexcept override {
    return m_wait_write_event;
  }

  inline bool wait_read_event() const noexcept override {
    return m_wait_read_event;
  }

  inline const struct sockaddr_in* local_address(
      socklen_t *len) const noexcept override {
    *len = m_local_address_len;
    return &m_local_address;
  }

  inline const struct sockaddr_in* remote_address(
      socklen_t *len) const noexcept override {
    *len = m_remote_address_len;
    return &m_remote_address;
  }

  /// err returns the errno value in case a syscall has failed
  /// during the last syscall
  inline int err() const noexcept {
    return m_err;
  }

  /// shutdown_r closes the read part of the socket
  /// so that all subsequent calls to read from
  /// the socket will fail
  Status shutdown_r() noexcept {
    int res = shutdown(m_sockfd, SHUT_RD);
    if (res == -1) {
      m_err = errno;
      return SocketShutdownFailed;
    } else {
      return OK;
    }
  }

  /// shutdown_w closes the write part of the socket
  /// so that all subsequent calls to write on the
  /// socket will fail
  Status shutdown_w() noexcept {
    int res = shutdown(m_sockfd, SHUT_WR);
    if (res == -1) {
      m_err = errno;
      return SocketShutdownFailed;
    } else {
      return OK;
    }
  }

  /// shutdown_rw closes the read and write parts
  /// of the socket so that all subsequent calls
  /// to write on the socket will fail
  Status shutdown_rw() noexcept {
    int res = shutdown(m_sockfd, SHUT_RDWR);
    if (res == -1) {
      m_err = errno;
      return SocketShutdownFailed;
    } else {
      return OK;
    }
  }

  inline int read_fd() const noexcept override {
    return m_sockfd;
  }

  inline int write_fd() const noexcept override {
    return m_sockfd;
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
  int m_sockfd;

  socklen_t m_local_address_len;
  socklen_t m_remote_address_len;
  struct sockaddr_in m_local_address;
  struct sockaddr_in m_remote_address;
};

#endif  // OS_SOCKET_H_
