// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_RECOVERER_H_
#define IO_RECOVERER_H_

#include <stddef.h>
#include <stdint.h>

#include "status/status.hpp"

#include "reader.hpp"

class RecovererReader : public Reader {
public:
  RecovererReader() = default;
  virtual ~RecovererReader() = default;

  /// recover moves the read offset backwards at most `length` bytes. It returns
  /// the number of bytes the read offset was actually moved. Recover is the
  /// opposite operation of a consume for a `Reader`.
  /// A `recover` operation of len `n` preceeded by a `consume` operation
  /// of len `n` should leave the state of the Reader the same
  /// as it was before the `consume` operation
  virtual size_t recover(size_t len) noexcept = 0;

  /// recoverable returns the number of bytes that can be recovered
  /// because they have been previously `consumed`
  virtual size_t recoverable() const noexcept = 0;
};

#endif  // IO_RECOVERER_H
