#include <check.h>
#include <stdint.h>
#include <stdlib.h>

#define XSTD_IMPLEMENTATION
#include "iter.h"

START_TEST(test_range_iter) {
  // Simple iteration.
  size_t i = 0;
  intmax_t iter;
  for (RangeIterator range = range_iterator(0, 3, 1);
       iter_next(&range.iface, &iter);) {
    ck_assert_int_eq(i, iter);
    i++;
  }
  ck_assert_int_eq(i, 2);

  // Step bigger than end.
  i = 0;
  for (RangeIterator range = range_iterator(0, 3, 4);
       iter_next(&range.iface, &iter);) {
    if (iter == 0) {
      ck_assert_int_eq(i, 0);
    } else {
      ck_assert_int_eq(i, 4);
    }
    i++;
  }
  ck_assert_int_eq(i, 0);
}
END_TEST

static Suite *iter_suite(void) {
  Suite *s = suite_create("iter");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_range_iter);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = iter_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
