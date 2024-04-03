// XSTD bundle from https://github.com/negrel/xstd.h

#ifdef __clang__
#define typeof __typeof__
#else
#ifndef typeof
#define typeof assert(0 && "typeof macro is not defined")
#endif
#endif
// Single file header allocator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ALLOC_H_INCLUDE
#define XSTD_ALLOC_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_ALLOC_IMPLEMENTATION
#include <assert.h>
#include <stdlib.h>
#endif

// Allocator interface / virtual table.
typedef struct xstd_allocator {
  void *(*malloc)(struct xstd_allocator *allocator, size_t size);
  void (*free)(struct xstd_allocator *allocator, void *ptr);
  void *(*calloc)(struct xstd_allocator *allocator, size_t nmemb, size_t size);
  void *(*realloc)(struct xstd_allocator *allocator, void *ptr, size_t newsize);
} Allocator;

// alloc_malloc() allocates size bytes and returns a pointer to the allocated
// memory. The memory is not initialized.
void *alloc_malloc(Allocator *, size_t);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_malloc(Allocator *allocator, size_t size) {
  void *ptr = allocator->malloc(allocator, size);
  assert(ptr != NULL);
  return ptr;
}
#endif

// alloc_free() frees the memory space pointed to by ptr, which must have been
// returned by a previous call to malloc() or related functions.
void alloc_free(Allocator *allocator, void *ptr);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void alloc_free(Allocator *allocator, void *ptr) {
  allocator->free(allocator, ptr);
}
#endif

// The calloc() function allocates memory for an array of nmemb elements of size
// bytes each and returns a pointer to the allocated memory. The memory is set
// to zero.
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size) {
  void *ptr = allocator->calloc(allocator, nmemb, size);
  assert(ptr != NULL);
  return ptr;
}
#endif

// The realloc() function changes the size of the memory block pointed to by ptr
// to newsize bytes. The contents of the memory will be unchanged in the range
// from the start of the region up to the minimum of the old and new sizes. If
// the new size is larger than the old size, the added memory will not be
// initialized.
void *alloc_realloc(Allocator *allocator, void *ptr, size_t newsize);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_realloc(Allocator *allocator, void *ptr, size_t newsize) {
  ptr = allocator->realloc(allocator, ptr, newsize);
  assert(ptr != NULL);
  return ptr;
}
#endif

// malloc implementation for libc allocator. Call alloc_malloc instead of this
// function directly.
void *libc_malloc(Allocator *, size_t size);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *libc_malloc(Allocator *alloc, size_t size) {
  (void)alloc;
  return malloc(size);
}
#endif

// free implementation for libc allocator. Call alloc_free instead of this
// function directly.
void libc_free(Allocator *, void *);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void libc_free(Allocator *alloc, void *ptr) {
  (void)alloc;
  free(ptr);
}
#endif

// calloc implementation for libc allocator. Call alloc_calloc instead of this
// function directly.
void *libc_calloc(Allocator *, size_t nmemb, size_t size);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *libc_calloc(Allocator *alloc, size_t nmemb, size_t size) {
  (void)alloc;
  void *ptr = calloc(nmemb, size);
  return ptr;
}
#endif

// realloc implementation for libc allocator. Call alloc_realloc instead of this
// function directly.
void *libc_realloc(Allocator *, void *ptr, size_t size);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *libc_realloc(Allocator *alloc, void *ptr, size_t size) {
  (void)alloc;
  return realloc(ptr, size);
}
#endif

// libc_alloc returns a libc based memory allocator.
void libc_alloc_init(Allocator *);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void libc_alloc_init(Allocator *alloc) {
  alloc->malloc = &libc_malloc;
  alloc->free = &libc_free;
  alloc->calloc = &libc_calloc;
  alloc->realloc = &libc_realloc;
}
#endif

#endif
#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#ifdef XSTD_ARENA_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#define XSTD_ALLOC_IMPLEMENTATION
#endif


// ArenaAllocator is an arena allocator.
typedef struct xstd_arena_allocator {
  Allocator allocator;
  Allocator *parent_allocator;
  size_t arena_size;
  struct xstd_arena *arena_list;
  size_t cursor;
} ArenaAllocator;

struct xstd_arena {
  // Header
  struct xstd_arena *next;

  // Body
  // ...
};

void *arena_calloc(Allocator *allocator, size_t nmemb, size_t size);

