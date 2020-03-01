// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test/test.hpp"

#include "stream_buffer.hpp"
#include "scanner.hpp"

static Scanner scanner_from_content(
    char *content,
    size_t len) {
  auto buffer = std::make_unique<StreamBuffer>(
      reinterpret_cast<uint8_t*>(content), len, do_nothing_delete_dispose_func);
  buffer->extend(len);
  return Scanner(std::make_unique<RecovererBuffer>(std::move(buffer)));
}

static int test_scanner_read_line_empty() {
  char content[] = "\0";
  const uint8_t *data;
  size_t rbytes;
  auto scanner = scanner_from_content(content, 0);

  ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
  ASSERT_EQ(rbytes, 0);

  return EXIT_SUCCESS;
}

static int test_scanner_read_line_newline_only() {
  char content[] = "\n\n\n\n\n";
  const uint8_t *data;
  size_t rbytes;
  auto scanner = scanner_from_content(content, strlen(content));

  ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
  ASSERT_EQ(scanner.consume(rbytes), 0);

  return EXIT_SUCCESS;
}

static int test_scanner_read_line_with_newline() {
  char content[] = "some bytes\n";
  const uint8_t *data;
  size_t rbytes;
  auto scanner = scanner_from_content(content, strlen(content));

  ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
  ASSERT_EQ(rbytes, strlen(content) - 1);
  ASSERT_MEM_EQ(data, content, rbytes);

  ASSERT_EQ(scanner.consume(rbytes), rbytes);
  ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
  ASSERT_EQ(rbytes, 0);

  return EXIT_SUCCESS;
}

static int test_scanner_read_multiple_lines() {
  char content[] = "line0\nline1\nline2\nline3\nline4";
  char expected[] = "line ";
  const uint8_t *data;
  size_t rbytes;
  auto scanner = scanner_from_content(content, strlen(content));

  for (int i = 0; i < 5; i++) {
    expected[4] = 48 + i;
    ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
    ASSERT_EQ(rbytes, strlen(expected));
    ASSERT_MEM_EQ(expected, data, rbytes);
    ASSERT_EQ(scanner.consume(rbytes), rbytes);
  }

  ASSERT_EQ(scanner.peek(&data, 0, &rbytes), OK);
  ASSERT_EQ(rbytes, 0);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_scanner_read_line_empty());
  TEST_RUN(ctx, test_scanner_read_line_newline_only());
  TEST_RUN(ctx, test_scanner_read_line_with_newline());
  TEST_RUN(ctx, test_scanner_read_multiple_lines());

  return TEST_RELEASE(ctx);
}

