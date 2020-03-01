// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "socket.hpp"

UdpSocket UdpSocket::open(const SocketDomain& domain) {
  return UdpSocket(domain);
}

UdpSocket UdpSocket::open_ipv4() {
  return UdpSocket::open(SocketDomain::IPv4);
}

UdpSocket UdpSocket::open_ipv6() {
  return UdpSocket::open(SocketDomain::IPv6);
}

std::unique_ptr<UdpSocket> UdpSocket::open_ptr(const SocketDomain& domain) {
  return std::make_unique<UdpSocket>(domain);
}

std::unique_ptr<UdpSocket> UdpSocket::open_ipv4_ptr() {
  return UdpSocket::open_ptr(SocketDomain::IPv4);
}

std::unique_ptr<UdpSocket> UdpSocket::open_ipv6_ptr() {
  return UdpSocket::open_ptr(SocketDomain::IPv6);
}

Status UdpSocket::write(const uint8_t *src,
                        size_t len,
                        size_t *wbytes) noexcept {
  *wbytes = 0;

  ssize_t res = ::sendto(
      m_sockfd, src, len, 0,
      reinterpret_cast<const struct sockaddr*>(&m_remote_address),
      sizeof(struct sockaddr_in));

  switch (res) {
    case -1:
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        m_wait_write_event = true;
        return OK;

      } else {
        return SocketWriteFailed;
      }
    case 0:
      return OK;

    default:
      *wbytes += res;
      src += res;
      len -= res;
      break;
  }

  return OK;
}

Status UdpSocket::read(uint8_t *dst,
                       size_t len,
                       size_t *rbytes) noexcept {
  *rbytes = 0;
  socklen_t socklen = sizeof(struct sockaddr_in);

  ssize_t res = ::recvfrom(
      m_sockfd, dst, len, 0,
      reinterpret_cast<struct sockaddr*>(&m_remote_address),
      &socklen);

  switch (res) {
    case -1:
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        m_wait_read_event = true;
        return OK;
      } else {
        m_err = errno;
        return SocketReadFailed;
      }

    case 0:
      break;

    default:
      *rbytes += res;
      dst += res;
      len -= res;
      break;
  }

  return OK;
}


TcpSocket TcpSocket::open(const SocketDomain& domain) {
  return TcpSocket(domain);
}

TcpSocket TcpSocket::open_ipv4() {
  return TcpSocket::open(SocketDomain::IPv4);
}

TcpSocket TcpSocket::open_ipv6() {
  return TcpSocket::open(SocketDomain::IPv6);
}

std::unique_ptr<TcpSocket> TcpSocket::open_ptr(const SocketDomain& domain) {
  return std::make_unique<TcpSocket>(domain);
}

std::unique_ptr<TcpSocket> TcpSocket::open_ipv4_ptr() {
  return TcpSocket::open_ptr(SocketDomain::IPv4);
}

std::unique_ptr<TcpSocket> TcpSocket::open_ipv6_ptr() {
  return TcpSocket::open_ptr(SocketDomain::IPv6);
}


Status TcpSocket::write(const uint8_t *src,
                        size_t len,
                        size_t *wbytes) noexcept {
  *wbytes = 0;

  while (len > 0) {
    ssize_t res = ::send(m_sockfd, src, len, 0);

    switch (res) {
      case -1:
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          m_wait_write_event = true;
          return OK;

        } else {
          m_err = errno;
          return SocketWriteFailed;
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

Status TcpSocket::read(uint8_t *dst,
                       size_t len,
                       size_t *rbytes) noexcept {
  *rbytes = 0;

  while (len > 0) {
    ssize_t res = ::recv(m_sockfd, dst, len, 0);

    switch (res) {
      case -1:
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          m_wait_read_event = true;
          return OK;
        } else {
          m_err = errno;
          return SocketReadFailed;
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

