// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_FLUSHER_H_
#define IO_FLUSHER_H_

#include <stddef.h>
#include <stdint.h>

#include "status/status.hpp"

class Flusher {
public:
  Flusher() = default;
  virtual ~Flusher() = default;

  /// flush flushes any buffered content to the sink
  /// associated with the implementing object
  virtual Status flush(size_t *fbytes) noexcept = 0;
};

#endif  // IO_FLUSHER_H_
