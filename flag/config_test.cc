// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test/test.hpp"

#include "config.hpp"
#include "status.hpp"
#include "buffer/stream_buffer.hpp"
#include <string.h>

static Config make_config(const char *content,
                          size_t len) {
  size_t wbytes;
  auto buffer = std::make_unique<StreamBuffer>(128);
  buffer->write(reinterpret_cast<const uint8_t*>(content), len, &wbytes);
  auto recoverer = std::make_unique<RecovererBuffer>(std::move(buffer));
  auto scanner = std::make_unique<Scanner>(std::move(recoverer));
  return Config(std::move(scanner));
}

static int test_config_parse_string() {
  const char *content = "key value";
  const char *error;
  std::string value;
  auto config = make_config(content, strlen(content));

  ASSERT_TRUE(config.string_var(&value, "key", "key for a test"));
  ASSERT_EQ(config.parse(&error), OK);
  ASSERT_EQ(value.compare("value"), 0);

  return EXIT_SUCCESS;
}

static int test_config_parse_bool_true() {
  const char *content = "key true";
  const char *error;
  bool value;

  auto config = make_config(content, strlen(content));

  ASSERT_TRUE(config.bool_var(&value, "key", "key for a test"));
  ASSERT_EQ(config.parse(&error), OK);
  ASSERT_TRUE(value);

  return EXIT_SUCCESS;
}

static int test_config_parse_bool_invalid() {
  const char *content = "key random";
  const char *error;
  bool value;

  auto config = make_config(content, strlen(content));

  ASSERT_TRUE(config.bool_var(&value, "key", "key for a test"));
  ASSERT_EQ(config.parse(&error), ConfigParseValue);

  return EXIT_SUCCESS;
}

static int test_config_parse_bool_false() {
  const char *content = "key false";
  const char *error;
  bool value;

  auto config = make_config(content, strlen(content));

  ASSERT_TRUE(config.bool_var(&value, "key", "key for a test"));
  ASSERT_EQ(config.parse(&error), OK);
  ASSERT_FALSE(value);

  return EXIT_SUCCESS;
}

static int test_config_parse_multiple_properties() {
  const char *content = "prop1 value1\n"
                        "prop2 value2\n"
                        "prop3 value3";
  const char *error;
  std::string value1, value2, value3;

  auto config = make_config(content, strlen(content));

  ASSERT_TRUE(config.string_var(&value1, "prop1", "prop1 for a test"));
  ASSERT_TRUE(config.string_var(&value2, "prop2", "prop2 for a test"));
  ASSERT_TRUE(config.string_var(&value3, "prop3", "prop3 for a test"));

  ASSERT_EQ(config.parse(&error), OK);

  ASSERT_EQ(value1.compare("value1"), 0);
  ASSERT_EQ(value2.compare("value2"), 0);
  ASSERT_EQ(value3.compare("value3"), 0);

  return EXIT_SUCCESS;
}

static int test_config_parse_ignore_comments() {
  const char *content = "#\n"
                        " \t#\n";
  const char *error;
  auto config = make_config(content, strlen(content));

  ASSERT_EQ(config.parse(&error), OK);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_config_parse_string());
  TEST_RUN(ctx, test_config_parse_bool_true());
  TEST_RUN(ctx, test_config_parse_bool_invalid());
  TEST_RUN(ctx, test_config_parse_bool_false());
  TEST_RUN(ctx, test_config_parse_multiple_properties());
  TEST_RUN(ctx, test_config_parse_ignore_comments());

  TEST_RELEASE(ctx);
  return EXIT_SUCCESS;
}
