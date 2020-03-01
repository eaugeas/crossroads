// Copyright (c) 2018, tlblanc <tlblanc1490 at gmail dot com>

#ifndef IO_MEMIO_H_
#define IO_MEMIO_H_

#include <arpa/inet.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

extern inline const uint8_t *readmem(const uint8_t *mem,
                                     uint8_t *dest,
                                     size_t len) {
  memcpy(dest, mem, len);
  return mem + len;
}

extern inline const uint8_t *readmempad(const uint8_t *mem,
                                        uint8_t *dest,
                                        size_t len) {
  size_t padding;

  memcpy(dest, mem, len);
  padding = (4 - (len % 4)) % 4;
  return mem + len + padding;
}

extern inline const uint8_t* readu32(const uint8_t *mem, uint32_t *value) {
  memcpy(reinterpret_cast<uint8_t*>(value), mem, sizeof(*value));
  *value = ntohl(*value);
  return mem + 4;
}

extern inline const uint8_t* readu64(const uint8_t *mem, uint64_t *value) {
  mem = readu32(mem, reinterpret_cast<uint32_t*>(value) + 1);
  mem = readu32(mem, reinterpret_cast<uint32_t*>(value));
  return mem;
}

extern inline const uint8_t* readu16(const uint8_t *mem, uint16_t *value) {
  memcpy(reinterpret_cast<uint8_t*>(value), mem, sizeof(*value));
  *value = ntohs(*value);
  return mem + 2;
}

extern inline uint8_t* writeu16(uint8_t *mem, uint16_t value) {
  value = htons(value);
  memcpy(mem, reinterpret_cast<uint8_t*>(&value), sizeof(value));
  return mem + 2;
}

extern inline uint8_t* writeu32(uint8_t *mem, uint32_t value) {
  value = htonl(value);
  memcpy(mem, reinterpret_cast<uint8_t*>(&value), sizeof(value));
  return mem + 4;
}

extern inline uint8_t* writeu64(uint8_t *mem, uint64_t value) {
  mem = writeu32(mem, *(reinterpret_cast<uint32_t*>(&value) + 1));
  return writeu32(mem, static_cast<uint32_t>(value));
}

extern inline uint8_t* writeu48(uint8_t *mem, uint64_t value) {
  uint16_t s = htons(value >> 32);
  uint32_t u = htonl(value & 0xffffffff);

  memcpy(mem, reinterpret_cast<uint8_t*> (&s), sizeof(s));
  memcpy(mem + 2, reinterpret_cast<uint8_t*>(&u), sizeof(u));

  return mem + 6;
}

extern inline uint8_t* writemem(uint8_t *mem,
                                const uint8_t *src,
                                size_t len) {
  memcpy(mem, src, len);
  return mem + len;
}

extern inline uint8_t* writemempad(uint8_t *mem,
                                   const uint8_t *src,
                                   size_t len) {
  size_t padding = (4 - (len % 4)) % 4;

  memcpy(mem, src, len);
  if (padding > 0) {
    memset(mem + len, 0, padding);
  }

  return mem + len + padding;
}

#endif  // IO_MEMIO_H_
