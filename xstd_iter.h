#ifndef XSTD_ITER_H
#define XSTD_ITER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct xstd_iterator {
  void *(*next)(struct xstd_iterator *);
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

#ifdef XSTD_ITER_IMPLEMENTATION
void *iter_next(Iterator *iter) { return iter->next(iter); }
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

typedef struct xstd_range_iter {
  Iterator iterator;
  int64_t value, start, end, step;
} RangeIterator;

void *range_iter_next(Iterator *it);
#ifdef XSTD_ITER_IMPLEMENTATION
void *range_iter_next(Iterator *it) {
  RangeIterator *iter = (RangeIterator *)it;

  iter->value += iter->step;
  if (iter->value >= iter->end) {
    iter->value -= iter->step;
    return NULL;
  }

  return &iter->value;
}
#endif

RangeIterator range_iter(int64_t start, int64_t end, int64_t step);

#ifdef XSTD_ITER_IMPLEMENTATION
RangeIterator range_iter(int64_t start, int64_t end, int64_t step) {
  RangeIterator iter = {0};
  iter.iterator.next = &range_iter_next;
  iter.value = start - step;
  iter.end = end;
  iter.step = step;

  return iter;
}
#endif

#ifdef XSTD_ITER_VEC_IMPLEMENTATION
#include <stdint.h>

#include "xstd_vec.h"

#define vec_iter_foreach(vec, iterator)                                        \
  for (struct {                                                                \
         short i;                                                              \
         VecIterator iter;                                                     \
       } tmp = {.i = 0, .iter = vec_iter(vec)};                                \
       tmp.i == 0; tmp.i = 1)                                                  \
  iter_foreach((Iterator *)&tmp.iter, typeof(vec), iterator)

typedef struct {
  Iterator iterator;
  size_t cursor;
  Vec vec;
} VecIterator;

void *vec_iter_next(Iterator *it);

#ifdef XSTD_ITER_IMPLEMENTATION
void *vec_iter_next(Iterator *it) {
  if (it == NULL)
    return NULL;

  VecIterator *iter = (VecIterator *)it;
  void *result = NULL;

  struct xstd_vector *vec = headerof_vec(iter->vec);
  if (iter->cursor < vec->len) {
    result = (void *)((uintptr_t)(iter->vec) + iter->cursor * vec->elem_size);
    iter->cursor++;
  }

  return result;
}
#endif

VecIterator vec_iter(const Vec vec);

#ifdef XSTD_ITER_IMPLEMENTATION
VecIterator vec_iter(const Vec vec) {
  VecIterator iterator = {0};
  iterator.iterator.next = &vec_iter_next;
  iterator.cursor = 0;
  iterator.vec = vec;

  return iterator;
}
#endif

#endif

#endif
