// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "parser.hpp"

#include <errno.h>
#include <string.h>

#include <algorithm>

#include "string_utils.hpp"

static inline size_t _min_pos_len(size_t len1, size_t len2) {
  return len1 == 0 ? len2 : len2 == 0 ? len1 : std::min(len1, len2);
}

const char Parser::kString[] = "string";
const char Parser::kBool[] = "bool";
const char Parser::kInt32[] = "int32";
const char Parser::kInt64[] = "int64";
const char Parser::kUInt32[] = "uint32";
const char Parser::kUInt64[] = "uint64";
const char Parser::kDuration[] = "duration";


Parser Parser::string_parser(char *ptr, size_t len) noexcept {
  return Parser(ptr, len, true, parse_func_charray);
}

Parser Parser::string_parser(std::string *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_string);
}

Parser Parser::bool_parser(bool *ptr) noexcept {
  return Parser(ptr, 0, false, parse_func_bool);
}

Parser Parser::int32_parser(int32_t *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_int32);
}

Parser Parser::uint32_parser(uint32_t *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_uint32);
}

Parser Parser::int64_parser(int64_t *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_int64);
}

Parser Parser::uint64_parser(uint64_t *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_uint64);
}

Parser Parser::duration_parser(
    std::chrono::nanoseconds *ptr) noexcept {
  return Parser(ptr, 0, true, parse_func_duration);
}

bool parse_func_string(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept {
  return parse_string(static_cast<std::string*>(value),
                      s, _min_pos_len(len, maxlen));
}

bool parse_func_charray(const char *s,
                        size_t len,
                        size_t maxlen,
                        void *value) noexcept {
  return parse_charray(static_cast<char*>(value), maxlen, s, len);
}

bool parse_func_bool(const char *s,
                     size_t len,
                     size_t maxlen,
                     void *value) noexcept {
  return parse_bool(static_cast<bool*>(value),
                    s, _min_pos_len(len, maxlen));
}

bool parse_func_int32(const char *s,
                      size_t len,
                      size_t maxlen,
                      void *value) noexcept {
  return parse_int32(static_cast<int32_t*>(value),
                     s, _min_pos_len(len, maxlen));
}

bool parse_func_int64(const char *s,
                      size_t len,
                      size_t maxlen,
                      void *value) noexcept {
  return parse_int64(static_cast<int64_t*>(value),
                     s, _min_pos_len(len, maxlen));
}

bool parse_func_uint32(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept {
  return parse_uint32(static_cast<uint32_t*>(value),
                      s, _min_pos_len(len, maxlen));
}

bool parse_func_uint64(const char *s,
                       size_t len,
                       size_t maxlen,
                       void *value) noexcept {
  return parse_uint64(static_cast<uint64_t*>(value),
                      s, _min_pos_len(len, maxlen));
}

bool parse_func_duration(const char *s,
                         size_t len,
                         size_t maxlen,
                         void *value) noexcept {
  return parse_duration(static_cast<std::chrono::nanoseconds*>(value),
                        s, _min_pos_len(len, maxlen));
}

bool parse_bool(bool *value, const char *s, size_t len) noexcept {
  int64_t parsed;

  if (string_case_equals(s, len, "true", 4)) {
    *value = true;
    return true;

  } else if (string_case_equals(s, len, "false", 5)) {
    *value = false;
    return true;

  } else if (parse_int64(&parsed, s, len)) {
    *value = parsed != 0;
    return true;

  } else {
    return false;
  }
}

bool parse_int32(int32_t *value, const char *s, size_t len) noexcept {
  const char *endptr = nullptr;
  int64_t parsed = strntol(s, len, &endptr, 10);

  if (errno ||
      (static_cast<size_t>(endptr - s + 1) < len
       && !isspace(*endptr))) {
    return false;

  } else if (parsed > INT32_MAX) {
    return false;

  } else if (parsed < INT32_MIN) {
    return false;

  } else {
    *value = static_cast<int32_t>(parsed);
    return true;
  }
}

bool parse_int64(int64_t *value, const char *s, size_t len) noexcept {
  const char *endptr = nullptr;
  int64_t parsed = strntol(s, len, &endptr, 10);

  if (errno ||
      (static_cast<size_t>(endptr - s + 1) < len
       && !isspace(*endptr))) {
    return false;

  } else if (parsed > INT64_MAX) {
    return false;

  } else if (parsed < INT64_MIN) {
    return false;

  } else {
    *value = static_cast<int64_t>(parsed);
    return true;
  }
}

bool parse_uint32(uint32_t *value, const char *s, size_t len) noexcept {
  const char *endptr = nullptr;
  uint64_t parsed = strntoul(s, len, &endptr, 10);

  if (errno ||
      (static_cast<size_t>(endptr - s + 1) < len
       && !isspace(*endptr))) {
    return false;

  } else if (parsed > UINT32_MAX) {
    return false;

  } else {
    *value = static_cast<int64_t>(parsed);
    return true;
  }
}

bool parse_uint64(uint64_t *value, const char *s, size_t len) noexcept {
  const char *endptr = nullptr;
  uint64_t parsed = strntoul(s, len, &endptr, 10);

  if (errno ||
      (static_cast<size_t>(endptr - s + 1) < len
       && !isspace(*endptr))) {
    return false;

  } else if (parsed > UINT64_MAX) {
    return false;

  } else {
    *value = static_cast<int64_t>(parsed);
    return true;
  }
}

bool parse_duration(std::chrono::nanoseconds *value,
                    const char *s,
                    size_t len) noexcept {
  const char *endptr = nullptr;
  uint64_t parsed = strntoul(s, len, &endptr, 10);
  size_t pending = len - (endptr - s + 1);
  uint64_t factor = 0;

  if (errno) {
    return false;

  } else if (parsed > UINT64_MAX) {
    return false;

  } else {
    switch (*endptr) {
      case 'h':
        if (pending == 0) {
          factor = 3600 * (1e9);
        } else {
          return false;
        }
        break;
      case 'm':
        if (pending == 0) {
          factor = 60 * (1e9);
        } else if (pending == 1 && *(endptr + 1) == 's') {
          factor = 1e6;
        } else {
          return false;
        }
        break;
      case 's':
        if (pending == 0) {
          factor = 1e9;
        } else {
          return false;
        }
        break;
      case 'u':
        if (pending == 1 && *(endptr + 1) == 's') {
          factor = 1e3;
        } else {
          return false;
        }
        break;
      case 'n':
        if (pending == 1 && *(endptr + 1) == 's') {
          factor = 1;
        } else {
          return false;
        }
        break;
      default:
        return false;
    }

    *value = std::chrono::nanoseconds(parsed * factor);
    return true;
  }
}

bool parse_string(std::string *value,
                  const char *s,
                  size_t len) noexcept {
  value->assign(s, len);
  return true;
}

bool parse_charray(char *value,
                   size_t maxlen,
                   const char *s,
                   size_t len) noexcept {
  if (len < maxlen) {
    memset(value, 0, maxlen);
    strncpy(value, s, len);
    return true;

  } else {
    return false;
  }
}
