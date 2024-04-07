#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#define XSTD_IMPLEMENTATION
#include "list.h"

typedef_list(int, IntList);

typedef_list_iterator(IntList, IntListIterator);

fndef_list_iterator_init(IntListIterator, IntList, int_list_iter)

    START_TEST(test_list_prepend) {
  // List head.
  IntList *head = NULL;

  IntList *l1 = calloc(1, sizeof(IntList));
  l1->value = 1;
  ck_assert(list_next(l1) == NULL);

  // Prepend l1 to head.
  list_prepend(&head, l1);
  ck_assert(head == l1);
  ck_assert(list_next(l1) == NULL);

  IntList *l2 = calloc(1, sizeof(IntList));
  ck_assert(list_next(l2) == NULL);

  // Prepend l2 to head.
  list_prepend(&head, l2);

  ck_assert(head == l2);
  ck_assert(list_next(l2) == l1);
  ck_assert(list_next(l1) == NULL);

  free(l1);
  free(l2);
}
END_TEST

START_TEST(test_list_remove) {
  // List head.
  IntList *head = NULL;

  IntList *l0 = calloc(1, sizeof(IntList));
  l0->value = 0;
  list_prepend(&head, l0);

  IntList *l1 = calloc(1, sizeof(IntList));
  l1->value = 1;
  list_prepend(&head, l1);

  IntList *l2 = calloc(1, sizeof(IntList));
  list_prepend(&head, l2);

  // Should have no effect.
  list_remove(head, (IntList *)NULL);
  ck_assert(head == l2);
  ck_assert(list_next(l2) == l1);
  ck_assert(list_next(l1) == l0);

  // Replace l2 -> l1 pointer with l2 -> l0.
  list_remove(head, l1);
  ck_assert(head == l2);
  ck_assert(list_next(l2) == l0);
  ck_assert(list_next(l1) == l0);

  free(l0);
  free(l1);
  free(l2);
}
END_TEST

START_TEST(test_list_remove_next) {
  // List head.
  IntList *head = NULL;

  IntList *l1 = calloc(1, sizeof(IntList));
  l1->value = 1;
  list_prepend(&head, l1);

  IntList *l2 = calloc(1, sizeof(IntList));
  list_prepend(&head, l2);

  // Should have no effect.
  list_remove_next(l1);
  ck_assert(head == l2);
  ck_assert(list_next(l2) == l1);
  ck_assert(list_next(l1) == NULL);

  // Remove l2 -> l1 pointer.
  list_remove_next(l2);
  ck_assert(head == l2);
  ck_assert(list_next(l2) == NULL);
  ck_assert(list_next(l1) == NULL);

  free(l1);
  free(l2);
}
END_TEST

START_TEST(test_list_iterator) {
  // List head.
  IntList *head = NULL;

  IntList *l1 = calloc(1, sizeof(IntList));
  l1->value = 1;
  list_prepend(&head, l1);

  IntList *l2 = calloc(1, sizeof(IntList));
  l2->value = 2;
  list_prepend(&head, l2);

  IntListIterator iter = int_list_iter(head);
  size_t i = 0;
  iter_foreach((Iterator *)&iter, IntList *, it) {
    i++;
    switch (it.index) {
    case 0:
      ck_assert_int_eq(it.value->value, 2);
      break;
    case 1:
      ck_assert_int_eq(it.value->value, 1);
      break;
    }
  }
  ck_assert_int_eq(i, 2);

  free(l1);
  free(l2);
}
END_TEST

static Suite *list_suite(void) {
  Suite *s = suite_create("list");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_list_prepend);
  tcase_add_test(tc_core, test_list_remove);
  tcase_add_test(tc_core, test_list_remove_next);
  tcase_add_test(tc_core, test_list_iterator);

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
