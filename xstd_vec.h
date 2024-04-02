// Single file header vector (growable array)
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_VEC_H_INCLUDE
#define XSTD_VEC_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>

#define XSTD_ITER_VEC_IMPLEMENTATION
#ifdef XSTD_VEC_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define XSTD_ALLOC_IMPLEMENTATION
#endif

#ifdef XSTD_VEC_ALLOCATOR
#include "xstd_alloc.h"
#endif

typedef void *Vec;

struct xstd_vector {
  // Header
#ifdef XSTD_VEC_ALLOCATOR
  Allocator *allocator;
#endif
  size_t cap;
  size_t len;
  size_t elem_size;

  // Body
  // ...
};

#ifdef __clang__
#define typeof __typeof__
#else
#ifndef typeof
#define typeof assert(0 && "typeof macro is not defined")
#endif
#endif

// vec_foreach is a foreach macro for the vector type.
// The first parameter is the vector on which to iterate. The
// second is the name of the iterator. The index is accessible
// with `iterator.index` and the value with `iterator.value`.
#define vec_foreach(vec, iterator)                                             \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(*vec) value;                                                   \
       } iterator = {.value = vec[0]};                                         \
       iterator.index < vec_len(vec); iterator.value = vec[++iterator.index])

// vec_foreach_ptr does the same as vec_foreach
// but the iterator contains a ptr instead of a copy.
#define vec_foreach_ptr(vec, iterator)                                         \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(vec) value;                                                    \
       } iterator = {.value = &vec[0]};                                        \
       iterator.index < vec_len(vec); iterator.value = &vec[++iterator.index])

// vec_push returns a pointer to an element at the end of the vector.
// This function takes a double pointer to a vector to reallocate in case it's
// full.
#define vec_push(vec) ((typeof(*vec))_vec_push((Vec *)vec))

// vec_unshift adds an element at the beginning of the vector and shift
// other elements.
#define vec_unshift(vec) ((typeof(*vec))_vec_unshift((void **)vec))

#define bodyof_vec(vecptr) ((uintptr_t)(vecptr) + sizeof(struct xstd_vector))
#define headerof_vec(vecptr)                                                   \
  ((struct xstd_vector *)((uintptr_t)vecptr - sizeof(struct xstd_vector)))

#ifdef XSTD_VEC_IMPLEMENTATION
static size_t sizeof_vector(struct xstd_vector *v) {
  return sizeof(struct xstd_vector) + v->cap * v->elem_size;
}
#endif

// vec_new returns a new zeroed vector with the given capacity to store
// element of the given size. It the returns a pointer to the first element of
// the vector.
#ifdef XSTD_VEC_ALLOCATOR
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size);
#else
Vec vec_new(size_t cap, size_t elem_size);
#endif

