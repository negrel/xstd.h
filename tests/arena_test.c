#include <stdlib.h>

#include <check.h>

#define XSTD_ARENA_IMPLEMENTATION
#include "xstd_arena.h"

#include "xstd_alloc.h"

START_TEST(test_arena_malloc) {
  ArenaAllocator arena = {0};
  arena_allocator_init(&arena, g_libc_allocator, 2 * sizeof(void *));
  ck_assert(arena.body_.arena_list_ == NULL);

  Allocator *alloc = (Allocator *)&arena;

  // Allocated data is bigger than arena, allocates a dedicated arena.
  size_t *ptr = alloc_malloc(alloc, 3 * sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  *ptr = 1;

  // Allocate data that fits within a single arena.
  ptr = alloc_malloc(alloc, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 2;

  // Still fits within same arena.
  ptr = alloc_malloc(alloc, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 3;

  // Now overflow current arena, a new one is allocated.
  ptr = alloc_malloc(alloc, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next == NULL);
  *ptr = 4;

  // Allocated data is bigger than arena.
  ptr = alloc_malloc(alloc, 3 * sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next->next == NULL);
  *ptr = 5;

  arena_alloc_reset(&arena);
}
END_TEST

START_TEST(test_arena_calloc) {
  ArenaAllocator arena = {0};
  arena_allocator_init(&arena, g_libc_allocator, 2 * sizeof(void *));
  ck_assert(arena.body_.arena_list_ == NULL);

  Allocator *alloc = (Allocator *)&arena;

  // Allocated data is bigger than arena, allocates a dedicated arena.
  size_t *ptr = alloc_calloc(alloc, 3, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  *ptr = 1;

  // Allocate data that fits within a single arena.
  ptr = alloc_calloc(alloc, 1, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 2;

  // Still fits within same arena.
  ptr = alloc_calloc(alloc, 1, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 3;

  // Now overflow current arena, a new one is allocated.
  ptr = alloc_calloc(alloc, 1, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next == NULL);
  *ptr = 4;

  // Allocated data is bigger than arena.
  ptr = alloc_calloc(alloc, 3, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next->next == NULL);
  *ptr = 5;

  arena_alloc_reset(&arena);
}
END_TEST

START_TEST(test_arena_realloc) {
  ArenaAllocator arena = {0};
  arena_allocator_init(&arena, g_libc_allocator, 2 * sizeof(void *));
  ck_assert(arena.body_.arena_list_ == NULL);

  Allocator *alloc = (Allocator *)&arena;

  // Allocated data is bigger than arena, allocates a dedicated arena.
  size_t *ptr = alloc_realloc(alloc, NULL, 3 * sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  *ptr = 1;

  // Allocate data that fits within a single arena.
  ptr = alloc_realloc(alloc, ptr, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 2;

  // Still fits within same arena.
  ptr = alloc_realloc(alloc, ptr, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next == NULL);
  *ptr = 3;

  // Now overflow current arena, a new one is allocated.
  ptr = alloc_realloc(alloc, ptr, sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next == NULL);
  *ptr = 4;

  // Allocated data is bigger than arena.
  ptr = alloc_realloc(alloc, ptr, 3 * sizeof(void *));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  ck_assert(arena.body_.arena_list_->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next != NULL);
  ck_assert(arena.body_.arena_list_->next->next->next->next == NULL);
  *ptr = 5;

  arena_alloc_reset(&arena);
}
END_TEST

START_TEST(test_arena_free) {
  ArenaAllocator arena = {0};
  arena_allocator_init(&arena, g_libc_allocator, 2 * sizeof(void *));
  ck_assert(arena.body_.arena_list_ == NULL);

  Allocator *alloc = (Allocator *)&arena;

  size_t *ptr = alloc_malloc(alloc, sizeof(size_t));
  ck_assert(ptr != NULL);
  ck_assert(arena.body_.arena_list_ != NULL);
  *ptr = 1;
  size_t ptr_val = *ptr;

  // Noop.
  alloc_free(alloc, ptr);

  ck_assert_int_eq(*ptr, ptr_val);

  arena_alloc_reset(&arena);
}
END_TEST

static Suite *alloc_suite(void) {
  Suite *s = suite_create("arena");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_arena_malloc);
  tcase_add_test(tc_core, test_arena_calloc);
  tcase_add_test(tc_core, test_arena_realloc);
  tcase_add_test(tc_core, test_arena_free);
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
