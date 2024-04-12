#ifndef XSTD_ITER_H_INCLUDE
#define XSTD_ITER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "constructor.h"
#include "iface.h"

// Iterator interface.
iface_def(
    Iterator, struct xstd_iterator_vtable { void *(*next)(InterfaceImpl); });

#define iter_foreach(iter, type, iterator)                                     \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        type value;                                                            \
      } iterator =                                                             \
          {                                                                    \
              .index = 0,                                                      \
             .value = iter_next(iter),                                         \
          };                                                                   \
      iterator.value != NULL;                                                  \
      iterator.index++, iterator.value = iter_next(iter))

void *iter_next(Iterator *);

#ifdef XSTD_IMPLEMENTATION
void *iter_next(Iterator *iter) { return iface_call_empty(iter, next); }
#endif

#define range_foreach(iterator, from, to, step)                                \
  for (struct {                                                                \
         short i;                                                              \
         RangeIterator iter;                                                   \
       } tmp = {.i = 0, .iter = range_iterator(from, to, step)};               \
       tmp.i == 0; tmp.i = 1)                                                  \
  iter_foreach((Iterator *)&tmp.iter, size_t *, iterator)

#define range_to_foreach(i, to) range_foreach(i, 0, to, 1)
#define range_from_to_foreach(i, from, to) range_foreach(i, from, to, 1)

iface_impl_def(
    Iterator, RangeIterator, struct {
      int64_t value;
      int64_t end;
      int64_t step;
    });

#ifdef XSTD_IMPLEMENTATION
static void *range_iterator_next(InterfaceImpl impl) {
  typeof_iface_impl(RangeIterator) *b =
      cast_iface_impl_ptr(RangeIterator, impl);

  b->value += b->step;
  if (b->value >= b->end) {
    b->value -= b->step;
    return NULL;
  }

  return &b->value;
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
                        .value = start - step,                                 \
                    }))                                                        \
  }

def_type_constructors(RangeIterator, range_iterator)

#endif