#ifdef XSTD_VEC_IMPLEMENTATION
#ifdef XSTD_VEC_ALLOCATOR
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size) {
#else
Vec vec_new(size_t cap, size_t elem_size) {
#endif
  size_t header = sizeof(struct xstd_vector);
  size_t body = cap * elem_size;

#ifdef XSTD_VEC_ALLOCATOR
  struct xstd_vector *vec = alloc_calloc(allocator, header + body, 1);
#else
  struct xstd_vector *vec = calloc(header + body, 1);
#endif
  if (vec == NULL)
    return NULL;

#ifdef XSTD_VEC_ALLOCATOR
  vec->allocator = allocator;
#endif

  vec->len = 0;
  vec->cap = cap;
  vec->elem_size = elem_size;

  return (void *)bodyof_vec(vec);
}
#endif

// vec_clone creates a clone of the given vector and returns it.
Vec vec_clone(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
Vec vec_clone(const Vec vec) {
  assert(vec != NULL);

  struct xstd_vector *v = headerof_vec(vec);
  size_t size_v = sizeof_vector(v);

#ifdef XSTD_VEC_ALLOCATOR
  void *clone = alloc_malloc(v->allocator, size_v);
#else
  void *clone = malloc(size_v);
#endif
  if (clone == NULL)
    return NULL;

  memcpy(clone, v, size_v);

  return (void *)bodyof_vec(clone);
}
#endif

// vec_len returns the current len of the vector.
size_t vec_len(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
size_t vec_len(const Vec vec) {
  assert(vec != NULL);

  return headerof_vec(vec)->len;
}
#endif

// vec_cap returns the current capacity of the vector.
size_t vec_cap(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
size_t vec_cap(const Vec vec) {
  assert(vec != NULL);

  return headerof_vec(vec)->cap;
}
#endif

// vec_isfull returns true if the vector is full.
bool vec_isfull(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
bool vec_isfull(const Vec vec) {
  assert(vec != NULL);

  return vec_len(vec) == vec_cap(vec);
}
#endif

// vec_isempty returns true if the vector size is 0.
bool vec_isempty(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
bool vec_isempty(const Vec vec) {
  assert(vec != NULL);

  return vec_len(vec) == 0;
}
#endif

// vec_push_ returns a pointer to an element at the end of the vector.
// This function takes a pointer to a vector to reallocate it if it's full.
// An handy vec_push macro exists so you don't have to cast arguments and
// return type.
void *_vec_push(Vec *);

#ifdef XSTD_VEC_IMPLEMENTATION
void *_vec_push(void **vec) {
  assert(vec != NULL);
  assert(*vec != NULL);

  if (vec_isfull(*vec)) {
    // Let's double the capacity of our vector
    struct xstd_vector *old = headerof_vec(*vec);

#ifdef XSTD_VEC_ALLOCATOR
    void *new = vec_new(old->allocator, old->cap * 2, old->elem_size);
#else
    void *new = vec_new(old->cap * 2, old->elem_size);
#endif
    headerof_vec(new)->len = old->len;

    memcpy(new, (void *)bodyof_vec(old), old->cap * old->elem_size);
#ifdef XSTD_VEC_ALLOCATOR
    alloc_free(old->allocator, old);
#else
    free(old);
#endif

    *vec = new;
  }

  struct xstd_vector *v = headerof_vec(*vec);
  size_t offset = v->elem_size * v->len;

  v->len++;

  return (void *)(bodyof_vec(v) + offset);
}
#endif

// vec_pop removes the last element of the vector and store it in popped
// if not null.
void vec_pop(Vec, void *);

#ifdef XSTD_VEC_IMPLEMENTATION

void vec_pop(Vec vec, void *popped) {
  assert(vec != NULL);

  if (vec_isempty(vec))
    return;

  struct xstd_vector *v = headerof_vec(vec);
  v->len--;

  if (popped != NULL)
    memcpy(popped, (void *)((uintptr_t)vec + v->len * v->elem_size),
           v->elem_size);
}
#endif

// vec_shift removes the first element of the vector and store it shifted
// if not null. Remaining elements are copied to their index - 1.
void vec_shift(Vec, void *);

#ifdef XSTD_VEC_IMPLEMENTATION
void vec_shift(Vec vec, void *shifted) {
  assert(vec != NULL);

  if (vec_isempty(vec))
    return;

  struct xstd_vector *v = headerof_vec(vec);
  if (shifted != NULL)
    memcpy(shifted, vec, v->elem_size);

  v->len--;

  memmove((void *)bodyof_vec(v),
          (void *)((uintptr_t)bodyof_vec(v) + v->elem_size),
          v->len * v->elem_size);
}
#endif

// vec_unshift_ adds an element at the beginning of the vector and shift
// other elements.
// An handy vec_unshift macro exists so you don't have to cast arguments and
// return type.
void *_vec_unshift(Vec *);

#ifdef XSTD_VEC_IMPLEMENTATION

void *_vec_unshift(void **vec) {
  assert(vec != NULL);
  assert(*vec != NULL);

  if (vec_isfull(*vec)) {
    // Let's double the capacity of our vector
    struct xstd_vector *old = headerof_vec(*vec);

#ifdef XSTD_VEC_ALLOCATOR
    Vec new = vec_new(old->allocator, old->cap * 2, old->elem_size);
#else
    Vec new = vec_new(old->cap * 2, old->elem_size);
#endif
    headerof_vec(new)->len = old->len + 1;

    // Copy old elements in new at index 1
    memcpy((void *)((uintptr_t) new + old->elem_size), (void *)bodyof_vec(old),
           old->cap * old->elem_size);
#ifdef XSTD_VEC_ALLOCATOR
    alloc_free(old->allocator, old);
#else
    free(old);
#endif

    *vec = new;

    return new;
  }

  struct xstd_vector *v = headerof_vec(*vec);
  // Move all elements by 1
  memmove((void *)((uintptr_t)*vec + v->elem_size), *vec,
          v->len * v->elem_size);
  v->len++;

  return *vec;
}
#endif

// vec_free free the given vector.
void vec_free(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
void vec_free(Vec vec) {
  assert(vec != NULL);

  struct xstd_vector *v = headerof_vec(vec);
  free(v);
}
#endif

#endif
