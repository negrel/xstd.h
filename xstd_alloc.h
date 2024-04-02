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
