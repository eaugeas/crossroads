// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test.hpp"

void test_print_help(const char *prog) {
  printf("usage: %s [-v] [-h] [-t]\n", prog);
  printf("\noptions:\n");
  printf("\t-v, --verbose: outputs information about tests run\n");
  printf("\t-h, --help: prints this menu\n");
  printf("\t-t, --test: expression to match against tests to run\n");
  printf("\t-b, --bench: expression to match against benchmarks to run\n");
  printf("\n");
}

int test_release(test_ctx_t *ctx) {
  free(ctx->test_expr);
  free(ctx->bench_expr);
  printf("test sucess: %d, failed: %d\n", ctx->success, ctx->failure);

  if (ctx->failure > 0) {
    return EXIT_FAILURE;

  } else {
    return EXIT_SUCCESS;
  }
}

int test_ctx_init(
    test_ctx_t *ctx,
    int argc,
    char *argv[]
) {
  ctx->test_expr = static_cast<char*>(malloc(128 * sizeof(char)));
  memset(ctx->test_expr, 0, 128);
  ctx->bench_expr = static_cast<char*>(malloc(128 * sizeof(char)));
  memset(ctx->bench_expr, 0, 128);
  ctx->failure = 0;
  ctx->success = 0;
  ctx->verbose = 0;

  while (1) {
    static struct option long_options[] = {
      {"test", required_argument, 0, 't'},
      {"bench", required_argument, 0, 'b'},
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
    };

    size_t len;
    int option_index = 0;
    int c = getopt_long(argc, argv, "t:b:vh", long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
    case 't':
      len = strlen(optarg) > 127 ? 127 : strlen(optarg);
      strncpy(ctx->test_expr, optarg, len);
      ctx->test_expr[len] = '\0';
      break;
    case 'b':
      len = strlen(optarg) > 127 ? 127 : strlen(optarg);
      strncpy(ctx->bench_expr, optarg, len);
      ctx->bench_expr[len] = '\0';
      break;
    case 'v':
      ctx->verbose = 1;
      break;
    case 'h':
      test_print_help(argv[0]);
      test_release(ctx);
      return -11;
    case '?':
      // getopt_long already printed an error message
      return -1;
    default:
      break;
    }
  }

  return 0;
}
