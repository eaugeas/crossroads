// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef FLAG_CONFIG_H_
#define FLAG_CONFIG_H_

#include <stddef.h>
#include <string.h>

#include <chrono>
#include <map>
#include <memory>
#include <string>

#include "buffer/buffer.hpp"
#include "buffer/scanner.hpp"
#include "status/status.hpp"
#include "value/parser.hpp"

class Config final {
 public:
  // delete to prevent a conversion from a static const char
  // value to a std::string
  bool string_var(char *value,
                  const char *name,
                  const char *desc) = delete;

  bool string_var(char *value,
                  size_t len,
                  const char *name,
                  const char *desc) noexcept;

  bool string_var(std::string *value,
                  const char *name,
                  const char *desc) noexcept;

  bool bool_var(bool *value,
                const char *name,
                const char *desc) noexcept;

  bool int32_var(int32_t *value,
                 const char *name,
                 const char *desc) noexcept;

  bool uint32_var(uint32_t *value,
                  const char *name,
                  const char *desc) noexcept;

  bool int64_var(int64_t *value,
                 const char *name,
                 const char *desc) noexcept;

  bool uint64_var(uint64_t *value,
                  const char *name,
                  const char *desc) noexcept;

  bool duration_var(std::chrono::nanoseconds *value,
                    const char *name,
                    const char *desc) noexcept;

  /**
   * print_help prints to stdout all the configuration parameters
   * that are expected by the configuration parser
   */
  void print_help(const char *prog) const noexcept;

  explicit Config(std::unique_ptr<Scanner> &&scanner):
      m_scanner(std::move(scanner)) {
    memset(m_error, 0, ERROR_LENGTH);
  }

  /**
   * parse should be called on the configuration parser once all the
   * expected variables have been set up. In case of encountering an
   * error during the parser it will return a status code
   * and it will not be able pt proceed
   */
  Status parse(const char **error) noexcept;

 private:
  class variable_t {
   public:
    variable_t(Parser parser,
               const char *type,
               const char *name,
               const char *desc):
        m_parser(parser),
        m_type(type),
        m_name(name),
        m_desc(desc) { }

    ~variable_t() = default;

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

   private:
    Parser m_parser;
    const char *m_type;
    const char *m_name;
    const char *m_desc;
  };

  Status process_line(int count,
                      const char *line,
                      size_t len) noexcept;

  static constexpr int ERROR_LENGTH = 128;

  std::unique_ptr<Scanner> m_scanner;
  char m_error[ERROR_LENGTH];
  std::map<std::string, variable_t> m_vars;
};

#endif  // FLAG_CONFIG_H_
