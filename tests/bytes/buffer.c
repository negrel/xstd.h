#include <check.h>
#include <stdlib.h>

#define XSTD_IMPLEMENTATION
#include "alloc.h"
#include "bytes/buffer.h"

START_TEST(test_bytes_buffer_get_set_append) {
  BytesBuffer buf = bytes_buffer(g_libc_allocator);

  size_t appended = bytes_buffer_append(&buf, "Hello world!", 13, 1);
  ck_assert_int_eq(appended, 13);
  ck_assert_str_eq("Hello world!", (const char *)bytes_buffer_bytes(&buf).data);

  // Erase null terminator.
  bytes_buffer_set(&buf, bytes_buffer_length(&buf) - 1, (char)' ');

  // Append another string.
  appended = bytes_buffer_append(&buf, "Foo bar...", 11, 1);
  ck_assert_int_eq(appended, 11);
  ck_assert_str_eq("Hello world! Foo bar...",
                   (const char *)bytes_buffer_bytes(&buf).data);

  ck_assert_int_eq(bytes_buffer_get(&buf, 0, char), (char)'H');
  ck_assert_int_eq(bytes_buffer_get(&buf, 1, char), (char)'e');

  bytes_buffer_deinit(&buf);
}
END_TEST

static Suite *bytes_buffer_suite(void) {
  Suite *s = suite_create("bytes_buffer");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_bytes_buffer_get_set_append);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = bytes_buffer_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
