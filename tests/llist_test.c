#include "xstd_llist.h"
#include <stdlib.h>

#include <check.h>

#define XSTD_LINKED_LIST_IMPLEMENTATION
#include "xstd_llist.h"

START_TEST(test_llist_append) {
  LinkedList llist;
  llist_init(&llist);

  llist_append(&llist, 1);
  ck_assert_int_eq(llist_item_value(llist.first, int), 1);
  ck_assert_int_eq(llist_item_value(llist.last, int), 1);

  llist_append(&llist, 2);
  ck_assert_int_eq(llist_item_value(llist.first, int), 1);
  ck_assert_int_eq(llist_item_value(llist.last, int), 2);

  llist_append(&llist, 3.14); // Wrong type.
  ck_assert_int_eq(llist_item_value(llist.first, int), 1);
  ck_assert_double_eq(llist_item_value(llist.last, double), 3.14);
}
END_TEST

START_TEST(test_llist_prepend) {
  LinkedList llist;
  llist_init(&llist);

  llist_prepend(&llist, 1);
  ck_assert_int_eq(llist_item_value(llist.first, int), 1);
  ck_assert_int_eq(llist_item_value(llist.last, int), 1);

  llist_prepend(&llist, 2);
  ck_assert_int_eq(llist_item_value(llist.first, int), 2);
  ck_assert_int_eq(llist_item_value(llist.last, int), 1);

  llist_prepend(&llist, 3.14); // Wrong type.
  ck_assert_double_eq(llist_item_value(llist.first, double), 3.14);
  ck_assert_int_eq(llist_item_value(llist.last, int), 1);
}
END_TEST

static Suite *llist_suite(void) {
  Suite *s = suite_create("linked_list");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_llist_append);
  tcase_add_test(tc_core, test_llist_prepend);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = llist_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
