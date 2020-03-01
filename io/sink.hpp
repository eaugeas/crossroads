// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_SINK_H_
#define IO_SINK_H_

#include <stddef.h>
#include <stdint.h>

#include "status/status.hpp"

class Sink {
 public:
  Sink() = default;
  virtual ~Sink() = default;

  /// write writes as many bytes as possible from `source_t` to the writer
  /// Implementations must guarantee that 0 <= `*writtenBytes` <= length. Also in case
  /// of error `status_t` will be different from OK.
  ///
  /// A write operation is effectively a (provide + extend) operation
  virtual Status write(const uint8_t *src,
                       size_t len,
                       size_t *wbytes) noexcept = 0;
};

#endif  // IO_SINK_H_
