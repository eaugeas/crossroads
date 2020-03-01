// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "aio.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#ifdef __APPLE__
#include <sys/event.h>

int aio_create() {
  return kqueue();
}

int aio_close(int sockfd) {
  return close(sockfd);
}

int aio_wait(int queue,
             struct kevent *events,
             size_t len,
             int64_t timeout_ms) {
  if (timeout_ms > -1) {
    struct timespec t_spec = {
      .tv_sec = timeout_ms / 1000,
      .tv_nsec = (timeout_ms % 1000) * 1000000
    };
    return kevent(queue, NULL, 0, events, len, &t_spec);

  } else {
    return kevent(queue, NULL, 0, events, len, NULL);
  }
}

int aio_rmonit(int queue, int fd, int id, bool edge) {
  int opts = edge ? EV_CLEAR : 0;
  int flags = EV_ADD | EV_ENABLE | opts;
  struct kevent event[1];
  memset(event, 0, sizeof(struct kevent));

  void* data = reinterpret_cast<void*>(static_cast<intptr_t>(id));
  EV_SET(&event[0], fd, EVFILT_READ, flags, 0, 0, data);

  return kevent(queue, event, 1, NULL, 0, 0);
}

int aio_wmonit(int queue, int fd, int id, bool edge) {
  int opts = edge ? EV_CLEAR : 0;
  int flags = EV_ADD | EV_ENABLE | opts;
  struct kevent event[1];
  memset(event, 0, sizeof(struct kevent));

  void* data = reinterpret_cast<void*>(static_cast<intptr_t>(id));
  EV_SET(&event[0], fd, EVFILT_WRITE, flags, 0, 0, data);

  return kevent(queue, event, 1, NULL, 0, 0);
}

int aio_monit(int queue, int fd, int id, bool edge) {
  int opts = edge ? EV_CLEAR : 0;
  int flags = EV_ADD | EV_ENABLE | opts;
  struct kevent event[2];
  memset(event, 0, 2*sizeof(struct kevent));

  void* data = reinterpret_cast<void*>(static_cast<intptr_t>(id));
  EV_SET(&event[0], fd, EVFILT_READ, flags, 0, 0, data);
  EV_SET(&event[1], fd, EVFILT_WRITE, flags, 0, 0, data);

  return kevent(queue, event, 2, NULL, 0, 0);
}

int aio_unmonit(int queue,
                int fd) {
  struct kevent event[2];
  memset(event, 0, 2*sizeof(struct kevent));

  EV_SET(&event[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
  EV_SET(&event[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

  return kevent(queue, event, 2, nullptr, 0, 0);
}

int aio_runmonit(int queue,
                 int fd) {
  struct kevent event[1];
  memset(event, 0, 1*sizeof(struct kevent));

  EV_SET(&event[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);

  return kevent(queue, event, 1, nullptr, 0, 0);
}

int aio_wunmonit(int queue,
                 int fd) {
  struct kevent event[1];
  memset(event, 0, sizeof(struct kevent));

  EV_SET(&event[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

  return kevent(queue, event, 1, nullptr, 0, 0);
}

int aio_getid(const struct kevent *event) {
  return static_cast<int>(reinterpret_cast<intptr_t>(event->udata));
}

int aio_isclosed(const struct kevent *event) {
  return (event->flags & EV_EOF);
}

int aio_ispeer_closed(const struct kevent *event) {
  return (event->flags & EV_EOF);
}

int aio_iserror(const struct kevent *event) {
  return (event->flags & EV_ERROR);
}

int aio_isread(const struct kevent *event) {
  return event->filter == EVFILT_READ;
}

int aio_iswrite(const struct kevent *event) {
  return event->filter == EVFILT_WRITE;
}

#elif __linux__
#include <sys/epoll.h>
int aio_create() {
  return epoll_create(1);
}

int aio_close(int queue) {
  return close(queue);
}

int aio_wait(int queue,
             struct epoll_event *events,
             size_t len,
             int64_t timeout_ms) {
  return epoll_wait(queue, events, len, timeout_ms);
}

int aio_rmonit(int queue, int fd, int id, bool edge) {
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));

  int flags = edge ? EPOLLET : 0;
  event.events = EPOLLIN | EPOLLRDHUP | flags;
  event.data.u32 = static_cast<uint32_t>(id);

  return epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event);
}

int aio_wmonit(int queue, int fd, int id, bool edge) {
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));

  int flags = edge ? EPOLLET : 0;
  event.events = EPOLLOUT | EPOLLRDHUP | flags;
  event.data.u32 = static_cast<uint32_t>(id);

  return epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event);
}

int aio_monit(int queue, int fd, int id, bool edge) {
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));

  int flags = edge ? EPOLLET : 0;
  event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | flags;
  event.data.u32 = static_cast<uint32_t>(id);

  return epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event);
}

