#include "result.h"
#include <check.h>
#include <stdlib.h>

START_TEST(test_result_int_str) {
  typedef Result(int, char *) WorkResult;

  // Using variable.
  int ok_value = 42;
  WorkResult r1 = ResultOk(WorkResult, ok_value);
  ck_assert(result_is_ok(r1));
  ck_assert(!result_is_err(r1));
  ck_assert(r1.is_ok);
  ck_assert_int_eq(ok_value, r1.data.ok);

  char *err_value = "something bad happend...";
  WorkResult r2 = ResultError(WorkResult, err_value);
  ck_assert(!result_is_ok(r2));
  ck_assert(result_is_err(r2));
  ck_assert(!r2.is_ok);
  ck_assert_str_eq(err_value, r2.data.err);

  // Using literal.
  WorkResult r3 = ResultOk(WorkResult, 42);
  ck_assert(result_is_ok(r3));
  ck_assert(!result_is_err(r3));
  ck_assert(r3.is_ok);
  ck_assert_int_eq(42, r3.data.ok);

  WorkResult r4 = ResultError(WorkResult, "oops...");
  ck_assert(!result_is_ok(r4));
  ck_assert(result_is_err(r4));
  ck_assert(!r4.is_ok);
  ck_assert_str_eq("oops...", r4.data.err);
}
END_TEST

START_TEST(test_result_struct) {
  typedef struct {
    char *scheme;
    char *host;
    int port;
    char *path;
  } Url;
  typedef struct {
    int position;
    char *msg;
  } UrlParseError;

  typedef Result(Url, UrlParseError) WorkResult;

  // Using variable.
  Url ok_value = {0};
  WorkResult r1 = ResultOk(WorkResult, ok_value);
  ck_assert(result_is_ok(r1));
  ck_assert(!result_is_err(r1));
  ck_assert(r1.is_ok);
  ck_assert_mem_eq(&ok_value, &r1.data.err, sizeof(Url));

  UrlParseError err_value = {0};
  WorkResult r2 = ResultError(WorkResult, err_value);
  ck_assert(!result_is_ok(r2));
  ck_assert(result_is_err(r2));
  ck_assert(!r2.is_ok);
  ck_assert_mem_eq(&err_value, &r2.data.err, sizeof(UrlParseError));

  // Using literal.
  WorkResult r3 = ResultOk(WorkResult, {0});
  ck_assert(result_is_ok(r3));
  ck_assert(!result_is_err(r3));
  ck_assert(r3.is_ok);
  ck_assert_mem_eq(&ok_value, &r3.data.err, sizeof(Url));

  WorkResult r4 = ResultError(WorkResult, {0});
  ck_assert(!result_is_ok(r4));
  ck_assert(result_is_err(r4));
  ck_assert(!r4.is_ok);
  ck_assert_mem_eq(&err_value, &r4.data.err, sizeof(UrlParseError));
}
END_TEST

static Suite *result_suite(void) {
  Suite *s = suite_create("result");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_result_int_str);
  tcase_add_test(tc_core, test_result_struct);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  Suite *suite = result_suite();
  SRunner *sr = srunner_create(suite);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
