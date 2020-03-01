// Copyright (c) 2018, tlblanc <tlblanc1490 at gmail dot com>

#ifndef VALUE_FLAG_H_
#define VALUE_FLAG_H_

#include <stddef.h>
#include <string.h>

#include <array>
#include <chrono>
#include <string>

#include "value/parser.hpp"

class Flag final {
 public:
  Flag() {
    memset(m_error, 0, ERROR_LENGTH);
  }

  // delete to prevent a conversion from a static const char
  // value to a std::string
  bool string_var(char *value,
                  char flag,
                  const char *name,
                  const char *desc) = delete;

  bool string_var(char *value,
                  size_t len,
                  char flag,
                  const char *name,
                  const char *desc) noexcept;

  bool string_var(std::string *value,
                  char flag,
                  const char *name,
                  const char *desc) noexcept;

  bool bool_var(bool *value,
                char flag,
                const char *name,
                const char *desc) noexcept;

  bool int32_var(int32_t *value,
                 char flag,
                 const char *name,
                 const char *desc) noexcept;

  bool uint32_var(uint32_t *value,
                  char flag,
                  const char *name,
                  const char *desc) noexcept;

  bool int64_var(int64_t *value,
                 char flag,
                 const char *name,
                 const char *desc) noexcept;

  bool uint64_var(uint64_t *value,
                  char flag,
                  const char *name,
                  const char *desc) noexcept;

  bool duration_var(std::chrono::nanoseconds *value,
                    char flag,
                    const char *name,
                    const char *desc) noexcept;

  /**
   * error returns a string representing the error encountered
   * in case "parse" returns an error status code
   */
  const char *error() const noexcept;

  /**
   * parse should be called on the parser once all the
   * expected variables have been set up. In case of encountering an
   * error during the parser it will return a status code
   * and it will not be able pt proceed
   */
  bool parse(int argc, const char *argv[]) noexcept;

  /**
   * print_help prints to stdout all the command line
   * arguments expected by the application
   */
  void print_help(const char *prog) const noexcept;

 private:
  class Variable {
   public:
    Variable():
        m_parser(nullptr, 0, false, nullptr) {
      m_flag = 0;
      m_type = nullptr;
      m_name = nullptr;
      m_desc = nullptr;
      m_init = false;
    }

    ~Variable() = default;

    // cppcheck-suppress performance
    static void init(Variable *variable,
                     const char *type,
                     Parser parser,
                     char flag,
                     const char *name,
                     const char *desc) {
      variable->m_parser = parser;
      variable->m_type = type;
      variable->m_flag = flag;
      variable->m_name = name;
      variable->m_desc = desc;
      variable->m_init = true;
    }

    inline char flag() const noexcept {
      return m_flag;
    }

    inline const char *type() const noexcept {
      return m_type;
    }

    inline const char *name() const noexcept {
      return m_name;
    }

    inline const char *desc() const noexcept {
      return m_desc;
    }

    inline bool set(const char *s, std::size_t size) noexcept {
      return m_parser.set(s, size);
    }

    inline void *get() const noexcept {
      return m_parser.get();
    }

    inline bool expects_arg() const noexcept {
      return m_parser.expects_arg();
    }

    inline bool is_init() const noexcept {
      return m_init;
    }

   private:
    Parser m_parser;
    char m_flag;
    const char *m_type;
    const char *m_name;
    const char *m_desc;
    bool m_init;
  };

  Flag::Variable *find_flag(const char *arg,
                            size_t len) noexcept;

  bool parse_command(int argc,
                    const char *argv[],
                    int *currindex,
                    bool *done) noexcept;

  static constexpr int ERROR_LENGTH = 128;

  char m_error[ERROR_LENGTH];
  // all ascii codes for numbers [48 - 57],
  // for uppercase letters [65-90], and
  // for lowercase letters [97-122]
  std::array<Variable, 62> m_vars;
};

#endif  // VALUE_FLAG_H_
