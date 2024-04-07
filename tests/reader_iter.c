// COMMENT
// ANOTHER COMMENT

#include <check.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#include "iter.h"
#include "reader_iter.h"

START_TEST(test_reader_iter) {
  FILE *f = fopen(__FILE__, "r");
  FileReader freader = file_reader(f);

  uint8_t buf[4] = {0};

  ReaderIterator reader_iter = {0};
  reader_iterator_init(&reader_iter, &freader.reader, &buf[0], 4);

  char collected[64] = {0};
  iter_foreach(&reader_iter.iterator, char *, it) {
    collected[it.index] = *it.value;
    if (it.index > 32)
      break;
  }

  ck_assert(strncmp(collected, "// COMMENT\n// ANOTHER COMMENT\n", 30) == 0);

  fclose(f);
}
END_TEST

static Suite *iter_suite(void) {
  Suite *s = suite_create("iter");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_reader_iter);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = iter_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
