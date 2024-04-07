// Single file header iterator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ITER_H_INCLUDE
#define XSTD_ITER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "alloc.h"

struct xstd_iterator_vtable {
  void *(*next)(void *);
};

// Iterator interface.
typedef struct xstd_iterator {
  struct xstd_iterator_vtable *vtable_;
  size_t offset_;
} Iterator;

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
void *iter_next(Iterator *iter) {
  return iter->vtable_->next((void *)((uintptr_t)iter + iter->offset_));
}
#endif

#define range_foreach(iterator, from, to, step)                                \
  for (struct {                                                                \
         short i;                                                              \
         RangeIterator iter;                                                   \
       } tmp = {.i = 0, .iter = range_iter(from, to, step)};                   \
       tmp.i == 0; tmp.i = 1)                                                  \
  iter_foreach((Iterator *)&tmp.iter, size_t *, iterator)

#define range_to_foreach(i, to) range_foreach(i, 0, to, 1)
#define range_from_to_foreach(i, from, to) range_foreach(i, from, to, 1)

struct xstd_range_iter_body {
  int64_t value, end, step;
};

typedef struct xstd_range_iter {
  Iterator iterator;
  struct xstd_range_iter_body body_;
} RangeIterator;

#ifdef XSTD_IMPLEMENTATION
static void *range_iterator_next(void *body) {
  struct xstd_range_iter_body *b = (struct xstd_range_iter_body *)body;

  b->value += b->step;
  if (b->value >= b->end) {
    b->value -= b->step;
    return NULL;
  }

  return &b->value;
}
#endif

struct xstd_iterator_vtable xstd_range_iterator_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_iterator_vtable xstd_range_iterator_vtable = {
    .next = &range_iterator_next,
};
#endif

void range_iterator_init(RangeIterator *range_iterator, int64_t start,
                         int64_t end, int64_t step);

#ifdef XSTD_IMPLEMENTATION
void range_iterator_init(RangeIterator *range_iterator, int64_t start,
                         int64_t end, int64_t step) {
  *range_iterator = (RangeIterator){0};
  range_iterator->iterator.vtable_ = &xstd_range_iterator_vtable;
  range_iterator->iterator.offset_ = offsetof(RangeIterator, body_);
  range_iterator->body_.end = end;
  range_iterator->body_.step = step;
  range_iterator->body_.value = start - step;
}
#endif

RangeIterator *range_iterator_new(Allocator *allocator, int64_t start,
                                  int64_t end, int64_t step);

#ifdef XSTD_IMPLEMENTATION
RangeIterator *range_iterator_new(Allocator *allocator, int64_t start,
                                  int64_t end, int64_t step) {
  RangeIterator *ri = alloc_malloc(allocator, sizeof(RangeIterator));
  range_iterator_init(ri, start, end, step);
  return ri;
}
#endif

RangeIterator range_iter(int64_t start, int64_t end, int64_t step);

#ifdef XSTD_IMPLEMENTATION
RangeIterator range_iter(int64_t start, int64_t end, int64_t step) {
  RangeIterator ri = {0};
  range_iterator_init(&ri, start, end, step);
  return ri;
}
#endif

#endif
