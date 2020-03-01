// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef OS_CHANNEL_H_
#define OS_CHANNEL_H_

#include "aio.hpp"
#include "io/sink.hpp"
#include "io/source.hpp"

class Channel : public Sink, public Source  {
 public:
  Channel() = default;
  virtual ~Channel() = default;

  virtual int read_fd() const noexcept = 0;
  virtual int write_fd() const noexcept = 0;
  virtual bool wait_write_event() const noexcept = 0;
  virtual bool wait_read_event() const noexcept = 0;
  virtual const struct sockaddr_in* local_address(socklen_t *len) const noexcept = 0;
  virtual const struct sockaddr_in* remote_address(socklen_t *len) const noexcept = 0;
};

#endif  // OS_CHANNEL_H_

