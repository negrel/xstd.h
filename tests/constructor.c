#include <check.h>
#include <stdlib.h>

static Suite *constructor_suite(void) {
  Suite *s = suite_create("constructor");
  TCase *tc_core = tcase_create("Core");

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = constructor_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
