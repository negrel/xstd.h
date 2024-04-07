#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#define XSTD_IO_CLOSER_IMPLEMENTATION
#include "xstd_io_closer.h"

START_TEST(test_file_closer) {
  FILE *f = fopen(__FILE__, "r");
  FileCloser fc = file_closer(f);
  Closer *closer = (Closer *)&fc;

  int error = 0;
  closer_close(closer, &error);
  ck_assert_int_eq(error, 0);
}
END_TEST

static Suite *io_closer_suite(void) {
  Suite *s = suite_create("io_closer");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_file_closer);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = io_closer_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
