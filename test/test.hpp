// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct test_ctx_s {
  char *test_expr;
  char *bench_expr;
  int failure;
  int success;
  int verbose;
} test_ctx_t;

#define COND_EQ(a, b) ((a) == (b))
#define COND_MEM_EQ(a, b, l) (memcmp((a), (b), (l)) == 0)
#define COND_TRUE(a) COND_EQ(!!a, 1)
#define COND_FALSE(a) COND_EQ(!a, 1)

#define STRING_ERROR "assertion error %s %s at line %d: "

#define PRINT_ERR(a, b, COND)                            \
  fprintf(stderr, STRING_ERROR #a " " #COND " " #b "\n", \
          __FILE__, __FUNCTION__, __LINE__);

#define ASSERT_COND1(a, COND)   \
  if (!COND(a)) {               \
    PRINT_ERR(a, a, COND)       \
    return EXIT_FAILURE;        \
  }

#define ASSERT_COND2(a, b, COND) \
  if (!COND(a, b)) {             \
    PRINT_ERR(a, b, COND)        \
    return EXIT_FAILURE;         \
  }

#define ASSERT_COND3(a, b, n, COND) \
  if (!COND(a, b, n)) {             \
    PRINT_ERR(a, b, COND)           \
    return EXIT_FAILURE;            \
  }

#define ASSERT_MEM_EQ(a, b, l) ASSERT_COND3((a), (b), (l), COND_MEM_EQ)
#define ASSERT_EQ(a, b) ASSERT_COND2((a), (b), COND_EQ)
#define ASSERT_TRUE(a) ASSERT_COND1((a), COND_TRUE)
#define ASSERT_FALSE(a) ASSERT_COND1((a), COND_FALSE)

void test_print_help(const char *prog);
int test_release(test_ctx_t *ctx);
int test_ctx_init(
    test_ctx_t *ctx,
    int argc,
    char *argv[]);

#define TEST_RELEASE(ctx) test_release(&ctx);

#define TEST_INIT(ctx, argc, argv)         \
  if (test_ctx_init(&ctx, argc, argv)) { \
    return EXIT_SUCCESS;                   \
  }                                        \

#define BENCH_RUN(ctx, test) bench_run(&ctx, #test, test)

static double clock_nns_time() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (now.tv_sec * 1e9) + now.tv_nsec;
}

static int abs_between(double value, double lower, double upper) {
  value = value < 0 ? -value : value;
  return value > lower && value < upper;
}

void bench_run(test_ctx_t* ctx, const char* name, int (*bench)(int)) {
  int incr = 4;
  int max_iter = 1 << 24;
  double max_error = 0.02;
  double ops = 0;
  int iter;
  size_t maxlen = strlen(name) > strlen(ctx->bench_expr)
    ? strlen(ctx->bench_expr) : strlen(name);

  if (ctx->bench_expr == NULL ||
      ctx->bench_expr[0] == '\0' ||
      strncmp(name, ctx->bench_expr, maxlen) == 0) {
    return;
  }

  for (iter = 1 << 10; iter < max_iter; iter <<= incr) {
    double start = clock_nns_time();
    bench(iter);
    double end = clock_nns_time();
    double total = end - start;
    double curr = static_cast<double>(iter / total);
    int acceptable = curr > 0 && ops > 0 &&
      abs_between(curr / ops, 1 - max_error, 1);
    ops = curr;
    // repeat until the error obtained is acceptable
    if (acceptable) {
      break;
    }
  }

  // when presenting the results try to present them in the time unit
  // in which it is easier to understand it
  int counter = 0;
  for (; counter < 4 && ops < 1; counter++) {
    ops *= 1000;
  }

  const char* unit = "ns";
  switch (counter) {
  case 0:
    unit = "ns";
    break;
  case 1:
    unit = "us";
    break;
  case 2:
    unit = "ms";
    break;
  case 3:
    unit = "s";
    break;
  default:
    abort();
  }

  printf("%s - %lf ops/%s - %d iter\n", name, ops, unit, iter);
}

#define TEST_RUN(ctx, test)                                  \
  if (strlen((ctx).test_expr) == 0 ||                        \
      strstr(#test, (ctx).test_expr) != NULL) {              \
    if ((ctx).verbose) printf("running " #test "\n");        \
    int result = test;                                     \
    if (result == EXIT_FAILURE) {                            \
      if ((ctx).verbose) printf("test " #test " failed\n");  \
      (ctx).failure++;                                       \
    } else {                                                 \
      if ((ctx).verbose) printf("test " #test " succeeded\n");  \
      (ctx).success++;                                          \
    }                                                        \
  }                                                          \

#endif  // TEST_TEST_H_
