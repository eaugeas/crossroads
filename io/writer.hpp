// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_WRITER_H_
#define IO_WRITER_H_

#include <stddef.h>

#include "status/status.hpp"
#include "sink.hpp"

class Writer : public Sink {
 public:
  Writer() = default;
  virtual ~Writer() = default;

  /// provide provides a memory address that the caller can write to.
  /// Check `pbytes` first before writing to `source`, and do
  /// not use `source` if `///pbytes` == 0. Provide does not move the write offset
  /// pointer of the Writer, which means that multiple calls to provide will yield the
  /// same result. `intent` sets the number of bytes that the caller
  /// intends to write. If it is 0, the reader returns the memory space
  /// already available. Otherwise, the reader attempts to provide a memory
  /// space big enough for the write intent. See `Extend` for advancing the write offset
  ///
  /// Implementations must guarantee that 0 <= `*pbytes`. Also in case
  /// of error `status_t` will be different from OK
  virtual Status provide(uint8_t **source,
                         size_t intent,
                         size_t *pbytes) noexcept = 0;

  /// extend moves the read offset forward at most `length` bytes.
  /// If `length` <= ///providedBytes, it is guaranteed that the writer_t will
  /// fully extend to `length`, otherwise, the result of the `extend`
  /// operation may be smaller than the provided `length`.
  virtual size_t extend(size_t len) noexcept = 0;
};

#endif  // IO_WRITER_H_
