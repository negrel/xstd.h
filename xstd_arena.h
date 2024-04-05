#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#ifdef XSTD_ARENA_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#define XSTD_ALLOC_IMPLEMENTATION
#endif

#include "xstd_alloc.h"

// ArenaAllocator is an arena allocator.
typedef struct xstd_arena_allocator {
  Allocator allocator_;
  Allocator *parent_allocator_;
  size_t arena_size_;
  struct xstd_arena *arena_list_;
  size_t cursor_;
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

  if (alloc_size > alloc->arena_size_) {
    struct xstd_arena *new_arena = alloc_calloc(
        alloc->parent_allocator_, 1, alloc_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list_;
    alloc->arena_list_ = new_arena;
    alloc->cursor_ = alloc->arena_size_; // Allocated arena as full.
    return (void *)((uintptr_t)new_arena + sizeof(struct xstd_arena));
  }

  // Allocate at least size of a pointer to ensure all pointer are aligned.
  alloc_size = alloc_size < sizeof(void *) ? sizeof(void *) : alloc_size;

  // No current arena.
  if (alloc->arena_list_ == NULL) {
    struct xstd_arena *new_arena =
        alloc_calloc(alloc->parent_allocator_, 1,
                     alloc->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list_;
    alloc->arena_list_ = new_arena;
    alloc->cursor_ = 0;
  } else if (alloc->arena_size_ - alloc->cursor_ < alloc_size) {
    struct xstd_arena *new_arena =
        alloc_calloc(alloc->parent_allocator_, 1,
                     alloc->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = alloc->arena_list_;
    alloc->arena_list_ = new_arena;
    alloc->cursor_ = 0;
  }

  void *ptr = (void *)((uintptr_t)alloc->arena_list_ +
                       sizeof(struct xstd_arena) + alloc->cursor_);
  alloc->cursor_ += alloc_size;
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

  arena->parent_allocator_ = parent;
  arena->arena_size_ = arena_size;
  arena->arena_list_ = NULL;
  arena->allocator_ = (Allocator){0};
  arena->allocator_.malloc = &arena_malloc;
  arena->allocator_.calloc = &arena_calloc;
  arena->allocator_.realloc = &arena_realloc;
  arena->allocator_.free = &arena_free;
}
#endif

void arena_alloc_reset(ArenaAllocator *arena);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_alloc_reset(ArenaAllocator *arena_alloc) {
  struct xstd_arena **arena = &arena_alloc->arena_list_;
  while (*arena != NULL) {
    struct xstd_arena *a = *arena;
    *arena = a->next;
    alloc_free(arena_alloc->parent_allocator_, a);
  }

  arena_alloc_init(arena_alloc, arena_alloc->parent_allocator_,
                   arena_alloc->arena_size_);
}
#endif

#endif
