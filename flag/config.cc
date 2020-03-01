// Copyright (c) 2018, tlblanc <tlblanc1490 at gmail dot com>

#include "config.hpp"

#include <assert.h>
#include <string.h>

#include "status.hpp"
#include "value/string_utils.hpp"

static inline bool _is_comment(char c) {
  return c == '#';
}

bool Config::string_var(char *ptr,
                          size_t len,
                          const char *name,
                          const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::string_parser(ptr, len),
                                       Parser::kString,
                                       name,  desc));
  return true;
}


bool Config::string_var(std::string *ptr,
                          const char *name,
                          const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::string_parser(ptr),
                                       Parser::kString,
                                       name,  desc));
  return true;
}

bool Config::bool_var(bool *ptr,
                        const char *name,
                        const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::bool_parser(ptr),
                                       Parser::kBool,
                                       name,  desc));
  return true;
}

bool Config::int32_var(int32_t *ptr,
                         const char *name,
                         const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::int32_parser(ptr),
                                       Parser::kInt32,
                                       name,  desc));
  return true;
}


bool Config::uint32_var(uint32_t *ptr,
                          const char *name,
                          const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::uint32_parser(ptr),
                                       Parser::kUInt32,
                                       name,  desc));
  return true;
}

bool Config::int64_var(int64_t *ptr,
                         const char *name,
                         const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::int64_parser(ptr),
                                       Parser::kInt64,
                                       name,  desc));
  return true;
}


bool Config::uint64_var(uint64_t *ptr,
                          const char *name,
                          const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::uint64_parser(ptr),
                                       Parser::kUInt64,
                                       name,  desc));
  return true;
}


bool Config::duration_var(std::chrono::nanoseconds *ptr,
                            const char *name,
                            const char *desc) noexcept {
  std::string s(name);
  if (m_vars.count(s) > 0) {
    return false;
  }

  m_vars.emplace(std::piecewise_construct,
                 std::forward_as_tuple(std::move(s)),
                 std::forward_as_tuple(Parser::duration_parser(ptr),
                                       Parser::kDuration,
                                       name,  desc));
  return true;
}

void Config::print_help(const char *prog) const noexcept {
  printf("Configuration; %s", prog);

  for (const auto& pair : m_vars) {
    printf("  %s [%s]\t%s\n", pair.second.name(),
           pair.second.type(), pair.second.desc());
  }
}

Status Config::parse(const char **error) noexcept {
  Status status = OK;
  size_t len;

  for (int i = 0; status->ok() && len > 0; i++) {
    const char *line;
    status = m_scanner->peek(
        reinterpret_cast<const uint8_t**>(&line), 0, &len);
    status = process_line(i, line, len);
    assert(len == m_scanner->consume(len));
    *error = m_error;
  }

  return status;
}

Status Config::process_line(int count,
                            const char *line,
                            size_t len) noexcept {
  size_t remaining = len;
  const char *begin_key = string_skip_blank(line, len);
  size_t parsed = static_cast<size_t>(begin_key - line);

  if (parsed >= remaining) {
    return OK;
  }

  remaining -= parsed;
  if (_is_comment(*begin_key)) {
    return OK;
  }

  const char *end_key = string_skip_non_blank(begin_key, remaining);
  parsed = static_cast<size_t>(end_key - begin_key);
  if (parsed >= remaining) {
    return OK;
  }

  remaining -= parsed;
  const std::string key(begin_key, parsed);
  const char *begin_value = string_skip_blank(end_key, remaining);
  parsed = begin_value - end_key;
  if (parsed >= remaining) {
    snprintf(m_error, ERROR_LENGTH, "%s - %d", "line has no value", count);
    return ConfigLineNoValue;
  }
  remaining -= parsed;
  const char *end_value = string_skip_non_blank(begin_value, remaining);
  parsed = static_cast<size_t>(end_value - begin_value);
  if (parsed == 0) {
    snprintf(m_error, ERROR_LENGTH, "[%d] error: %s",
             count, "line has no value");
    return ConfigNotFoundKey;
  }

  if (m_vars.count(key) == 0) {
    snprintf(m_error, ERROR_LENGTH, "[%d] error: %s - key: %s",
             count, "key was not defined", key.c_str());
    return ConfigNotFoundKey;
  }

  variable_t& variable = m_vars.find(key)->second;
  if (!variable.set(begin_value, parsed)) {
    snprintf(m_error, ERROR_LENGTH, "[%d] error: %s - key: %s",
             count, "failed to parse value for key", key.c_str());
    return ConfigParseValue;
  }

  return OK;
}
