#include <stdlib.h>

#include <check.h>

#define XSTD_ALLOC_IMPLEMENTATION
#include "xstd_alloc.h"

#define XSTD_ITER_IMPLEMENTATION
#include "xstd_iter.h"

#define XSTD_VEC_IMPLEMENTATION
#include "xstd_vec.h"

START_TEST(test_vec_new) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  int *vec = vec_new(&alloc, 10, sizeof(int));

  ck_assert_int_eq(vec_cap(vec), 10);
  ck_assert_int_eq(vec_len(vec), 0);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_push) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  const size_t initial_cap = 4;

  int *vec = vec_new(&alloc, initial_cap, sizeof(int));
  vec_push(&vec); // Element should be zeroed
  ck_assert_int_eq(vec_len(vec), 1);

  *vec_push(&vec) = 1;
  ck_assert_int_eq(vec_len(vec), 2);

  *vec_push(&vec) = 2;
  ck_assert_int_eq(vec_len(vec), 3);

  *vec_push(&vec) = 3;
  ck_assert_int_eq(vec_len(vec), 4);

  ck_assert_int_eq(vec[0], 0);
  ck_assert_int_eq(vec[1], 1);
  ck_assert_int_eq(vec[2], 2);
  ck_assert_int_eq(vec[3], 3);

  vec[0] = -1;
  ck_assert_int_eq(vec[0], -1);

  // let's push again so our vector grows
  *vec_push(&vec) = 4;
  ck_assert_int_eq(vec_len(vec), 5);

  ck_assert_int_eq(vec[0], -1);
  ck_assert_int_eq(vec[1], 1);
  ck_assert_int_eq(vec[2], 2);
  ck_assert_int_eq(vec[3], 3);
  ck_assert_int_eq(vec[4], 4);

  ck_assert_int_gt(vec_cap(vec), initial_cap);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_pop) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  const size_t initial_cap = 4;

  int *vec = vec_new(&alloc, initial_cap, sizeof(int));
  vec_push(&vec); // Element should be zeroed
  *vec_push(&vec) = 1;
  *vec_push(&vec) = 2;
  *vec_push(&vec) = 3;
  ck_assert_int_eq(vec_len(vec), 4);

  int popped = 0;
  vec_pop(vec, &popped);
  ck_assert_int_eq(popped, 3);
  ck_assert_int_eq(vec_len(vec), 3);

  // Let's skip that one, shouldn't segfault
  vec_pop(vec, NULL);
  ck_assert_int_eq(vec_len(vec), 2);

  vec_pop(vec, &popped);
  ck_assert_int_eq(popped, 1);
  ck_assert_int_eq(vec_len(vec), 1);

  vec_pop(vec, &popped);
  ck_assert_int_eq(popped, 0);
  ck_assert_int_eq(vec_len(vec), 0);

  // Pop with a length of zero should'nt change the variable
  popped = 99;
  vec_pop(vec, &popped);
  ck_assert_int_eq(popped, 99);

  // Lendth should remain 0
  ck_assert_int_eq(vec_len(vec), 0);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_unshift) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  const size_t initial_cap = 4;

  int *vec = vec_new(&alloc, initial_cap, sizeof(int));
  vec_unshift(&vec); // Element should be zeroed
  ck_assert_int_eq(vec_len(vec), 1);

  *vec_unshift(&vec) = 1;
  ck_assert_int_eq(vec_len(vec), 2);

  *vec_unshift(&vec) = 2;
  ck_assert_int_eq(vec_len(vec), 3);

  *vec_unshift(&vec) = 3;
  ck_assert_int_eq(vec_len(vec), 4);

  ck_assert_int_eq(vec[0], 3);
  ck_assert_int_eq(vec[1], 2);
  ck_assert_int_eq(vec[2], 1);
  ck_assert_int_eq(vec[3], 0);

  vec[0] = -1;
  ck_assert_int_eq(vec[0], -1);

  // let's push again so our vector grows
  *vec_unshift(&vec) = 4;
  ck_assert_int_eq(vec_len(vec), 5);

  ck_assert_int_eq(vec[0], 4);
  ck_assert_int_eq(vec[1], -1);
  ck_assert_int_eq(vec[2], 2);
  ck_assert_int_eq(vec[3], 1);
  ck_assert_int_eq(vec[4], 0);

  ck_assert_int_gt(vec_cap(vec), initial_cap);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_shift) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  const size_t initial_cap = 4;

  int *vec = vec_new(&alloc, initial_cap, sizeof(int));
  vec_push(&vec); // Element should be zeroed
  *vec_push(&vec) = 1;
  *vec_push(&vec) = 2;
  *vec_push(&vec) = 3;
  ck_assert_int_eq(vec_len(vec), 4);

  int shifted = 0;
  vec_shift(vec, &shifted);
  ck_assert_int_eq(shifted, 0);
  ck_assert_int_eq(vec_len(vec), 3);

  // Let's skip that one, shouldn't segfault
  vec_shift(vec, NULL);
  ck_assert_int_eq(vec_len(vec), 2);

  vec_shift(vec, &shifted);
  ck_assert_int_eq(shifted, 2);
  ck_assert_int_eq(vec_len(vec), 1);

  vec_shift(vec, &shifted);
  ck_assert_int_eq(shifted, 3);
  ck_assert_int_eq(vec_len(vec), 0);

  // Pop with a length of zero should'nt change the variable
  shifted = 99;
  vec_shift(vec, &shifted);
  ck_assert_int_eq(shifted, 99);

  // Lendth should remain 0
  ck_assert_int_eq(vec_len(vec), 0);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_clone) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  int *vec = vec_new(&alloc, 10, sizeof(int));
  *vec_push(&vec) = 5;
  *vec_push(&vec) = 6;

  int *clone = vec_clone(vec);
  ck_assert_int_eq(clone[0], 5);
  ck_assert_int_eq(clone[1], 6);

  vec_pop(clone, NULL);
  ck_assert_int_eq(vec_len(clone), 1);

  ck_assert_int_eq(vec_len(vec), 2);
  ck_assert_int_eq(vec[1], 6);

  vec_free(clone);
  vec_free(vec);
}
END_TEST

