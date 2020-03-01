// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef STATUS_STATUS_H_
#define STATUS_STATUS_H_

#include <stddef.h>

/// return error codes. msg is assumed to always
/// be a static string that will never be deleted
class StatusClass final {
 public:
  StatusClass(int status, const char *msg):
      m_status(status),
      m_msg(msg) { }

  ~StatusClass() = default;

  StatusClass(const StatusClass &status):
      m_status(status.m_status),
      m_msg(status.m_msg) { }

  StatusClass(StatusClass &&status):
      m_status(status.m_status),
      m_msg(status.m_msg) { }

  StatusClass operator=(const StatusClass &status) {
    if (this == &status) {
      return *this;
    }

    m_status = status.m_status;
    m_msg = status.m_msg;
    return *this;
  }

  StatusClass operator=(StatusClass &&status) {
    if (this == &status) {
      return *this;
    }
    m_status = status.m_status;
    m_msg = status.m_msg;
    return *this;
  }

  bool operator==(const StatusClass &status) {
    return this == &status;
  }

  inline int status() const noexcept {
    return m_status;
  }

  inline bool ok() const noexcept {
    return m_status == OK;
  }

  inline bool error() const noexcept {
    return m_status != OK;
  }

  inline const char *msg() const noexcept {
    return m_msg;
  }

 private:
  static constexpr int OK = 0;

  int m_status;
  const char *m_msg;
};

using Status = const StatusClass*;

const extern StatusClass *OK;

#endif  // STATUS_STATUS_H_
