// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef VALUE_PARSER_H_
#define VALUE_PARSER_H_

#include <stddef.h>
#include <stdint.h>

#include <chrono>
#include <functional>
#include <string>

using ParseFunc = std::function<bool(const char *s,
                                     size_t len,
                                     size_t maxlen,
                                     void *value)>;

struct Parser final {
 public:
  static const char kString[];
  static const char kBool[];
  static const char kInt32[];
  static const char kInt64[];
  static const char kUInt32[];
  static const char kUInt64[];
  static const char kDuration[];

  static Parser string_parser(char *ptr) noexcept = delete;
  static Parser string_parser(char *ptr, size_t len) noexcept;
  static Parser string_parser(std::string *ptr) noexcept;
  static Parser bool_parser(bool *ptr) noexcept;
  static Parser int32_parser(int32_t *ptr) noexcept;
  static Parser uint32_parser(uint32_t *ptr) noexcept;
  static Parser int64_parser(int64_t *ptr) noexcept;
  static Parser uint64_parser(uint64_t *ptr) noexcept;
  static Parser duration_parser(std::chrono::nanoseconds *ptr) noexcept;

  ~Parser() = default;

  Parser(void *value,
         size_t len,
         bool expects_arg,
         ParseFunc parse_func) noexcept
      : m_ptr(value),
        m_len(len),
        m_expects_arg(expects_arg),
        m_parse_func(parse_func) { }

  static void init(Parser *parser,
                   void *ptr,
                   size_t len,
                   bool expects_arg,
                   ParseFunc parse_func) noexcept {
    parser->m_ptr = ptr;
    parser->m_len = len;
    parser->m_expects_arg = expects_arg;
    parser->m_parse_func = parse_func;
  }

  bool set(const char *s, size_t size) noexcept {
    return m_parse_func(s, size, m_len, m_ptr);
  }

  void *get() const noexcept {
    return m_ptr;
  }

  bool expects_arg() const noexcept {
    return m_expects_arg;
  }

 private:
  void *m_ptr;
  size_t m_len;
  bool m_expects_arg;
  ParseFunc m_parse_func;
};

bool parse_func_string(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept;
bool parse_func_charray(const char *s,
                        size_t len,
                        size_t maxlen,
                        void *value) noexcept;
bool parse_func_bool(const char *s,
                     size_t len,
                     size_t maxlen,
                     void *value) noexcept;
bool parse_func_int32(const char *s,
                      size_t len,
                      size_t maxlen,
                      void *value) noexcept;
bool parse_func_int64(const char *s,
                      size_t len,
                      size_t maxlen,
                      void *value) noexcept;
bool parse_func_uint32(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept;
bool parse_func_uint64(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept;
bool parse_func_duration(const char *s,
                         size_t len,
                         size_t maxlen,
                         void *value) noexcept;

bool parse_string(char *value,
                  const char *s,
                  size_t len) noexcept = delete;
bool parse_string(std::string *value,
                  const char *s,
                  size_t len) noexcept;
bool parse_charray(char *value,
                   size_t maxlen,
                   const char *s,
                   size_t len) noexcept;
bool parse_bool(bool *value,
                const char *s,
                size_t len) noexcept;
bool parse_int32(int32_t *value,
                 const char *s,
                 size_t len) noexcept;
bool parse_int64(int64_t *value,
                 const char *s,
                 size_t len) noexcept;
bool parse_uint32(uint32_t *value,
                  const char *s,
                  size_t len) noexcept;
bool parse_uint64(uint64_t *value,
                  const char *s,
                  size_t len) noexcept;
bool parse_duration(std::chrono::nanoseconds *value,
                    const char *s,
                    size_t len) noexcept;


#endif  // VALUE_PARSER_H_
