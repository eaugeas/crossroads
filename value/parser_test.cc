// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test/test.hpp"

#include "parser.hpp"

static int test_parse_bool() {
  bool value = true;

  ASSERT_TRUE(parse_bool(&value, "false", 5));
  ASSERT_FALSE(value);
  ASSERT_TRUE(parse_bool(&value, "true", 4));
  ASSERT_TRUE(value);
  ASSERT_TRUE(parse_bool(&value, "12345", 5));
  ASSERT_TRUE(value);
  ASSERT_TRUE(parse_bool(&value, "00000", 5));
  ASSERT_FALSE(value);

  ASSERT_FALSE(parse_bool(&value, "false", 4));
  ASSERT_FALSE(parse_bool(&value, "true", 3));
  ASSERT_FALSE(parse_bool(&value, "false", 4));
  ASSERT_FALSE(parse_bool(&value, "true", 3));
  ASSERT_FALSE(parse_bool(&value, "4123asdf", 8));

  return EXIT_SUCCESS;
}

static int test_parse_int32() {
  int32_t value;

  ASSERT_TRUE(parse_int32(&value, "1234", 4));
  ASSERT_EQ(value, 1234);

  ASSERT_TRUE(parse_int32(&value, "0", 1));
  ASSERT_EQ(value, 0);

  ASSERT_TRUE(parse_int32(&value, "-1234", 5));
  ASSERT_EQ(value, -1234);

  ASSERT_TRUE(parse_int32(&value, "", 0));
  ASSERT_EQ(value, 0);

  ASSERT_FALSE(parse_int32(&value, "123456789987654321", 18));

  return EXIT_SUCCESS;
}

static int test_parse_int64() {
  int64_t value;

  ASSERT_TRUE(parse_int64(&value, "1234", 4));
  ASSERT_EQ(value, 1234);

  ASSERT_TRUE(parse_int64(&value, "0", 1));
  ASSERT_EQ(value, 0);

  ASSERT_TRUE(parse_int64(&value, "-1234", 5));
  ASSERT_EQ(value, -1234);

  ASSERT_TRUE(parse_int64(&value, "", 0));
  ASSERT_EQ(value, 0);

  ASSERT_TRUE(parse_int64(&value, "123456789987654321", 18));
  ASSERT_EQ(value, 123456789987654321);

  return EXIT_SUCCESS;
}

static int test_parse_uint32() {
  uint32_t value;

  ASSERT_TRUE(parse_uint32(&value, "1234", 4));
  ASSERT_EQ(value, 1234);

  ASSERT_TRUE(parse_uint32(&value, "0", 1));
  ASSERT_EQ(value, 0);

  ASSERT_FALSE(parse_uint32(&value, "-1234", 5));

  ASSERT_TRUE(parse_uint32(&value, "", 0));
  ASSERT_EQ(value, 0);

  ASSERT_FALSE(parse_uint32(&value, "123456789987654321", 18));

  return EXIT_SUCCESS;
}

static int test_parse_uint64() {
  uint64_t value;

  ASSERT_TRUE(parse_uint64(&value, "1234", 4));
  ASSERT_EQ(value, 1234);

  ASSERT_TRUE(parse_uint64(&value, "0", 1));
  ASSERT_EQ(value, 0);

  ASSERT_TRUE(parse_uint64(&value, "-1234", 5));
  ASSERT_EQ(value, static_cast<uint64_t>(-1234));

  ASSERT_TRUE(parse_uint64(&value, "", 0));
  ASSERT_EQ(value, 0);

  ASSERT_TRUE(parse_uint64(&value, "123456789987654321", 18));
  ASSERT_EQ(value, 123456789987654321);

  return EXIT_SUCCESS;
}

static int test_parse_duration() {
  std::chrono::nanoseconds value;

  ASSERT_TRUE(parse_duration(&value, "1h", 2));
  ASSERT_EQ(value.count(), 3600 * 1e9);
  ASSERT_TRUE(parse_duration(&value, "1m", 2));
  ASSERT_EQ(value.count(), 60 * 1e9);
  ASSERT_TRUE(parse_duration(&value, "1s", 2));
  ASSERT_EQ(value.count(), 1e9);
  ASSERT_TRUE(parse_duration(&value, "1ms", 3));
  ASSERT_EQ(value.count(), 1e6);
  ASSERT_TRUE(parse_duration(&value, "1us", 3));
  ASSERT_EQ(value.count(), 1e3);
  ASSERT_TRUE(parse_duration(&value, "1ns", 3));
  ASSERT_EQ(value.count(), 1);

  ASSERT_FALSE(parse_duration(&value, "1hs", 3));
  ASSERT_FALSE(parse_duration(&value, "1mu", 3));
  ASSERT_FALSE(parse_duration(&value, "1ss", 3));
  ASSERT_FALSE(parse_duration(&value, "1uu", 3));
  ASSERT_FALSE(parse_duration(&value, "1nu", 3));
  ASSERT_FALSE(parse_duration(&value, "hs", 2));
  ASSERT_FALSE(parse_duration(&value, "mu", 2));
  ASSERT_FALSE(parse_duration(&value, "ss", 2));
  ASSERT_FALSE(parse_duration(&value, "uu", 2));
  ASSERT_FALSE(parse_duration(&value, "nu", 2));
  ASSERT_FALSE(parse_duration(&value, "", 0));

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_parse_bool());
  TEST_RUN(ctx, test_parse_int32());
  TEST_RUN(ctx, test_parse_int64());
  TEST_RUN(ctx, test_parse_uint32());
  TEST_RUN(ctx, test_parse_uint64());
  TEST_RUN(ctx, test_parse_duration());

  TEST_RELEASE(ctx);

  return EXIT_SUCCESS;
}
