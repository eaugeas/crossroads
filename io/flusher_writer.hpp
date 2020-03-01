// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_FLUSHERWRITER_H_
#define IO_FLUSHERWRITER_H_

#include <stddef.h>

#include "writer.hpp"
#include "flusher.hpp"

class FlusherWriter : public Writer, public Flusher {
public:
  FlusherWriter() = default;
  virtual ~FlusherWriter() = default;
};

#endif  // IO_FLUSHERWRITER_H_
