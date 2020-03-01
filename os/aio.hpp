// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef AIO_H_
#define AIO_H_

#ifdef __APPLE__
#include <sys/event.h>
typedef struct kevent aio_event_t;

#elif __linux__
#include <sys/epoll.h>
typedef struct epoll_event aio_event_t;
#else
#error "platform not implemented"
#endif

#include <arpa/inet.h>

int aio_create();
int aio_close(int sockfd);
int aio_shutdownrd(int sockfd);
int aio_shutdownwd(int sockfd);
int aio_wait(int queue,
             aio_event_t *events,
             size_t len,
             int64_t timeout_ms);
int aio_monit(int queue, int fd, int id, bool edge);
int aio_rmonit(int queue, int fd, int id, bool edge);
int aio_wmonit(int queue, int fd, int id, bool edge);
int aio_unmonit(int queue, int fd);
int aio_runmonit(int queue, int fd);
int aio_wunmonit(int queue, int fd);
int aio_getid(const aio_event_t *event);

int aio_isclosed(const aio_event_t *event);
int aio_ispeer_closed(const aio_event_t *event);
int aio_iserror(const aio_event_t *event);
int aio_isread(const aio_event_t *event);
int aio_iswrite(const aio_event_t *event);

int aio_pipe(int fd[2]);

int aio_socket(int domain,
               int type,
               int protocol);

int aio_accept(int serverfd,
               struct sockaddr *addr,
               socklen_t *addrlen);
int aio_bind(int sockfd,
             const struct sockaddr* addr,
             socklen_t addrlen);

int aio_listen(int sockfd,
               int backlog);

int aio_connect(int sockfd,
                const struct sockaddr* addr,
                socklen_t addrlen);
int aio_handle_signals(void);

#endif  // AIO_H_
