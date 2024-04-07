#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>
#include <string.h>

#define XSTD_IO_READ_WRITER_IMPLEMENTATION
#include "xstd_io_read_writer.h"

START_TEST(test_file_read_writer_read) {
  FILE *f = fopen(__FILE__, "r");
  FileReadWriter frw = file_read_writer(f);
  ReadWriter *read_writer = (ReadWriter *)&frw;

  uint8_t buf[128] = {0};
  size_t read = 0;
  int error = 0;

  reader_read(&read_writer->reader, buf, 128, &read, &error);

  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 128);

  while (read != 0) {
    reader_read(&read_writer->reader, buf, 128, &read, &error);
  }

  ck_assert_int_eq(error, EOF);
}
END_TEST

START_TEST(test_file_read_writer_write) {
  errno = 0;

  FILE *f = fopen("/dev/null", "w");
  FileReadWriter frw = file_read_writer(f);
  ReadWriter *read_writer = &frw.read_writer;

  uint8_t buf[128] = {0};
  size_t write = 0;
  int error = 0;

  writer_write(&read_writer->writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);

  writer_write(&read_writer->writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);

  writer_write(&read_writer->writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);
}
END_TEST

static Suite *io_read_writer_suite(void) {
  Suite *s = suite_create("io_read_writer");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_file_read_writer_read);
  tcase_add_test(tc_core, test_file_read_writer_write);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_read_writer_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
