#include "alloc.h"
#include <stdlib.h>

#include <check.h>

#define XSTD_IMPLEMENTATION
#include "dlist.h"

START_TEST(test_dlist) {
  DList dl = dlist(g_libc_allocator);
  (void)dl;
}
END_TEST

static Suite *list_suite(void) {
  Suite *s = suite_create("list");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_dlist);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = list_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
