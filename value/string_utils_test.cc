// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include "test/test.hpp"

#include "string_utils.hpp"

static int test_case_equals() {
  ASSERT_TRUE(string_case_equals("", ""));
  ASSERT_TRUE(string_case_equals("Word", "worD"));
  ASSERT_TRUE(string_case_equals("WORD", "WORD"));
  ASSERT_TRUE(string_case_equals("word", "WORD"));
  ASSERT_TRUE(string_case_equals("word", "word"));
  ASSERT_TRUE(string_case_equals("word", 4, "word", 4));
  ASSERT_TRUE(string_case_equals("word", 0, "words", 0));

  ASSERT_FALSE(string_case_equals("word", "words"));
  ASSERT_FALSE(string_case_equals("word", 4, "words", 5));
  ASSERT_FALSE(string_case_equals("", "word"));
  ASSERT_FALSE(string_case_equals(nullptr, "word"));
  ASSERT_FALSE(string_case_equals(nullptr, nullptr));
  ASSERT_FALSE(string_case_equals("word", nullptr));

  return EXIT_SUCCESS;
}

static int test_equals() {
  ASSERT_TRUE(string_equals("", ""));
  ASSERT_TRUE(string_equals("word", "word"));
  ASSERT_TRUE(string_equals("WORD", "WORD"));
  ASSERT_TRUE(string_equals("word", 4, "word", 4));
  ASSERT_TRUE(string_equals("word", 4, "words", 4));
  ASSERT_TRUE(string_equals("word", 0, "words", 0));

  ASSERT_FALSE(string_equals("word", "words"));
  ASSERT_FALSE(string_equals("word", 4, "words", 5));
  ASSERT_FALSE(string_equals("", "word"));
  ASSERT_FALSE(string_equals(nullptr, "word"));
  ASSERT_FALSE(string_equals(nullptr, nullptr));
  ASSERT_FALSE(string_equals("word", nullptr));
  ASSERT_FALSE(string_equals("Word", "worD"));
  ASSERT_FALSE(string_equals("word", "WORD"));

  return EXIT_SUCCESS;
}

static int test_string_is_blank() {
  ASSERT_TRUE(string_is_blank(" ", 1));
  ASSERT_TRUE(string_is_blank("\n", 1));
  ASSERT_TRUE(string_is_blank("\t", 1));
  ASSERT_TRUE(string_is_blank("    \t ", 6));
  ASSERT_TRUE(string_is_blank("\n\n\n", 3));
  ASSERT_TRUE(string_is_blank("\t    \n   ", 9));

  ASSERT_FALSE(string_is_blank("a", 1));
  ASSERT_FALSE(string_is_blank("#", 1));
  ASSERT_FALSE(string_is_blank("word", 4));
  ASSERT_FALSE(string_is_blank(" word", 5));
  ASSERT_FALSE(string_is_blank(" word\n", 6));

  return EXIT_SUCCESS;
}

static int test_string_is_empty() {
  ASSERT_TRUE(string_is_empty(nullptr, 0));
  ASSERT_TRUE(string_is_empty("", 0));
  ASSERT_TRUE(string_is_empty("\0", 1));

  ASSERT_FALSE(string_is_empty("a", 1));
  ASSERT_FALSE(string_is_empty("#", 1));
  ASSERT_FALSE(string_is_empty("word", 4));
  ASSERT_FALSE(string_is_empty(" word", 5));
  ASSERT_FALSE(string_is_empty(" word\n", 6));
  ASSERT_FALSE(string_is_empty(" ", 1));
  ASSERT_FALSE(string_is_empty("\n", 1));
  ASSERT_FALSE(string_is_empty("\t", 1));
  ASSERT_FALSE(string_is_empty("    \t ", 6));
  ASSERT_FALSE(string_is_empty("\n\n\n", 3));
  ASSERT_FALSE(string_is_empty("\t    \n   ", 9));

  return EXIT_SUCCESS;
}

static int test_is_blank() {
  ASSERT_TRUE(string_is_blank(' '));
  ASSERT_TRUE(string_is_blank('\n'));
  ASSERT_TRUE(string_is_blank('\t'));

  ASSERT_FALSE(string_is_blank('a'));
  ASSERT_FALSE(string_is_blank('#'));

  return EXIT_SUCCESS;
}

static int test_is_not_blank() {
  ASSERT_FALSE(string_is_not_blank(' '));
  ASSERT_FALSE(string_is_not_blank('\n'));
  ASSERT_FALSE(string_is_not_blank('\t'));

  ASSERT_TRUE(string_is_not_blank('a'));
  ASSERT_TRUE(string_is_not_blank('#'));

  return EXIT_SUCCESS;
}

static int test_skip_blank() {
  ASSERT_EQ(*string_skip_blank("   ", 3), '\0');
  ASSERT_EQ(*string_skip_blank("   word   ", 10), 'w');
  ASSERT_EQ(*string_skip_blank("\tword   ", 8), 'w');
  ASSERT_EQ(*string_skip_blank("", 0), '\0');
  ASSERT_EQ(*string_skip_blank("word", 4), 'w');

  return EXIT_SUCCESS;
}

static int test_skip_non_blank() {
  ASSERT_EQ(*string_skip_non_blank("   ", 3), ' ');
  ASSERT_EQ(*string_skip_non_blank("word   ", 7), ' ');
  ASSERT_EQ(*string_skip_non_blank("\t", 1), '\t');
  ASSERT_EQ(*string_skip_non_blank("\t\t\t    \t\n\n\t", 11), '\t');
  ASSERT_EQ(*string_skip_non_blank("word", 4), '\0');

  return EXIT_SUCCESS;
}

static int test_strntol() {
  ASSERT_EQ(strntol("1234", 4, nullptr, 10), 1234);
  ASSERT_EQ(strntol("-1234", 5, nullptr, 10), -1234);
  ASSERT_EQ(strntol("\0", 0, nullptr, 10), 0);
  ASSERT_EQ(strntol("1234a", 5, nullptr, 10), 1234);
  ASSERT_EQ(strntol("1234 5678", 9, nullptr, 10), 1234);
  ASSERT_EQ(strntol("12345678", 8, nullptr, 10), 12345678);

  return EXIT_SUCCESS;
}

static int test_strntoul() {
  ASSERT_EQ(strntoul("1234", 4, nullptr, 10), 1234);
  ASSERT_EQ(strntoul("-1234", 5, nullptr, 10),
            static_cast<uint64_t>(-1234));
  ASSERT_EQ(strntoul("\0", 0, nullptr, 10), 0);
  ASSERT_EQ(strntoul("1234a", 5, nullptr, 10), 1234);
  ASSERT_EQ(strntoul("1234 5678", 9, nullptr, 10), 1234);
  ASSERT_EQ(strntoul("12345678", 8, nullptr, 10), 12345678);

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_case_equals());
  TEST_RUN(ctx, test_equals());
  TEST_RUN(ctx, test_string_is_blank());
  TEST_RUN(ctx, test_string_is_empty());
  TEST_RUN(ctx, test_is_blank());
  TEST_RUN(ctx, test_is_not_blank());
  TEST_RUN(ctx, test_skip_blank());
  TEST_RUN(ctx, test_skip_non_blank());
  TEST_RUN(ctx, test_strntol());
  TEST_RUN(ctx, test_strntoul());

  TEST_RELEASE(ctx);

  return EXIT_SUCCESS;
}
