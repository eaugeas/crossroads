// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef VALUE_STRINGUTILS_H_
#define VALUE_STRINGUTILS_H_

#include <stddef.h>
#include <string.h>

#include <string>

typedef bool(char_skipper_t)(char c);

bool string_case_equals(const char *a, size_t alen,
                        const char *b, size_t blen);
bool string_case_equals(const char *a, const char *b);

bool string_equals(const char *a, size_t alen,
                   const char *b, size_t blen);
bool string_equals(const char *a, const char *b);

bool string_is_newline(char c);
bool string_is_not_newline(char c);
bool string_is_blank(char c);
bool string_is_not_blank(char c);

bool string_is_empty(const char *c);
bool string_is_blank(const char *c);
bool string_is_empty(const char *c, size_t len);
bool string_is_blank(const char *c, size_t len);
bool string_is_not_empty(const char *c, size_t len);
bool string_is_not_blank(const char *c, size_t len);

char *string_dup(const char *c);
char *string_dup(const char *c, size_t len);
const char *string_skip_char(const char *c,
                             size_t len,
                             char_skipper_t skipper);
const char *string_skip_line(const char *c,
                             size_t len);
const char *string_skip_blank(const char *c,
                              size_t len);
const char *string_skip_non_blank(const char *c,
                                  size_t len);
int64_t strntol(const char *nptr,
                size_t len,
                const char **endptr,
                int base);
uint64_t strntoul(const char *nptr,
                  size_t len,
                  const char **endptr,
                  int base);

#endif  // VALUE_STRINGUTILS_H_
