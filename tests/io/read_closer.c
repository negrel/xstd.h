#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#define XSTD_IMPLEMENTATION
#include "io/read_closer.h"

START_TEST(test_file_read_closer_read) {
  FILE *f = fopen(__FILE__, "r");
  FileReadCloser frc = file_read_closer(f);
  ReadCloser *read_closer = (ReadCloser *)&frc;

  uint8_t buf[128] = {0};
  size_t read = 0;
  int error = 0;

  reader_read(&read_closer->reader, buf, 128, &read, &error);

  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 128);

  while (read != 0) {
    reader_read(&read_closer->reader, buf, 128, &read, &error);
  }
  ck_assert_int_eq(error, EOF);

  fclose(f);
}
END_TEST

START_TEST(test_file_read_closer_close) {
  errno = 0;

  FILE *f = fopen(__FILE__, "r");
  FileReadCloser frc = file_read_closer(f);
  ReadCloser *read_closer = (ReadCloser *)&frc;

  uint8_t buf[128] = {0};
  size_t read = 0;
  int error = 0;

  reader_read(&read_closer->reader, buf, 128, &read, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 128);

  closer_close(&read_closer->closer, &error);
}
END_TEST

static Suite *io_read_closer_suite(void) {
  Suite *s = suite_create("io_read_closer");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_file_read_closer_read);
  tcase_add_test(tc_core, test_file_read_closer_close);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_read_closer_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