#ifdef XSTD_ARENA_IMPLEMENTATION
void *arena_calloc(Allocator *a, size_t nmemb, size_t size) {
  ArenaAllocator *alloc = (ArenaAllocator *)a;

  // Detect overflow.
  size_t alloc_size = nmemb * size;
  if (alloc_size / size != nmemb ||
      alloc_size + sizeof(struct xstd_arena) < alloc_size) {
    return NULL;
  }

  if (alloc_size > alloc->arena_size) {
    struct xstd_arena *new_arena = alloc_calloc(
        alloc->parent_allocator, 1, alloc_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list;
    alloc->arena_list = new_arena;
    alloc->cursor = alloc->arena_size; // Allocated arena as full.
    return (void *)((uintptr_t)new_arena + sizeof(struct xstd_arena));
  }

  // Allocate at least size of a pointer to ensure all pointer are aligned.
  alloc_size = alloc_size < sizeof(void *) ? sizeof(void *) : alloc_size;

  // No current arena.
  if (alloc->arena_list == NULL) {
    struct xstd_arena *new_arena =
        alloc_calloc(alloc->parent_allocator, 1,
                     alloc->arena_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list;
    alloc->arena_list = new_arena;
    alloc->cursor = 0;
  } else if (alloc->arena_size - alloc->cursor < alloc_size) {
    struct xstd_arena *new_arena =
        alloc_calloc(alloc->parent_allocator, 1,
                     alloc->arena_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list;
    alloc->arena_list = new_arena;
    alloc->cursor = 0;
  }

  void *ptr = (void *)((uintptr_t)alloc->arena_list +
                       sizeof(struct xstd_arena) + alloc->cursor);
  alloc->cursor += alloc_size;
  return ptr;
}
#endif

void *arena_malloc(Allocator *allocator, size_t size);

#ifdef XSTD_ARENA_IMPLEMENTATION
void *arena_malloc(Allocator *allocator, size_t size) {
  return arena_calloc(allocator, 1, size);
}
#endif

void *arena_realloc(Allocator *allocator, void *ptr, size_t size);

#ifdef XSTD_ARENA_IMPLEMENTATION
void *arena_realloc(Allocator *allocator, void *ptr, size_t size) {
  (void)ptr;
  // TODO: if last alloc, return same pointer.
  // TODO: if new size is smaller, return same pointer.

  return alloc_malloc(allocator, size);
}
#endif

void arena_free(Allocator *allocator, void *ptr);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_free(Allocator *allocator, void *ptr) {
  (void)allocator;
  (void)ptr;
}
#endif

void arena_alloc_init(ArenaAllocator *arena, Allocator *parent,
                      size_t arena_size);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_alloc_init(ArenaAllocator *arena, Allocator *parent,
                      size_t arena_size) {
  assert(arena_size > sizeof(void *) &&
         "arena size must be greater than size of a pointer");

  arena->parent_allocator = parent;
  arena->arena_size = arena_size;
  arena->arena_list = NULL;
  arena->allocator = (Allocator){0};
  arena->allocator.malloc = &arena_malloc;
  arena->allocator.calloc = &arena_calloc;
  arena->allocator.realloc = &arena_realloc;
  arena->allocator.free = &arena_free;
}
#endif

void arena_alloc_reset(ArenaAllocator *arena);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_alloc_reset(ArenaAllocator *arena_alloc) {
  struct xstd_arena **arena = &arena_alloc->arena_list;
  while (*arena != NULL) {
    struct xstd_arena *a = *arena;
    *arena = a->next;
    alloc_free(arena_alloc->parent_allocator, a);
  }

  arena_alloc_init(arena_alloc, arena_alloc->parent_allocator,
                   arena_alloc->arena_size);
}
#endif

#endif
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


typedef void *Vec;

struct xstd_vector {
  // Header
  Allocator *allocator;
  size_t cap;
  size_t len;
  size_t elem_size;

  // Body
  // ...
};

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
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size);

#ifdef XSTD_VEC_IMPLEMENTATION
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size) {
  size_t header = sizeof(struct xstd_vector);
  size_t body = cap * elem_size;

  struct xstd_vector *vec = alloc_calloc(allocator, header + body, 1);
  if (vec == NULL)
    return NULL;

  vec->allocator = allocator;
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

  void *clone = alloc_malloc(v->allocator, size_v);
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

    void *new = vec_new(old->allocator, old->cap * 2, old->elem_size);
    headerof_vec(new)->len = old->len;

    memcpy(new, (void *)bodyof_vec(old), old->cap * old->elem_size);
    alloc_free(old->allocator, old);

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

    Vec new = vec_new(old->allocator, old->cap * 2, old->elem_size);
    headerof_vec(new)->len = old->len + 1;

    // Copy old elements in new at index 1
    memcpy((void *)((uintptr_t) new + old->elem_size), (void *)bodyof_vec(old),
           old->cap * old->elem_size);
    alloc_free(old->allocator, old);

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
// Single file header iterator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_LIST_H_INCLUDE
#define XSTD_LIST_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <string.h>


#ifdef XSTD_LIST_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif

typedef void *List;

struct xstd_list {
  // Header
  struct xstd_list *next;

  // Body
  // ...
};

#ifdef XSTD_LIST_IMPLEMENTATION
// private macros
#define bodyof_list(listptr) ((uintptr_t)(listptr) + sizeof(struct xstd_list))
#define headerof_list(listptr)                                                 \
  ((struct xstd_list *)((uintptr_t)listptr - sizeof(struct xstd_list)))
#endif

#define list_foreach(list, iterator)                                           \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        typeof(*list) value;                                                   \
        typeof(list) item;                                                     \
      } iterator = {.value = *list, .item = list};                             \
      iterator.item != NULL; iterator.item = list_next(iterator.item),         \
        iterator.value = iterator.item != NULL ? *iterator.item                \
                                               : (typeof(*list)){0})

