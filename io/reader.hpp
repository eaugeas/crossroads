// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_READER_H_
#define IO_READER_H_

#include <stddef.h>
#include <stdint.h>

#include "status/status.hpp"

#include "source.hpp"

class Reader : public Source {
public:
  Reader() = default;
  virtual ~Reader() = default;

  /// peek provides a constant pointer to the memory address where there's available.
  /// data to read. Check `pbytes` first before reading from destination, and do
  /// not use `destination` if pbytes == 0. Peek does not move the read offset
  /// pointer of the reader_t, which means that multiple calls to peek will yield the
  /// dst. Please note that it is fine if multiple consecutive calls to peek yield
  /// a result with the same value for dst, but `pbytes` is different.
  /// See `consume` for advancing the Read Pointer. `intent` is used
  /// to signal the intent of how many bytes are expected to be peeked. intent == 0
  /// should be treated as a special case in which all possible bytes the reader can
  /// provide should be made available.
  ///
  ///
  /// It is guaranted that 0 <= `*pbytes`. Also in case
  /// of error `status_t` will be different from OK
  ///
  /// Example of intent:
  ///
  /// const uint8_t *dst;
  /// size_t pbytes;
  /// Reader *reader;
  /// reader->peek(dst, 1, &pbytes); // pbytes must be at least 1 if possible
  /// reader->peek(dst, 0, &pbytes); // pbytes must be at least the previous pbytes
  ///                                // and at most all bytes that can be provided
  ///                                // at once by the reader
  virtual Status peek(const uint8_t **dst,
                      size_t intent,
                      size_t *pbytes) noexcept = 0;

  /// consume moves the read offset forward at most `length` bytes. It returns
  /// the number of bytes the read offset was actually moved
  virtual size_t consume(size_t len) noexcept = 0;
};

#endif  // IO_READER_H
