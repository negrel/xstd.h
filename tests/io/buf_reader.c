// A simple comment for the tests.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#include "io/buf_reader.h"
#include "io/reader.h"

START_TEST(test_buf_reader) {
  FILE *f = fopen(__FILE__, "r");
  FileReader freader = file_reader(f);

  uint8_t breader_buf[128] = {0};
  BufReader breader = buf_reader(&freader.reader, &breader_buf[0], 128);
  Reader *reader = &breader.reader;

  uint8_t buf[32] = {0};
  size_t read = 0;
  int error = 0;

  reader_read(reader, &buf[0], 32, &read, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 32);
  ck_assert(strncmp((char *)buf, "// A simple comment for the test", 32) == 0);

  ck_assert_int_eq(buf_reader_buffered(&breader), 96);

  reader_read(reader, &buf[0], 32, &read, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 32);
  ck_assert(strncmp((char *)buf, "s.\n", 2) == 0);

  ck_assert_int_eq(buf_reader_buffered(&breader), 64);

  while (read != 0) {
    reader_read(reader, buf, 32, &read, &error);
  }

  ck_assert_int_eq(error, EOF);

  ck_assert_int_eq(buf_reader_buffered(&breader), 0);

  fclose(f);
}
END_TEST

static Suite *io_buf_reader_suite(void) {
  Suite *s = suite_create("io_buf_reader");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_buf_reader);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_buf_reader_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
