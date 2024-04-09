#include <check.h>
#include <stdlib.h>

START_TEST(test_reader) {}
END_TEST

static Suite *reader_suite(void) {
  Suite *s = suite_create("reader");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_reader);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = reader_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
