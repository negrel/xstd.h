#include <stdlib.h>

#include <check.h>

#define XSTD_IMPLEMENTATION
#include "alloc.h"

START_TEST(test_libc_malloc) {
  size_t *ptr = alloc_malloc(g_libc_allocator, sizeof(size_t));
  ck_assert(ptr != NULL);

  alloc_free(g_libc_allocator, ptr);
}
END_TEST

START_TEST(test_libc_calloc) {
  size_t *ptr = alloc_calloc(g_libc_allocator, 1, sizeof(size_t));
  ck_assert(ptr != NULL);
  ck_assert_int_eq(0, *ptr);

  alloc_free(g_libc_allocator, ptr);
}
END_TEST

START_TEST(test_libc_realloc) {
  size_t *ptr = alloc_malloc(g_libc_allocator, sizeof(size_t));
  ck_assert(ptr != NULL);

  ptr = alloc_realloc(g_libc_allocator, ptr, 2 * sizeof(size_t));
  ptr[1] = 1;

  alloc_free(g_libc_allocator, ptr);
}
END_TEST

START_TEST(test_libc_free) {
  size_t *ptr = alloc_malloc(g_libc_allocator, sizeof(size_t));
  ck_assert(ptr != NULL);

  alloc_free(g_libc_allocator, ptr);
  alloc_free(g_libc_allocator, NULL);
}
END_TEST

static Suite *alloc_suite(void) {
  Suite *s = suite_create("alloc");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_libc_malloc);
  tcase_add_test(tc_core, test_libc_calloc);
  tcase_add_test(tc_core, test_libc_realloc);
  tcase_add_test(tc_core, test_libc_free);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = alloc_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
