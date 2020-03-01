// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef BUFFER_SCANNER_H_
#define BUFFER_SCANNER_H_

#include <functional>
#include <memory>

#include "io/recoverer.hpp"

/// Scanner provides a simple interface to break down
/// an input into tokens and read them as a Reader.
/// Scanner is not a multi-thread safe class. The Scanner
/// does not allocate any extra memory, it uses the underlying
/// memory in the Reader to provide the tokens. This means
/// that it is important that the underlying Reader has enough
/// memory to be able to keep any token that may be found.
class Scanner final : public Reader {
 public:
  using ScanFunc = std::function<bool(const uint8_t *current,
                                      const uint8_t *limit,
                                      bool is_end,
                                      size_t *offset)>;

  static bool scan_line_func(const uint8_t *current,
                             const uint8_t *limit,
                             bool is_end,
                             size_t *offset);

  Scanner(std::unique_ptr<RecovererReader> &&reader,
          bool skip_on_failure = false,
          ScanFunc scan_func = scan_line_func):
      m_is_end(false),
      m_skip_on_failure(skip_on_failure),
      m_next_peek_should_read(true),
      m_longest_token(1),
      m_token_len(0),
      m_reader(std::move(reader)),
      m_scan_func(scan_func) { }

  Scanner(const Scanner &scanner) = delete;
  Scanner(Scanner &&scanner) {
    this->m_is_end = scanner.m_is_end;
    this->m_skip_on_failure = scanner.m_skip_on_failure;
    this->m_next_peek_should_read = scanner.m_next_peek_should_read;
    this->m_longest_token = scanner.m_longest_token;
    this->m_token_len = scanner.m_token_len;
    swap(this->m_reader, scanner.m_reader);
    this->m_scan_func = scanner.m_scan_func;
  }

  Scanner& operator=(const Scanner &scanner) = delete;
  Scanner& operator=(Scanner &&scanner) = delete;

  ~Scanner() = default;

  size_t consume(const size_t len) noexcept override;
  Status peek(const uint8_t **dst,
              const size_t intent,
              size_t *pbytes) noexcept override;
  Status read(uint8_t *dst,
              const size_t len,
              size_t *rbytes) noexcept override;

 private:
  Status scan() noexcept;

  bool m_is_end;
  bool m_skip_on_failure;
  bool m_next_peek_should_read;
  uint32_t m_longest_token;
  size_t m_token_len;
  std::unique_ptr<RecovererReader> m_reader;
  ScanFunc m_scan_func;
};

#endif  // BUFFER_SCANNER_H_