START_TEST(test_vec_foreach) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  int *vec = vec_new(&alloc, 10, sizeof(int));
  *vec_push(&vec) = 5;
  *vec_push(&vec) = 6;
  *vec_push(&vec) = 8;

  size_t i = 0;
  vec_foreach(vec, it) {
    switch (it.index) {
    case 0:
      ck_assert_int_eq(5, it.value);
      break;

    case 1:
      ck_assert_int_eq(6, it.value);
      break;

    default:
      continue;
    }
    i++;
  }

  ck_assert_uint_eq(2, i);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_foreach_ptr) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  int *vec = vec_new(&alloc, 10, sizeof(int));
  *vec_push(&vec) = 5;
  *vec_push(&vec) = 6;
  *vec_push(&vec) = 8;

  size_t i = 0;
  vec_foreach_ptr(vec, it) {
    switch (it.index) {
    case 0:
      ck_assert_int_eq(5, *it.value);
      break;

    case 1:
      ck_assert_int_eq(6, *it.value);
      break;

    default:
      continue;
    }
    i++;
  }

  ck_assert_uint_eq(2, i);

  vec_free(vec);
}
END_TEST

START_TEST(test_vec_iter) {
  Allocator alloc = {0};
  libc_alloc_init(&alloc);

  int *vec = vec_new(&alloc, 10, sizeof(int));
  *vec_push(&vec) = 5;
  *vec_push(&vec) = 6;
  *vec_push(&vec) = 8;

  VecIterator vec_iterator = vec_iter(vec);
  Iterator *iter = (Iterator *)&vec_iterator;
  ck_assert_int_eq(5, *(int *)iter_next(iter));
  ck_assert_int_eq(6, *(int *)iter_next(iter));
  ck_assert_int_eq(8, *(int *)iter_next(iter));
  ck_assert(NULL == iter_next(iter));
  ck_assert(NULL == iter_next(iter));

  size_t i = 0;

  // foreach macro.
  vec_iter_foreach(vec, it) {
    switch (it.index) {
    case 0:
      ck_assert_int_eq(5, *it.value);
      break;

    case 1:
      ck_assert_int_eq(6, *it.value);
      break;

    case 2:
      ck_assert_int_eq(8, *it.value);
      break;
    }
    ck_assert(it.index == i);

    i++;
  }

  ck_assert_uint_eq(3, i);

  vec_free(vec);
}
END_TEST

static Suite *vec_suite(void) {
  Suite *s = suite_create("vector");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_vec_new);
  tcase_add_test(tc_core, test_vec_push);
  tcase_add_test(tc_core, test_vec_pop);
  tcase_add_test(tc_core, test_vec_unshift);
  tcase_add_test(tc_core, test_vec_shift);
  tcase_add_test(tc_core, test_vec_clone);
  tcase_add_test(tc_core, test_vec_foreach);
  tcase_add_test(tc_core, test_vec_foreach_ptr);
  tcase_add_test(tc_core, test_vec_iter);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = vec_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