#define list_foreach_ptr(list, iterator)                                       \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(list) value;                                                   \
       } iterator = {.value = list};                                           \
       iterator.value != NULL; iterator.value = list_next(iterator.value))

#define list_push(list)                                                        \
  ((typeof(*list))_list_push((void **)list, sizeof(typeof(*list))))

void *_list_push(List *, size_t);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_push(void **list, size_t item_size) {
  assert(list != NULL);

  void *head =
      (void *)bodyof_list(calloc(1, sizeof(struct xstd_list) + item_size));

  headerof_list(head)->next = headerof_list(*list);

  *list = head;

  return head;
}
#endif

#define list_next(list) (typeof(list))_list_next(list)

void *_list_next(const List);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_next(const List l) {
  if (l == NULL)
    return NULL;

  return (void *)bodyof_list(headerof_list(l)->next);
}
#endif

#define list_remove_head(list) _list_remove_head((void **)list)

// list_remove_head removes head element from the list. You must not call this
// fuction with an element in a middle of a list otherwise you will encounter a
// use after free error (segfault).
void _list_remove_head(List *);

#ifdef XSTD_LIST_IMPLEMENTATION
void _list_remove_head(void **l) {
  if (*l == NULL)
    return;

  void *tmp = headerof_list(*l);
  *l = list_next(*l);
  free(tmp);
}
#endif

// list_remove_next removes next element from the given list.
void list_remove_next(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_remove_next(List l) {
  if (l == NULL)
    return;

  List next = list_next(l);
  if (next != NULL) {
    struct xstd_list *list = headerof_list(l);
    list->next = headerof_list(next)->next;
    free(headerof_list(next));
  }
}
#endif

// list_free frees all elements contained in the list. You must not call this
// fuction with an element in a middle of a list otherwise you will encounter a
// use after free error (segfault).
void list_free(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_free(List l) {
  if (l == NULL)
    return;

  while (l != NULL) {
    List tmp = l;
    l = list_next(l);
    free(headerof_list(tmp));
  }
}
#endif

// list_free_next frees all elements following the given list.
void list_free_next(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_free_next(List l) {
  if (l == NULL)
    return;

  List next = list_next(l);
  if (next != NULL)
    list_free(next);

  headerof_list(l)->next = headerof_list(NULL);
}
#endif

#define list_end(list) ((typeof(list))_list_end(list))

// _list_end returns a pointer to last element of list.
void *_list_end(List l1);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_end(List l) {
  void *result = NULL;

  list_foreach_ptr(l, iter) { result = iter.value; }

  return result;
}
#endif

#endif
// Single file header iterator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ITER_H_INCLUDE
#define XSTD_ITER_H_INCLUDE

#include <stdint.h>
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
