#include "copy.hpp"

#include <algorithm>
#include <string.h>

#include <stdio.h>

Status copy(Reader *reader, Sink *sink, size_t *cbytes) noexcept {
  const uint8_t *buffer;
  size_t written, pbytes;
  Status status = OK;

  *cbytes = 0;

  for (;;) {
    status = reader->peek(&buffer, 0, &pbytes);
    if (status->error() || pbytes == 0) {
      break;
    }

    status = sink->write(buffer, pbytes, &written);
    if (status->error() || written == 0) {
      break;
    }

    *cbytes += reader->consume(written);
    if (written < pbytes) {
      break;
    }
  }

  return status;
}

Status copy(Source *source, Writer *writer, size_t *cbytes) noexcept {
  Status status = OK;
  uint8_t *buffer;
  size_t wbytes, rbytes;

  *cbytes = 0;

  for (;;) {
    status = writer->provide(&buffer, 0, &wbytes);
    if (status->error() || wbytes == 0) {
      break;
    }

    status = source->read(buffer, wbytes, &rbytes);
    if (status->error() || rbytes == 0) {
      break;
    }

    *cbytes += writer->extend(rbytes);
  }

  return status;
}

Status copy(Source *source, FlusherWriter *fwriter, size_t *cbytes) noexcept {
  Status status = OK;
  uint8_t *buffer;
  size_t pbytes, rbytes, fbytes;

  *cbytes = 0;

  for (;;) {
    status = fwriter->provide(&buffer, 0, &pbytes);
    if (status->error()) {
      return status;
    }

    if (pbytes == 0) {
      // lazily flush fwriter if no more bytes can be
      // provided
      status = fwriter->flush(&fbytes);
      if (status->error()) {
        return status;
      }

      continue;
    }

    status = source->read(buffer, pbytes, &rbytes);
    if (status->error() || rbytes == 0) {
      return status;
    }

    *cbytes += fwriter->extend(rbytes);
  }
}

Status copy(
    const uint8_t *src,
    size_t len,
    Writer *writer,
    size_t *cbytes) noexcept {
  return writer->write(src, len, cbytes);
}

Status copy(
    const uint8_t *src,
    size_t len,
    FlusherWriter *fwriter,
    size_t *cbytes) noexcept {
  size_t fbytes, wbytes;
  size_t src_index = 0;

  *cbytes = 0;

  for (;;) {
    fbytes = 0;

    auto status = fwriter->write(src + src_index, len - src_index, &wbytes);
    if (status->error()) {
      return status;
    }

    src_index += wbytes;
    *cbytes += wbytes;

    if (src_index < len) {
      // lazily flush the writer in case more space may be
      // needed to be able to write the remaining contents
      // of `src` to `fwriter`
      status = fwriter->flush(&fbytes);
      if (status->error()) {
        return status;
      }
    }

    if (fbytes == 0 && wbytes == 0) {
      return status;
    }
  }
}

Status copy(Reader *reader, uint8_t *dst, size_t len, size_t *cbytes) noexcept {
  return reader->read(dst, len, cbytes);
}

