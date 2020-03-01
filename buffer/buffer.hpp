// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef BUFFER_BUFFER_H_
#define BUFFER_BUFFER_H_

#include <stddef.h>

#include "io/reader.hpp"
#include "io/writer.hpp"

class Buffer : public Reader, public Writer {
 public:
  Buffer() = default;
  virtual ~Buffer() = default;

  /// returns the current number of bytes that at least
  /// can be read from the buffer
  virtual size_t readable() const noexcept = 0;

  /// returns the current number of bytes that at most
  /// can be written to the buffer
  virtual size_t writable() const noexcept = 0;

  /// returns the total capacity of the buffer
  virtual size_t capacity() const noexcept = 0;
};

#endif  // BUFFER_BUFFER_H_
