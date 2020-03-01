// Copyright (c) 2018, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_DISPOSEFUNC_H_
#define IO_DISPOSEFUNC_H_

#include <functional>

using DisposeFunc = std::function<void(uint8_t*)>;

extern inline void array_delete_dispose_func(uint8_t *ptr) {
  delete [] ptr;
}

extern inline void do_nothing_delete_dispose_func(uint8_t *ptr) {
  (void)(ptr);
}

#endif  // IO_DISPOSEFUNC_H_
