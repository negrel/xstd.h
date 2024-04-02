#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#include <stdint.h>
#define XSTD_ARENA_IMPLEMENTATION
#ifdef XSTD_ARENA_IMPLEMENTATION

#define XSTD_ALLOC_IMPLEMENTATION
#endif

#include "xstd_alloc.h"

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