int aio_unmonit(int queue,
                int fd) {
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));

  return epoll_ctl(queue, EPOLL_CTL_DEL, fd, &event);
}

int aio_runmonit(int queue,
                  int fd) {
  return aio_unmonit(queue, fd);
}

int aio_wunmonit(int queue,
                 int fd) {
  return aio_unmonit(queue, fd);
}

int aio_getid(const struct epoll_event *event) {
  return event->data.u32;
}

int aio_isclosed(const struct epoll_event *event) {
  return (event->events & EPOLLHUP);
}

int aio_ispeer_closed(const struct epoll_event *event) {
  return (event->events & EPOLLRDHUP);
}

int aio_iserror(const struct epoll_event *event) {
  return (event->events & EPOLLERR);
}

int aio_isread(const struct epoll_event *event) {
  return (event->events & EPOLLIN);
}

int aio_iswrite(const struct epoll_event *event) {
  return (event->events & EPOLLOUT);
}
#else
#error "platform not implemented"
#endif

#define RECOVER_ON_ERROR(action, recover)       \
  if ((action) == -1) {                         \
    int tmp_errno = errno;                      \
    do {                                        \
      recover                                   \
        } while (0);                            \
    errno = tmp_errno;                          \
    return -1;                                  \
  }

static int ___ignore_sigpipe(void) {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    act.sa_flags = SA_RESTART;
    return sigaction(SIGPIPE, &act, NULL);
}

static inline int __make_fd_async(int fd) {
  int current_flags = fcntl(fd, F_GETFL, 0);
  if (current_flags == -1) {
    return -1;
  }

  int new_flags = current_flags | O_NONBLOCK;
  if (fcntl(fd, F_SETFL, new_flags) == -1) {
    return -1;
  }

  return 0;
}

int aio_shutdownrd(int sockfd) {
  return shutdown(sockfd, SHUT_RD);
}

int aio_shutdownwd(int sockfd) {
  return shutdown(sockfd, SHUT_WR);
}

int aio_pipe(int fd[2]) {
  if (pipe(fd) == -1) {
    return -1;
  }

  RECOVER_ON_ERROR(__make_fd_async(fd[0]), {
      close(fd[0]);
      close(fd[1]);
    });

  RECOVER_ON_ERROR(__make_fd_async(fd[1]), {
      close(fd[0]);
      close(fd[1]);
    });

  return 0;
}

int aio_socket(int domain,
               int type,
               int protocol) {
  int sockfd = socket(domain, type, protocol);
  if (sockfd == -1) {
    return -1;
  }

  RECOVER_ON_ERROR(__make_fd_async(sockfd), {
      close(sockfd);
    });

  return sockfd;
}

int aio_accept(int serverfd,
               struct sockaddr *addr,
               socklen_t *addrlen) {
  int sockfd = accept(serverfd, (struct sockaddr*)addr, addrlen);
  if (sockfd == -1) {
    return -1;
  }

  RECOVER_ON_ERROR(__make_fd_async(sockfd), {
      close(sockfd);
    });

  return sockfd;
}

int aio_bind(int sockfd,
             const struct sockaddr* addr,
             socklen_t addrlen) {
  int arr[] = {1};
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, arr, sizeof(int)) == -1
      || bind(sockfd, addr, addrlen) == -1) {
    return -1;
  }

  return 0;
}

int aio_listen(int sockfd,
               int backlog) {
  if (listen(sockfd, backlog) == -1) {
    return -1;
  } else {
    return 0;
  }
}

int aio_connect(int sockfd,
                const struct sockaddr* addr,
                socklen_t addrlen) {
  return connect(sockfd, addr, addrlen) == -1 &&
    errno != EINPROGRESS ? -1 : 0;
}

int aio_handle_signals(void) {
  return ___ignore_sigpipe();
}
