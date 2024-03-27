#include <stdlib.h>

#include <check.h>

#define XSTD_LIST_IMPLEMENTATION
#include "xstd_list.h"

START_TEST(test_list_push) {
  int *list = NULL;
  list_push(&list); // Zeroed by default
  ck_assert_int_eq(*list, 0);

  *list_push(&list) = 2;
  ck_assert_int_eq(*list, 2);

  *list_push(&list) = 3;
  ck_assert_int_eq(*list, 3);
  ck_assert_int_eq(*list_next(list), 2);
  ck_assert_int_eq(*list_next(list_next(list)), 0);

  list_free(list);
}
END_TEST

START_TEST(test_list_remove_next) {
  int *list = NULL;

  int *one = list_push(&list);
  *one = 1;
  ck_assert(list == one);

  int *two = list_push(&list);
  *two = 2;
  ck_assert(list == two);

  int *three = list_push(&list);
  *three = 3;
  ck_assert(list == three);

  ck_assert(list_next(three) == two);
  ck_assert(list_next(two) == one);

  // No next (no op).
  list_remove_next(one);

  ck_assert(list_next(three) == two);
  ck_assert(list_next(two) == one);
  ck_assert(list_next(one) == NULL);

  // next of two is end of list.
  list_remove_next(two);

  ck_assert(list_next(three) == two);
  ck_assert(list_next(two) == NULL);

  // second element of list.
  list_remove_next(three);
  ck_assert(list_next(three) == NULL);

  // First element it self.
  list_remove_head(&three);

  ck_assert(three == NULL);
}
END_TEST

START_TEST(test_list_remove_head) {
  int *list = NULL;
  list_push(&list);
  *list_push(&list) = 2;
  *list_push(&list) = 3;

  list_remove_head(&list);

  ck_assert_int_eq(*list, 2);
  ck_assert_int_eq(*list_next(list), 0);

  list_free(list);
}
END_TEST

START_TEST(test_list_free) {
  int *list = NULL;
  list_push(&list);
  *list_push(&list) = 2;
  *list_push(&list) = 3;

  list_free(list);
}
END_TEST

START_TEST(test_list_free_next) {
  int *list = NULL;
  list_push(&list);
  *list_push(&list) = 2;
  *list_push(&list) = 3;

  list_free_next(list);
  list_remove_head(&list);
}
END_TEST

static Suite *list_suite(void) {
  Suite *s = suite_create("list");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_list_push);
  tcase_add_test(tc_core, test_list_remove_next);
  tcase_add_test(tc_core, test_list_remove_head);
  tcase_add_test(tc_core, test_list_free);
  tcase_add_test(tc_core, test_list_free_next);

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
