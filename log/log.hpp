// Copyright (c) 2018, eaugeas <eaugeas at gmail dot com>

#ifndef LOG_H_
#define LOG_H_

#include <errno.h>
#include <string.h>
#include <syslog.h>

#define LOGINIT(prog) openlog((prog),               \
    LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,   \
    LOG_USER)

#define LOG_ENABLE_UPTO(level) setlogmask(LOG_UPTO(level))

#define LDEBUG(call, fmt, ...)                                        \
  syslog(LOG_DEBUG,                                                   \
         "[file %s] [line %d] [func %s] [DEBUG] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#ifndef DEBUG
#define LTRACE(call, fmt, ...)
#else
#define LTRACE(call, fmt, ...)                                        \
  syslog(LOG_DEBUG,                                                   \
         "[file %s] [line %d] [func %s] [TRACE] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)
#endif  // DEBUG

#define LINFO(call, fmt, ...)                  \
  syslog(LOG_INFO,                                         \
         "[file %s] [line %d] [func %s] [INFO] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LEMERGENCY(call, fmt, ...)                  \
  syslog(LOG_EMERGENCY,                                         \
         "[file %s] [line %d] [func %s] [EMERGENCY] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LALERT(call, fmt, ...)                  \
  syslog(LOG_ALERT,                                         \
         "[file %s] [line %d] [func %s] [ALERT] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LCRITICAL(call, fmt, ...)                  \
  syslog(LOG_CRITICAL,                                         \
         "[file %s] [line %d] [func %s] [CRITICAL] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LERROR(call, fmt, ...)                  \
  syslog(LOG_ERR,                                           \
         "[file %s] [line %d] [func %s] [ERROR] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LPERR(call, fmt, ...)                  \
  syslog(LOG_PERR,                                         \
         "[file %s] [line %d] [func %s] [PERR] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LWARNING(call, fmt, ...)                  \
  syslog(LOG_WARNING,                                         \
         "[file %s] [line %d] [func %s] [WARNING] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LNOTICE(call, fmt, ...)                  \
  syslog(LOG_NOTICE,                                         \
         "[file %s] [line %d] [func %s] [NOTICE] %s: "                 \
         fmt, __FILE__, __LINE__, __FUNCTION__, call, ##__VA_ARGS__)

#define LOGCLOSE() closelog()

#ifdef DEBUG
#define LOG_ERRNO(operation, msg)                           \
  do {                                                      \
    int value = errno;                                      \
    errno = 0;                                              \
    LDEBUG(operation, "msg: %s, errno: %s, errmsg: %s",     \
           msg, strerror(errno))                            \
  } while(0)

#else
#define LOG_ERRNO(operation, msg)
#endif  // DEBUG

#endif  // LOG_H_
