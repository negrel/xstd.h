// A simple comment for the tests.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#include "io/reader.h"

START_TEST(test_file_reader) {
  FILE *f = fopen(__FILE__, "r");
  FileReader freader = file_reader(f);
  Reader *reader = (Reader *)&freader;

  uint8_t buf[32] = {0};
  size_t read = 0;
  int error = 0;

  reader_read(reader, &buf[0], 32, &read, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 32);
  ck_assert(strncmp((char *)buf, "// A simple comment for the test", 32) == 0);

  reader_read(reader, &buf[0], 32, &read, &error);
  ck_assert_int_eq(error, 0);
  ck_assert_int_eq(read, 32);
  ck_assert(strncmp((char *)buf, "s.\n", 2) == 0);

  while (read != 0) {
    reader_read(reader, buf, 32, &read, &error);
  }

  ck_assert_int_eq(error, EOF);

  fclose(f);
}
END_TEST

static Suite *io_reader_suite(void) {
  Suite *s = suite_create("io_reader");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_file_reader);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_reader_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
