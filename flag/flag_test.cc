// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test/test.hpp"

#include "flag.hpp"

static int test_flag_parse_string() {
  bool ok;
  std::string value;
  int argc = 3;
  const char *argv[3];
  Flag flags;

  argv[0] = "program";
  argv[1] = "-s";
  argv[2] = "value";

  flags.string_var(&value, 's', "string", "a string value");
  ok = flags.parse(argc, argv);

  ASSERT_TRUE(ok);
  ASSERT_EQ(value.compare("value"), 0);

  return EXIT_SUCCESS;
}

static int test_flag_parse_bool() {
  bool ok;
  bool value = false;
  int argc = 2;
  const char *argv[2];
  Flag flags;

  argv[0] = "program";
  argv[1] = "-b";

  flags.bool_var(&value, 'b', "bool", "a bool value");
  ok = flags.parse(argc, argv);

  ASSERT_TRUE(ok);
  ASSERT_TRUE(value);

  return EXIT_SUCCESS;
}

static int test_flag_parse_unknown_arg() {
  bool ok;
  bool value = false;
  int argc = 2;
  const char *argv[2];
  Flag flags;

  argv[0] = "program";
  argv[1] = "-c";

  flags.bool_var(&value, 'b', "bool", "a bool value");
  ok = flags.parse(argc, argv);

  ASSERT_FALSE(ok);

  return EXIT_SUCCESS;
}

static int test_flag_parse_bool_unset() {
  bool ok;
  bool value = false;
  int argc = 1;
  const char *argv[1];
  Flag flags;

  argv[0] = "program";

  flags.bool_var(&value, 'b', "bool", "a bool value");
  ok = flags.parse(argc, argv);

  ASSERT_TRUE(ok);
  ASSERT_FALSE(value);

  return EXIT_SUCCESS;
}

static int test_flag_parse_int32() {
  bool ok;
  int32_t value = 0;
  int argc = 3;
  const char *argv[3];
  Flag flags;

  argv[0] = "program";
  argv[1] = "-i";
  argv[2] = "100";

  flags.int32_var(&value, 'i', "integer", "an integer value");
  ok = flags.parse(argc, argv);

  ASSERT_TRUE(ok);
  ASSERT_EQ(value, 100);

  return EXIT_SUCCESS;
}

static int test_flag_parse_int32_longopt() {
  bool ok;
  int32_t value = 0;
  int argc = 3;
  const char *argv[3];
  Flag flags;

  argv[0] = "program";
  argv[1] = "--integer";
  argv[2] = "100";

  flags.int32_var(&value, 'i', "integer", "an integer value");
  ok = flags.parse(argc, argv);

  ASSERT_TRUE(ok);
  ASSERT_EQ(value, 100);

  return EXIT_SUCCESS;
}

static int test_flag_parse_uint32_negative() {
  bool ok;
  uint32_t value = 0;
  int argc = 3;
  const char *argv[3];
  Flag flags;

  argv[0] = "program";
  argv[1] = "-i";
  argv[2] = "-100";

  flags.uint32_var(&value, 'i', "integer", "an integer value");
  ok = flags.parse(argc, argv);

  ASSERT_FALSE(ok);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_flag_parse_string());
  TEST_RUN(ctx, test_flag_parse_bool());
  TEST_RUN(ctx, test_flag_parse_bool_unset());
  TEST_RUN(ctx, test_flag_parse_unknown_arg());
  TEST_RUN(ctx, test_flag_parse_int32());
  TEST_RUN(ctx, test_flag_parse_int32_longopt());
  TEST_RUN(ctx, test_flag_parse_uint32_negative());

  TEST_RELEASE(ctx);
  return EXIT_SUCCESS;
}
