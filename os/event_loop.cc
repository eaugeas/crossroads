// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "event_loop.hpp"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "log/log.hpp"

EventLoop::EventLoop():
    EventLoop(EventLoop::Properties::Builder().build()) { }

EventLoop::EventLoop(const EventLoop::Properties &properties):
    m_timeout(properties.timeout()),
    m_inactivity(properties.inactivity()),
    m_max_fd(properties.max_fd()),
    m_event_queue_size(properties.event_queue_size())
{
  const int fd = aio_create();
  if (fd == -1) {
    throw EventLoopException("failed to open fd for event loop", errno);
  }

  m_fd = fd;
}

EventLoop::~EventLoop() {
  if (m_fd > -1) {
    ::close(m_fd);
  }
}

Status EventLoop::monitor(Channel *channel, MonitorMode mode) noexcept {
  if (channel->write_fd() < 0 || channel->read_fd() < 0) {
    return ArgInvalidFD;
  }

  const auto edge = mode == MonitorMode::edge;
  if (channel->write_fd() != channel->read_fd()) {
    int res = aio_wmonit(m_fd, channel->write_fd(), channel->read_fd(), mode == MonitorMode::edge);
    if (res == -1) {
      LTRACE("MonitWriteFD", "fd: %d, msg: %s, err: %s",
             channel->write_fd(),
             "failed to monitor write file descriptor", strerror(errno));
      return EventLoopMonitorFDFailed;
    }

    res = aio_rmonit(m_fd, channel->read_fd(), channel->read_fd(), mode == MonitorMode::edge);
    if (res == -1) {
      LTRACE("MonitReadFD", "fd: %d, msg: %s, err: %s",
             channel->read_fd(),
             "failed to monitor read file descriptor", strerror(errno));
      aio_wunmonit(m_fd, channel->write_fd());
      return EventLoopMonitorFDFailed;
    }

  } else {
    int res = aio_monit(m_fd, channel->read_fd(), channel->read_fd(), edge);
    if (res == -1) {
      LTRACE("MonitFD", "fd: %d, msg: %s, err: %s",
             channel->read_fd(),
             "failed to monitor file descriptor", strerror(errno));
      return EventLoopMonitorFDFailed;
    }
  }

  return OK;
}

Status EventLoop::unmonitor(Channel *channel) noexcept {
  if (channel->write_fd() < 0 || channel->read_fd() < 0) {
    return ArgInvalidFD;
  }

  if (channel->write_fd() != channel->read_fd()) {
    int res = aio_wunmonit(m_fd, channel->write_fd());
    res = res == -1 && (res = aio_runmonit(
        m_fd, channel->read_fd())) == 0 ? -1 : res;
    if (res == -1) {
      return EventLoopMonitorFDFailed;
    }

  } else {
    int res = aio_unmonit(m_fd, channel->read_fd());
    if (res == -1) {
      return EventLoopUnmonitorFDFailed;
    }
  }

  return OK;
}

Status EventLoop::rmonitor(Channel *channel, MonitorMode mode) noexcept {
  if (channel->read_fd() < 0) {
    return ArgInvalidFD;
  }

  auto edge = MonitorMode::edge == mode;
  const int res = aio_rmonit(m_fd, channel->read_fd(), channel->read_fd(), edge);
  if (res == -1) {
    return EventLoopUnmonitorFDFailed;
  }

  return OK;
}

Status EventLoop::wmonitor(Channel *channel, MonitorMode mode) noexcept {
  if (channel->write_fd() < 0) {
    return ArgInvalidFD;
  }

  auto edge = MonitorMode::edge == mode;
  const int res = aio_wmonit(m_fd, channel->write_fd(), channel->write_fd(), edge);
  if (res == -1) {
    return EventLoopUnmonitorFDFailed;
  }

  return OK;
}

Status EventLoop::runmonitor(Channel *channel) noexcept {
  if (channel->read_fd() < 0) {
    return ArgInvalidFD;
  }

  const int res = aio_runmonit(m_fd, channel->read_fd());
  if (res == -1) {
    return EventLoopUnmonitorFDFailed;
  }

  return OK;
}

Status EventLoop::wunmonitor(Channel *channel) noexcept {
  if (channel->write_fd() < 0) {
    return ArgInvalidFD;
  }

  int res = aio_runmonit(m_fd, channel->write_fd());
  if (res == -1) {
    return EventLoopUnmonitorFDFailed;
  }

  return OK;
}

Status EventLoop::run() noexcept {
  
}
