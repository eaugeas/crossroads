// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_SOURCE_H_
#define IO_SOURCE_H_

#include <stddef.h>
#include <stdint.h>

#include "status/status.hpp"

class Source {
public:
  Source() = default;
  virtual ~Source() = default;

  /// read copies all the available bytes in the reader to `destination`.
  /// It is guaranted that 0 <= `///readBytes` <= length. Also in case
  /// of error `status_t` will be different from ok.
  ///
  /// A read operation is effectively a (peek + consume) operation
  virtual Status read(uint8_t *src,
                      size_t len,
                      size_t *rbytes) noexcept = 0;
};

#endif  // IO_SOURCE_H
