// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test.hpp"

static int test_assert_true() {
  ASSERT_TRUE(true);
  ASSERT_FALSE(false);
  ASSERT_TRUE(1);
  ASSERT_FALSE(0);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_assert_true());

  TEST_RELEASE(ctx);
  return EXIT_SUCCESS;
}
