// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "scanner.hpp"

#include <assert.h>

#include <algorithm>

#include "status.hpp"
#include "value/string_utils.hpp"

bool Scanner::scan_line_func(
    const uint8_t *current,
    const uint8_t *limit,
    bool is_end,
    size_t *offset) {

    while (true) {
        if (current >= limit) {
            // if we have reached the end of the buffer and there are no
            // newlines just return offset 0
            *offset = 0;
            return true;
        }

        const char *skipped = string_skip_line(
            reinterpret_cast<const char*>(current),
            limit - current);
        const uint8_t *stopped = reinterpret_cast<const uint8_t*>(skipped);
        if (stopped == current) {
            // skip all newline characters at the beginning of the
            // buffer
            *offset = 0;
            return true;
        }

        *offset = stopped - current;
        if (is_end || stopped < limit) {
            return true;

        } else {
            return false;
        }
    }
}

size_t Scanner::consume(const size_t len) noexcept {
  size_t cbytes = 0;

  if (m_token_len == len) {
    cbytes = m_reader->consume(len);
    assert(len <= cbytes);
    m_next_peek_should_read = true;
  }

  return cbytes;
}

Status Scanner::peek(const uint8_t **dst,
                     const size_t intent,
                     size_t *pbytes) noexcept {
  size_t ignore_bytes;
  Status status = OK;

  if (m_next_peek_should_read) {
    m_next_peek_should_read = false;
    status = scan();
    if (status->error()) {
      return status;
    }
  }

  // peek will return all the bytes from the reader that are available
  // to be "peeked" for the caller. However, we only want to return the
  // token length.
  status = m_reader->peek(dst, intent, &ignore_bytes);
  if (status->error()) {
    return status;
  }

  assert(ignore_bytes >= m_token_len);
  *pbytes = m_token_len;
  return OK;
}

Status Scanner::read(
    uint8_t *dst,
    const size_t len,
    size_t *rbytes) noexcept {
  *rbytes = 0;

  auto cbytes = consume(m_token_len);
  assert(cbytes == m_token_len);
  m_token_len = 0;

  const uint8_t *token;
  size_t pbytes;
  auto status = peek(&token, 0, &pbytes);
  if (status->error()) {
    return status;
  }

  if (m_token_len > len) {
    return ScanBufferTooSmall;
  }

  memcpy(dst, token, m_token_len);
  *rbytes = m_token_len;
  return OK;
}

Status Scanner::scan() noexcept {
  const uint8_t *buffer;
  size_t pbytes, cbytes, rbytes, tsize = m_longest_token;

  // scan will attempt a peek with an initial value of 1.
  // This is done to avoid forcing the reader reading from
  // a potential source every time a call to peek is made,
  // which would slow down this loop significantly. There
  // could be implementations of a reader that try to return
  // as many bytes at most as the intent, if possible. In this
  // case, the approach would not work well.
  do {
    m_token_len = 0;
    auto status = m_reader->peek(&buffer, tsize, &pbytes);
    if (status->error() || pbytes == 0) {
      return status;
    }

    auto ok = m_scan_func(buffer, buffer + pbytes, m_is_end, &m_token_len);
    if (ok && m_token_len == 0) {
      // increase the intended peek size to the maximum
      tsize = 0;
      m_reader->consume(1);

    } else if (!ok) {
      tsize = 0;

      if (m_is_end && !m_skip_on_failure) {
        return ScanFuncCannotRecover;

      } else if (!m_is_end) {
        if (tsize == 0) {
          // check if the end of the reader has been reached. In this
          // case, recover the buffer and try scan_func again
          // knowing that the reader has no more data to be processed
          cbytes = m_reader->consume(pbytes + 1);
          if (cbytes == pbytes) {
            rbytes = m_reader->recover(pbytes);
            assert(rbytes == pbytes);
            m_is_end = true;
            m_token_len = 0;
            continue;
          }
        } else {
          // attempt to peek as many bytes as possible from the reader
          // and see if a token can be parsed
          tsize = 0;
        }

        continue;

      } else {
        // attempt to recover from the parsing error by skipping
        // all the characters in the buffer
        cbytes = consume(pbytes);
        assert(cbytes == pbytes);
      }
    }
  } while (m_token_len == 0);

  m_longest_token = std::max(
      static_cast<uint64_t>(m_token_len),
      static_cast<uint64_t>(m_longest_token));
  return OK;
}
