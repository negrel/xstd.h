// Single file header allocator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ALLOC_H_INCLUDE
#define XSTD_ALLOC_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif

#include "iface.h"

// Allocator interface.
iface_def(
    Allocator, struct xstd_allocator_vtable {
      void *(*malloc)(InterfaceImpl allocator, size_t size);
      void (*free)(InterfaceImpl allocator, void *ptr);
      void *(*calloc)(InterfaceImpl allocator, size_t nmemb, size_t size);
      void *(*realloc)(InterfaceImpl allocator, void *ptr, size_t newsize);
    });

// alloc_malloc() allocates size bytes and returns a pointer to the allocated
// memory. The memory is not initialized.
void *alloc_malloc(Allocator *, size_t);

#ifdef XSTD_IMPLEMENTATION
void *alloc_malloc(Allocator *allocator, size_t size) {
  void *ptr = iface_call(allocator, malloc, size);
  assert(ptr != NULL);
  return ptr;
}
#endif

// alloc_free() frees the memory space pointed to by ptr, which must have been
// returned by a previous call to malloc() or related functions.
void alloc_free(Allocator *allocator, void *ptr);

#ifdef XSTD_IMPLEMENTATION
void alloc_free(Allocator *allocator, void *ptr) {
  iface_call(allocator, free, ptr);
}
#endif

// The calloc() function allocates memory for an array of nmemb elements of size
// bytes each and returns a pointer to the allocated memory. The memory is set
// to zero.
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size);

#ifdef XSTD_IMPLEMENTATION
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size) {
  void *ptr = iface_call(allocator, calloc, nmemb, size);
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

#ifdef XSTD_IMPLEMENTATION
void *alloc_realloc(Allocator *allocator, void *ptr, size_t newsize) {
  ptr = iface_call(allocator, realloc, ptr, newsize);
  assert(ptr != NULL);
  return ptr;
}
#endif

Allocator *g_libc_allocator;

#ifdef XSTD_IMPLEMENTATION
static void *libc_malloc(InterfaceImpl alloc, size_t size) {
  (void)alloc;
  return malloc(size);
}

static void libc_free(InterfaceImpl alloc, void *ptr) {
  (void)alloc;
  free(ptr);
}

static void *libc_calloc(InterfaceImpl alloc, size_t nmemb, size_t size) {
  (void)alloc;
  void *ptr = calloc(nmemb, size);
  return ptr;
}

static void *libc_realloc(InterfaceImpl alloc, void *ptr, size_t size) {
  (void)alloc;
  return realloc(ptr, size);
}

static struct xstd_allocator_vtable libc_allocator_vtable = {
    .malloc = &libc_malloc,
    .free = &libc_free,
    .calloc = &libc_calloc,
    .realloc = &libc_realloc,
};

static Allocator libc_allocator = {
    .vtable_ = &libc_allocator_vtable,
    .offset_ = 0,
};

Allocator *g_libc_allocator = &libc_allocator;
#endif

#endif
