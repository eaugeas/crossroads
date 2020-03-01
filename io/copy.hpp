// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_COPY_H_
#define IO_COPY_H_

#include <stddef.h>

#include "status/status.hpp"

#include "reader.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "writer.hpp"
#include "flusher_writer.hpp"

/// copy copies content from a Source/Reader to a Sink/Writer
/// using the internal buffer provided by the Reader/Writer
/// passed as argument. It returns the number of bytes successfully
/// copied in cbytes
Status copy(Reader *reader, Sink *sink, size_t *cbytes) noexcept;
Status copy(Source *source, Writer *writer, size_t *cbytes) noexcept;
Status copy(Source *source, FlusherWriter *fwriter, size_t *cbytes) noexcept;
Status copy(const uint8_t *src, size_t len, Writer *writer, size_t *cbytes) noexcept;
Status copy(const uint8_t *src, size_t len, FlusherWriter *writer, size_t *cbytes) noexcept;
Status copy(Reader *reader, uint8_t *dst, size_t len, size_t *cbytes) noexcept;

#endif  // IO_COPY_H_
