#ifndef XSTD_ITER_H_INCLUDE
#define XSTD_ITER_H_INCLUDE

#include <stdbool.h>
#include <stdint.h>

#include "constructor.h"
#include "iface.h"

// Iterator interface.
iface_def(
    Iterator,
    struct xstd_iterator_vtable { bool (*next)(InterfaceImpl, void *); });

bool iter_next(Iterator *, void *);

#ifdef XSTD_IMPLEMENTATION
bool iter_next(Iterator *iter, void *n) { return iface_call(iter, next, n); }
#endif

iface_impl_def(
    Iterator, RangeIterator, struct {
      intmax_t value;
      intmax_t end;
      intmax_t step;
    });

#ifdef XSTD_IMPLEMENTATION
static bool range_iterator_next(InterfaceImpl impl, void *next) {
  typeof_iface_impl(RangeIterator) *b =
      cast_iface_impl_ptr(RangeIterator, impl);

  *(intmax_t *)next = b->value;

  b->value += b->step;

  return !((b->step > 0 && b->value >= b->end) ||
           (b->step < 0 && b->value <= b->end));
}
#endif

struct xstd_iterator_vtable range_iterator_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_iterator_vtable range_iterator_vtable = {
    .next = &range_iterator_next,
};
#endif

#undef type_init_proto
#undef type_init_args
#undef type_init
#define type_init_proto int64_t start, int64_t end, int64_t step
#define type_init_args start, end, step
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface, &range_iterator_vtable,                          \
                    ((typeof_iface_impl(RangeIterator)){                       \
                        .end = end,                                            \
                        .step = step,                                          \
                        .value = start,                                        \
                    }))                                                        \
  }

def_type_constructors(RangeIterator, range_iterator)

#endif
