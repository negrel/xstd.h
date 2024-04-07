#ifndef XSTD_IO_CLOSER_H_INCLUDE
#define XSTD_IO_CLOSER_H_INCLUDE

#include <stdbool.h>
#include <stdio.h>

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#endif

#include "alloc.h"

struct xstd_closer_vtable {
  void (*close)(void *closer, int *error);
};

// Closer interface / virtual table. Closer is the interface that wraps basic
// Close method. The behavior of Close after the first call is undefined.
// Specific implementations may document their own behavior.
typedef struct {
  struct xstd_closer_vtable *vtable_;
  size_t offset_;
} Closer;

// closer_close calls the close method of the closer.
void closer_close(Closer *closer, int *error);

#ifdef XSTD_IMPLEMENTATION
void closer_close(Closer *closer, int *error) {
  closer->vtable_->close((void *)((uintptr_t)closer + closer->offset_), error);
}
#endif

// FileCloser wraps FILE and implements the Closer interface.
typedef struct {
  Closer closer;
  FILE *f_;
} FileCloser;

#ifdef XSTD_IMPLEMENTATION
static void file_closer_close(void *file, int *error) {
  FILE *f = *(void **)file;
  if (!fclose(f))
    *error = errno;
}
#endif

struct xstd_closer_vtable file_closer_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_closer_vtable file_closer_vtable = {.close = &file_closer_close};
#endif

// file_closer_init initializes the given FileCloser.
void file_closer_init(FileCloser *fcloser, FILE *f);

#ifdef XSTD_IMPLEMENTATION
void file_closer_init(FileCloser *fcloser, FILE *f) {
  *fcloser = (FileCloser){0};
  fcloser->closer.vtable_ = &file_closer_vtable;
  fcloser->closer.offset_ = offsetof(FileCloser, f_);
  fcloser->f_ = f;
}
#endif

// file_closer_new allocates, initiliazes and returns a new FileCloser.
FileCloser *file_closer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileCloser *file_closer_new(Allocator *allocator, FILE *f) {
  FileCloser *fcloser = alloc_malloc(allocator, sizeof(FileCloser));
  file_closer_init(fcloser, f);
  return fcloser;
}
#endif

// file_closer initiliazes and returns a FileCloser that wraps the given file.
FileCloser file_closer(FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileCloser file_closer(FILE *f) {
  FileCloser fcloser = {0};
  file_closer_init(&fcloser, f);
  return fcloser;
}
#endif

#endif
