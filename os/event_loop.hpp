// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef OS_EVENTLOOP_H_
#define OS_EVENTLOOP_H_

#include "aio.hpp"
#include "channel.hpp"
#include "status.hpp"

#include <chrono>

class EventLoopException final {
 public:
  EventLoopException(const char *msg, int err):
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

class EventLoop final {
 public:
  enum class MonitorMode {
    level,
    edge
  };

  struct Properties final {
    struct Builder final {

      Builder &timeout(std::chrono::milliseconds timeout) {
        m_timeout = timeout;
        return *this;
      }

      Builder &inactivity(std::chrono::milliseconds inactivity) {
        m_inactivity = inactivity;
        return *this;
      }

      Builder &max_fd(int max_fd) {
        m_max_fd = max_fd;
        return *this;
      }

      Builder &event_queue_size(size_t event_queue_size) {
        m_event_queue_size = event_queue_size;
        return *this;
      }

      Properties build() {
        return Properties(m_timeout, m_inactivity,
                          m_max_fd, m_event_queue_size);
      }

      std::chrono::milliseconds m_timeout;
      std::chrono::milliseconds m_inactivity;
      int m_max_fd = 1024;
      size_t m_event_queue_size = 512;
    };

    Properties(const std::chrono::milliseconds &timeout,
               const std::chrono::milliseconds &inactivity,
               const int max_fd,
               const size_t event_queue_size):
        m_timeout(timeout),
        m_inactivity(inactivity),
        m_max_fd(max_fd),
        m_event_queue_size(event_queue_size) { }

    inline std::chrono::milliseconds timeout() const noexcept {
      return m_timeout;
    }

    inline std::chrono::milliseconds inactivity() const noexcept {
      return m_inactivity;
    }

    inline int max_fd() const noexcept {
      return m_max_fd;
    }

    inline int event_queue_size() const noexcept {
      return m_event_queue_size;
    }

    const std::chrono::milliseconds m_timeout;
    const std::chrono::milliseconds m_inactivity;
    const int m_max_fd;
    const size_t m_event_queue_size;
  };

  EventLoop();
  explicit EventLoop(const Properties &properties);
  ~EventLoop();

  EventLoop(const EventLoop &loop) = delete;
  EventLoop(EventLoop &&loop):
      m_timeout(loop.m_timeout),
      m_inactivity(loop.m_inactivity),
      m_max_fd(loop.m_max_fd),
      m_event_queue_size(loop.m_event_queue_size)
  {
    m_fd = loop.m_fd;
    loop.m_fd = -1;
  }

  EventLoop& operator=(const EventLoop &loop) = delete;
  EventLoop& operator=(EventLoop &&loop) = delete;

  /// monitor monitors the channel for read and write events
  /// with the specified MonitorMode
  Status monitor(Channel *channel, MonitorMode mode) noexcept;

  /// rmonitor monitors the channel for read events
  /// with the specified MonitorMode
  Status rmonitor(Channel *channel, MonitorMode mode) noexcept;

  /// wmonitor monitors the channel for write events
  /// with the specified MonitorMode
  Status wmonitor(Channel *channel, MonitorMode mode) noexcept;

  /// unmonitor the event loop stops
  /// monitoring any events for that channel
  Status unmonitor(Channel *channel) noexcept;

  /// runmonitor the event loop stops
  /// monitoring read events for that channel
  Status runmonitor(Channel *channel) noexcept;

  /// wunmonitor the event loop stops
  /// monitoring read events for that channel
  Status wunmonitor(Channel *channel) noexcept;

  /// run the event loop and starts processing
  /// events for the monitored channels
  Status run() noexcept;

 private:
  int m_fd;

  const std::chrono::milliseconds m_timeout;
  const std::chrono::milliseconds m_inactivity;
  const int m_max_fd;
  const size_t m_event_queue_size;
};

#endif  // OS_EVENTLOOP_H_
