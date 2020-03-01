// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include <string.h>

#include "flag.hpp"

#include "value/string_utils.hpp"

static uint8_t _ascii_to_flag(uint8_t ascii) {
  if (ascii >= 48 && ascii <= 57) {
    return ascii - 48;

  } else if (ascii >= 65 && ascii <= 90) {
    return ascii - 65 + 10;

  } else if (ascii >= 97 && ascii <= 122) {
    return ascii - 97 + 26 + 10;

  } else {
    return 0xff;
  }
}

static inline bool _is_short_option(const char *arg,
                                    size_t len) {
  return len == 2 && arg[0] == '-' && arg[1] != '-';
}

static inline bool _is_long_option(const char *arg,
                                   size_t len) {
  return len > 2 && arg[0] == '-' && arg[1] == '-' && arg[2] != '-';
}

static inline bool _is_option(const char *arg,
                              size_t len) {
  return _is_short_option(arg, len) || _is_long_option(arg, len);
}

Flag::Variable *Flag::find_flag(const char *arg,
                                                    size_t len) noexcept {
  Flag::Variable *flag = nullptr;

  if (_is_short_option(arg, len)) {
    uint8_t flagcode = _ascii_to_flag(arg[1]);
    if (flagcode == 0xff) {
      return nullptr;
    }

    flag = &m_vars[flagcode];

    if (!flag->is_init()) {
      return nullptr;
    }

  } else if (_is_long_option(arg, len)) {
    for (auto& variable : m_vars) {
      if (variable.is_init() &&
          string_equals(variable.name(),
                        strlen(variable.name()),
                        arg + 2, len - 2)) {
        flag = &variable;
        return flag;
      }
    }
  }

  return flag;
}

bool Flag::parse_command(int argc,
                                  const char *argv[],
                                  int *currindex,
                                  bool *done) noexcept {
  bool ok;
  const char *arg, *optarg;
  size_t arglen;
  Variable *flag = nullptr;

  // first argument in argv is the process name.
  // we can skip it
  *currindex = *currindex == 0 ? 1 : *currindex;
  *done = false;

  if (argc <= *currindex) {
    *done = true;
    return true;
  }

  arg = argv[*currindex];

  arglen = strlen(arg);
  if (arglen <= 1) {
    snprintf(m_error, ERROR_LENGTH, "%s - error -- arg too short [%s]\n",
             argv[0], arg);
    return false;
  }

  (*currindex)++;
  flag = find_flag(arg, arglen);
  if (!flag) {
    snprintf(m_error, ERROR_LENGTH, "%s - error -- arg not found [%s]\n",
             argv[0], arg);
    return false;
  }

  if (!flag->expects_arg()) {
    flag->set("true", 4);
    return true;

  } else if (*currindex < argc &&
             !_is_option(argv[*currindex], strlen(argv[*currindex]))) {
    optarg = argv[*currindex];
    ok = flag->set(optarg, strlen(optarg));
    if (!ok) {
      snprintf(m_error, ERROR_LENGTH, "%s - error -- failed to parse [%s]\n",
               argv[0], argv[*currindex]);
      return ok;
    }

    (*currindex)++;
    return ok;

  } else {
    snprintf(m_error, ERROR_LENGTH, "%s - error -- no value found [%s]\n",
             argv[0], argv[*currindex]);
    return false;
  }
}

bool Flag::string_var(char *ptr,
                               size_t len,
                               char ascii,
                               const char *name,
                               const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kString,
                        Parser::string_parser(ptr, len),
                        ascii, name, desc);
  return true;
}

bool Flag::string_var(std::string *ptr,
                               char ascii,
                               const char *name,
                               const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kString,
                        Parser::string_parser(ptr),
                        ascii, name, desc);
  return true;
}

bool Flag::bool_var(bool *ptr,
                             char ascii,
                             const char *name,
                             const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kBool,
                        Parser::bool_parser(ptr),
                        ascii, name, desc);
  return true;
}

bool Flag::int32_var(int32_t *ptr,
                              char ascii,
                              const char *name,
                              const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);
  Parser parser(ptr, 0, true, parse_func_int32);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kInt32,
                        Parser::int32_parser(ptr),
                        ascii, name, desc);

  return true;
}

bool Flag::uint32_var(uint32_t *ptr,
                               char ascii,
                               const char *name,
                               const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kUInt32,
                        Parser::uint32_parser(ptr),
                        ascii, name, desc);
  return true;
}

bool Flag::int64_var(int64_t *ptr,
                              char ascii,
                              const char *name,
                              const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kInt64,
                        Parser::int64_parser(ptr),
                        ascii, name, desc);

  return true;
}

bool Flag::duration_var(std::chrono::nanoseconds *ptr,
                                 char ascii,
                                 const char *name,
                                 const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);
  Parser parser(ptr, 0, true, parse_func_duration);

  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kDuration,
                        Parser::duration_parser(ptr),
                        ascii, name, desc);

  return true;
}

bool Flag::uint64_var(uint64_t *ptr,
                               char ascii,
                               const char *name,
                               const char *desc) noexcept {
  Variable *variable = nullptr;
  uint8_t flagcode = _ascii_to_flag(ascii);
  Parser parser(ptr, 0, true, parse_func_uint64);
  if (flagcode == 0xff) {
    return false;
  }

  variable = &m_vars[flagcode];
  m_vars[flagcode].init(variable, Parser::kUInt64,
                        Parser::uint64_parser(ptr),
                        ascii, name, desc);

  return true;
}

const char *Flag::error() const noexcept {
  return m_error;
}

bool Flag::parse(int argc, const char *argv[]) noexcept {
  int offset = 0;
  bool ok = true;
  bool done = false;

  while (!done) {
    ok = parse_command(argc, argv, &offset, &done);
    if (!ok) {
      return ok;
    }
  }

  return ok;
}

void Flag::print_help(const char *prog) const noexcept {
  if (m_error[0] != 0) {
    printf("%s\n", m_error);
  }

  printf("Usage; %s [OPTIONS]\n\n", prog);

  for (const auto& variable : m_vars) {
    if (variable.is_init()) {
      printf("  -%c, --%s [%s]\t%s\n", variable.flag(),
             variable.name(), variable.type(), variable.desc());
    }
  }
}
