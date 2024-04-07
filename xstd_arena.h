#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_ARENA_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#define XSTD_ALLOC_IMPLEMENTATION
#endif

#include "xstd_alloc.h"

struct xstd_arena_allocator_body {
  Allocator *parent_allocator_;
  size_t arena_size_;
  struct xstd_arena *arena_list_;
  size_t cursor_;
};

// ArenaAllocator is an arena allocator.
typedef struct xstd_arena_allocator {
  Allocator allocator;
  struct xstd_arena_allocator_body body_;
} ArenaAllocator;

struct xstd_arena {
  // Header
  struct xstd_arena *next;

  // Body
  // ...
};

void arena_allocator_init(ArenaAllocator *arena, Allocator *parent,
                          size_t arena_size);

#ifdef XSTD_ARENA_IMPLEMENTATION
static void *arena_calloc(void *b, size_t nmemb, size_t size) {
  struct xstd_arena_allocator_body *arena_body =
      (struct xstd_arena_allocator_body *)b;

  // Detect overflow.
  size_t alloc_size = nmemb * size;
  if (alloc_size / size != nmemb ||
      alloc_size + sizeof(struct xstd_arena) < alloc_size) {
    return NULL;
  }

  if (alloc_size > arena_body->arena_size_) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     alloc_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = arena_body->arena_size_; // Allocated arena as full.
    return (void *)((uintptr_t)new_arena + sizeof(struct xstd_arena));
  }

  // Allocate at least size of a pointer to ensure all pointer are aligned.
  alloc_size = alloc_size < sizeof(void *) ? sizeof(void *) : alloc_size;

  // No current arena.
  if (arena_body->arena_list_ == NULL) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     arena_body->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = 0;
  } else if (arena_body->arena_size_ - arena_body->cursor_ < alloc_size) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     arena_body->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = 0;
  }

  void *ptr = (void *)((uintptr_t)arena_body->arena_list_ +
                       sizeof(struct xstd_arena) + arena_body->cursor_);
  arena_body->cursor_ += alloc_size;
  return ptr;
}

static void *arena_malloc(void *allocator, size_t size) {
  return arena_calloc(allocator, 1, size);
}

static void *arena_realloc(void *allocator, void *ptr, size_t size) {
  (void)ptr;
  // TODO: if last alloc, return same pointer.
  // TODO: if new size is smaller, return same pointer.

  return arena_malloc(allocator, size);
}

static void arena_free(void *allocator, void *ptr) {
  (void)allocator;
  (void)ptr;
}

static struct xstd_allocator_vtable arena_allocator_vtable = {
    .malloc = &arena_malloc,
    .free = &arena_free,
    .calloc = &arena_calloc,
    .realloc = &arena_realloc,
};

void arena_allocator_init(ArenaAllocator *arena, Allocator *parent,
                          size_t arena_size) {
  assert(arena_size > sizeof(void *) &&
         "arena size must be greater than size of a pointer");

  arena->body_.parent_allocator_ = parent;
  arena->body_.arena_size_ = arena_size;
  arena->body_.arena_list_ = NULL;
  arena->allocator = (Allocator){0};
  arena->allocator.vtable_ = &arena_allocator_vtable;
  arena->allocator.offset_ = offsetof(ArenaAllocator, body_);
}
#endif

ArenaAllocator arena_allocator(Allocator *parent, size_t arena_size);

#ifdef XSTD_ARENA_IMPLEMENTATION
ArenaAllocator arena_allocator(Allocator *parent, size_t arena_size) {
  ArenaAllocator alloc = {0};
  arena_allocator_init(&alloc, parent, arena_size);
  return alloc;
}
#endif

void arena_alloc_reset(ArenaAllocator *arena);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_alloc_reset(ArenaAllocator *arena_alloc) {
  struct xstd_arena **arena = &arena_alloc->body_.arena_list_;
  while (*arena != NULL) {
    struct xstd_arena *a = *arena;
    *arena = a->next;
    alloc_free(arena_alloc->body_.parent_allocator_, a);
  }

  arena_allocator_init(arena_alloc, arena_alloc->body_.parent_allocator_,
                       arena_alloc->body_.arena_size_);
}
#endif

#endif
