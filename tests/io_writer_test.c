#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#define XSTD_IO_WRITER_IMPLEMENTATION
#include "xstd_io_writer.h"

START_TEST(test_file_writer) {
  FILE *f = fopen("/dev/null", "w");
  FileWriter fwriter = file_writer(f);
  Writer *writer = (Writer *)&fwriter;

  uint8_t buf[128] = {0};
  size_t write = 0;
  int error = 0;

  writer_write(writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);

  writer_write(writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);

  writer_write(writer, buf, 128, &write, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(write, 128);
}
END_TEST

static Suite *io_writer_suite(void) {
  Suite *s = suite_create("io_writer");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_file_writer);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_writer_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
