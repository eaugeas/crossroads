// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "string_utils.hpp"

#define null_check(a, b) do {                     \
 if (a == nullptr && b == nullptr) {              \
    return true;                                  \
 } else if (a == nullptr || b == nullptr) {       \
    return false;                                 \
 }                                                \
 } while(0)                                       \

bool string_case_equals(const char *a, size_t alen,
                               const char *b, size_t blen) {
  null_check(a, b);
  return alen == blen && strncasecmp(a, b, alen) == 0;
}

bool string_case_equals(const char *a, const char *b) {
  null_check(a, b);
  while (tolower(*a) == tolower(*b)
         && *a != '\0'
         && *b != '\0') { a++; b++; }

  return *a == '\0' && *b == '\0';
}

bool string_equals(const char *a, size_t alen,
                          const char *b, size_t blen) {
  null_check(a, b);
  return alen == blen && strncmp(a, b, alen) == 0;
}

bool string_equals(const char *a, const char *b) {
  null_check(a, b);

  while (*a == *b
         && *a != '\0'
         && *b != '\0') { a++; b++; }
  return *a == '\0' && *b == '\0';
}

bool string_is_empty(const char *c,
                     size_t len) {
  return len == 0 || c == nullptr || *c == '\0';
}

bool string_is_empty(const char *c) {
  size_t len = c == nullptr ? 0 : strlen(c);
  return len == 0 || c == nullptr || *c == '\0';
}

bool string_is_blank(const char *c,
                     size_t len) {
  return string_skip_blank(c, len) == c + len;
}

bool string_is_blank(const char *c) {
  size_t len = c == nullptr ? 0 : strlen(c);
  return string_is_blank(c, len);
}

bool string_is_not_empty(const char *c,
                         size_t len) {
  return !string_is_empty(c, len);
}

bool string_is_not_blank(const char *c,
                         size_t len) {
  return !string_is_blank(c, len);
}

bool string_is_newline(char c) {
  return c == '\n';
}

bool string_is_not_newline(char c) {
  return c != '\n';
}

bool string_is_blank(char c) {
  return isspace(c);
}

bool string_is_not_blank(char c) {
  return !isspace(c);
}

char *string_dup(const char *c) {
  return string_dup(c, strlen(c));
}

char *string_dup(const char *c,
                 size_t len) {
  if (c == nullptr) {
    return nullptr;
  }

  char *nc = new (std::nothrow) char[len];
  if (nc == nullptr) {
    return nullptr;
  }

  memcpy(nc, c, len);
  nc[len] = '\0';
  return nc;
}

const char *string_skip_char(const char *c,
                             size_t len,
                             char_skipper_t skipper) {
  size_t i = 0;
  while (i < len && skipper(*c))
  { c++; i++; }
  return c;
}

const char *string_skip_line(const char *c,
                             size_t len) {
  return string_skip_char(c, len, string_is_not_newline);
}

const char *string_skip_blank(const char *c,
                              size_t len) {
  return string_skip_char(c, len, string_is_blank);
}

const char *string_skip_non_blank(const char *c,
                                  size_t len) {
  return string_skip_char(c, len, string_is_not_blank);
}

/// strntol is a small modification of
/// https://github.com/gcc-mirror/gcc/blob/master/libiberty/strtol.c
/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
int64_t strntol(const char *nptr,
                size_t len,
                const char **endptr,
                int base) {
  const char *s = nptr;
  uint64_t acc;
  int c;
  uint64_t cutoff;
  int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *s++;
    len--;
  } while (len > 0 && isspace(c));

  if (len > 0) {
    if (c == '-') {
      neg = 1;
      c = *s++;
      len--;
    } else if (c == '+') {
      c = *s++;
      len--;
    }

    if (len > 1 && (base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
      c = s[1];
      s += 2;
      len -= 2;
      base = 16;
    }
  }

  if (base == 0) {
    base = c == '0' ? 8 : 10;
  }

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? -(uint64_t)LONG_MIN : LONG_MAX;
  cutlim = cutoff % (uint64_t)base;
  cutoff /= (uint64_t)base;
  for (acc = 0, any = 0;; c = *s++, len--) {
    if (isdigit(c)) {
      c -= '0';

    } else if (isalpha(c)) {
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;

    } else {
      break;
    }

    if (c >= base) {
      break;
    }

    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;

    } else {
      any = 1;
      acc *= base;
      acc += c;
    }

    if (len == 0) {
      break;
    }
  }

  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
    errno = ERANGE;
  } else if (neg) {
    acc = -acc;
  }

  if (endptr != 0) {
    *endptr = any ? s - 1 : nptr;
  }

  return (acc);
}

// strntoul is a small modification of
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/strtoul.c
/*
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
uint64_t strntoul(const char *nptr,
                  size_t len,
                  const char **endptr,
                  int base) {
  const char *s = nptr;
  uint64_t acc;
  int c;
  uint64_t cutoff;
  int neg = 0, any, cutlim;

  /*
   * See strtol for comments as to the logic used.
   */
  do {
    c = *s++;
    len--;
  } while (len > 0 && isspace(c));

  if (len > 0) {
    if (c == '-') {
      neg = 1;
      c = *s++;
      len--;
    } else if (c == '+') {
      c = *s++;
      len--;
    }
    if (len > 1 && (base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
      c = s[1];
      s += 2;
      len -= 2;
      base = 16;
    }
  }

  if (base == 0) {
    base = c == '0' ? 8 : 10;
  }

  cutoff = (uint64_t)ULONG_MAX / (uint64_t)base;
  cutlim = (uint64_t)ULONG_MAX % (uint64_t)base;
  for (acc = 0, any = 0;; c = *s++, len--) {
    if (isdigit(c)) {
      c -= '0';
    } else if (isalpha(c)) {
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    } else {
      break;
    }

    if (c >= base) {
      break;
    }

    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;

    } else {
      any = 1;
      acc *= base;
      acc += c;
    }

    if (len == 0) {
      break;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
    errno = ERANGE;
  } else if (neg) {
    acc = -acc;
  }

  if (endptr != 0)
    *endptr = any ? s - 1 : nptr;
  return (acc);
}
