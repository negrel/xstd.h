#ifndef XSTD_RESULT_H_INCLUDE
#define XSTD_RESULT_H_INCLUDE

#include <stdbool.h>

#define Result(OkType, ErrType)                                                \
  struct {                                                                     \
    bool is_ok;                                                                \
    union {                                                                    \
      ErrType err;                                                             \
      OkType ok;                                                               \
    } data;                                                                    \
  }

#define result_is_ok(res) (res).is_ok

#define result_is_err(res) !(res).is_ok

#define ResultError(ResultType, err_value)                                     \
  (ResultType) {                                                               \
    .is_ok = false, .data = {                                                  \
      .err = err_value,                                                        \
    }                                                                          \
  }

#define ResultOk(ResultType, ok_value)                                         \
  (ResultType) {                                                               \
    .is_ok = true, .data = {                                                   \
      .ok = ok_value,                                                          \
    }                                                                          \
  }

#define ResultVoid(ErrType)                                                    \
  struct {                                                                     \
    bool is_ok;                                                                \
    union {                                                                    \
      ErrType err;                                                             \
    } data;                                                                    \
  }

#define ResultOkVoid(ResultType)                                               \
  (ResultType) { .is_ok = true }

#endif
