// Copyright (c) 2019, tlblanc <tlblanc1490 at gmail dot com>

#include <string.h>

#include <memory>
#include "test/test.hpp"

#include "io/copy.hpp"

#include "stream_buffer.hpp"
#include "msg_buffer.hpp"

static constexpr uint8_t data[] = "some content";
static constexpr size_t kDataLen = 12;

template <typename T>
static int test_read_write() {
  auto buffer = std::unique_ptr<T>(new T(128));
  size_t wbytes, rbytes;
  uint8_t readdata[kDataLen];

  ASSERT_EQ(buffer->write(data, kDataLen, &wbytes), OK);
  ASSERT_EQ(wbytes, kDataLen);

  ASSERT_EQ(buffer->read(readdata, kDataLen, &rbytes), OK);
  ASSERT_EQ(rbytes, kDataLen);
  ASSERT_MEM_EQ(data, readdata, kDataLen);

  return EXIT_SUCCESS;
}

static int test_read_write_too_much_stream() {
  auto buffer = std::make_unique<StreamBuffer>(8);
  size_t writable, wbytes, rbytes;
  uint8_t readdata[kDataLen];

  writable = buffer->writable();
  ASSERT_EQ(writable, 8);
  ASSERT_EQ(buffer->write(data, kDataLen, &wbytes), OK);
  ASSERT_EQ(wbytes, writable);

  ASSERT_EQ(buffer->read(readdata, kDataLen, &rbytes), OK);
  ASSERT_EQ(rbytes, writable);
  ASSERT_MEM_EQ(data, readdata, 8);

  return EXIT_SUCCESS;
}

static int test_read_write_too_much_message() {
  auto buffer = std::make_unique<MsgBuffer>(8);

  size_t writable, wbytes, rbytes;
  uint8_t readdata[kDataLen];

  writable = buffer->writable();
  ASSERT_EQ(writable, 4);
  ASSERT_EQ(buffer->write(data, kDataLen, &wbytes), OK);
  ASSERT_EQ(wbytes, 0);

  ASSERT_EQ(buffer->read(readdata, kDataLen, &rbytes), OK);
  ASSERT_EQ(rbytes, 0);

  return EXIT_SUCCESS;
}

template <typename T>
static int test_copy_reader() {
  auto reader = std::make_unique<T>(128);
  auto writer = std::make_unique<T>(128);
  uint8_t readdata[kDataLen];
  size_t wbytes, rbytes, cbytes;

  ASSERT_EQ(reader->write(data, kDataLen, &wbytes), OK);
  ASSERT_EQ(wbytes, kDataLen);

  ASSERT_EQ(copy(reader.get(), dynamic_cast<Sink*>(writer.get()), &cbytes), OK);
  ASSERT_EQ(cbytes, kDataLen);

  ASSERT_EQ(writer->read(readdata, kDataLen, &rbytes), OK);
  ASSERT_EQ(rbytes, kDataLen);
  ASSERT_MEM_EQ(data, readdata, kDataLen);

  return EXIT_SUCCESS;
}

template <typename T>
static int test_copy_reader_twice_capacity() {
  size_t capacity = 128;
  size_t counter = 0, total = 128 << 1;
  auto reader = std::make_unique<T>(capacity);
  auto writer = std::make_unique<T>(capacity);
  uint8_t readdata[kDataLen];
  size_t wbytes, rbytes, cbytes;

  while (counter < total) {
    ASSERT_EQ(reader->write(data, kDataLen, &wbytes), OK);
    ASSERT_EQ(wbytes, kDataLen);

    ASSERT_EQ(copy(reader.get(), dynamic_cast<Sink*>(writer.get()), &cbytes), OK);
    ASSERT_EQ(cbytes, kDataLen);

    ASSERT_EQ(writer->read(readdata, kDataLen, &rbytes), OK);
    ASSERT_EQ(rbytes, kDataLen);
    ASSERT_MEM_EQ(data, readdata, kDataLen);
    counter += kDataLen;
  }

  return EXIT_SUCCESS;
}

template <typename T>
static int bench_copy(int n) {
  size_t capacity = 128;
  auto reader = std::make_unique<T>(capacity);
  auto writer = std::make_unique<T>(capacity);
  uint8_t readdata[kDataLen];
  size_t wbytes, rbytes, cbytes;

  for (int i = 0; i < n; i++) {
    ASSERT_EQ(reader->write(data, kDataLen, &wbytes), OK);
    ASSERT_EQ(wbytes, kDataLen);

    ASSERT_EQ(copy(reader.get(), dynamic_cast<Sink*>(writer.get()), &cbytes), OK);
    ASSERT_EQ(cbytes, kDataLen);

    ASSERT_EQ(writer->read(readdata, kDataLen, &rbytes), OK);
    ASSERT_EQ(rbytes, kDataLen);
    ASSERT_MEM_EQ(data, readdata, kDataLen);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  test_ctx_t ctx;
  TEST_INIT(ctx, argc, argv);

  TEST_RUN(ctx, test_read_write<StreamBuffer>());
  TEST_RUN(ctx, test_read_write_too_much_stream());
  TEST_RUN(ctx, test_read_write<MsgBuffer>());
  TEST_RUN(ctx, test_read_write_too_much_message());
  TEST_RUN(ctx, test_copy_reader<StreamBuffer>());
  TEST_RUN(ctx, test_copy_reader<MsgBuffer>());
  TEST_RUN(ctx, test_copy_reader_twice_capacity<StreamBuffer>());
  TEST_RUN(ctx, test_copy_reader_twice_capacity<MsgBuffer>());
  BENCH_RUN(ctx, bench_copy<StreamBuffer>);
  BENCH_RUN(ctx, bench_copy<MsgBuffer>);

  return TEST_RELEASE(ctx);
}
