#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_IMPLEMENTATION
#include <stdint.h>
#endif

#include "alloc.h"
#include "constructor.h"
#include "iface.h"

// ArenaAllocator is an arena allocator.
iface_impl_def(
    Allocator, ArenaAllocator, struct {
      Allocator *parent_allocator_;
      size_t arena_size_;
      struct xstd_arena *arena_list_;
      size_t cursor_;
    });

struct xstd_arena {
  // Header
  struct xstd_arena *next;

  // Body
  // ...
};

void arena_allocator_init(ArenaAllocator *arena, Allocator *parent,
                          size_t arena_size);

#ifdef XSTD_IMPLEMENTATION
static void *arena_calloc(InterfaceImpl impl, size_t nmemb, size_t size) {
  typeof_iface_impl(ArenaAllocator) *arena_body =
      cast_iface_impl_ptr(ArenaAllocator, impl);

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
#endif

struct xstd_allocator_vtable arena_allocator_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_allocator_vtable arena_allocator_vtable = {
    .malloc = &arena_malloc,
    .free = &arena_free,
    .calloc = &arena_calloc,
    .realloc = &arena_realloc,
};
#endif

#undef type_init_proto
#undef type_init_args
#undef type_init
#define type_init_proto Allocator *parent, size_t arena_size
#define type_init_args parent, arena_size
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface, &arena_allocator_vtable,                         \
                    ((typeof_iface_impl(ArenaAllocator)){                      \
                        .parent_allocator_ = parent,                           \
                        .arena_size_ = arena_size,                             \
                    }))                                                        \
  }

def_type_constructors(ArenaAllocator, arena_allocator)

    void arena_alloc_destroy(ArenaAllocator *arena);

#ifdef XSTD_IMPLEMENTATION
void arena_alloc_destroy(ArenaAllocator *arena_alloc) {
  struct xstd_arena **arena = &arena_alloc->body_.arena_list_;
  while (*arena != NULL) {
    struct xstd_arena *a = *arena;
    *arena = a->next;
    alloc_free(arena_alloc->body_.parent_allocator_, a);
  }
  arena_alloc->body_.arena_list_ = NULL;
}
#endif

#endif
