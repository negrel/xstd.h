#ifndef XSTD_OPTION_H_INCLUDE
#define XSTD_OPTION_H_INCLUDE

#include <stdbool.h>

#define Option(type)                                                           \
  typedef union {                                                              \
    bool is_some;                                                              \
    type some;                                                                 \
  }

#define option_is_some(opt) opt.is_some

#define option_is_none(opt) !opt.is_some

#define OptionSome(OptionType, some)                                           \
  (OptionType) { .is_some = true, .some = some }

#define OptionNone(OptionType)                                                 \
  (OptionType) { .is_some = false }

#endif
