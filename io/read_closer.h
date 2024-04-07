#ifndef XSTD_IO_READ_CLOSER_H_INCLUDE
#define XSTD_IO_READ_CLOSER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#endif

#include "alloc.h"
#include "io/closer.h"
#include "io/reader.h"

// ReadCloser is the interface that groups the basic Read and Close methods.
typedef struct {
  Reader reader;
  Closer closer;
} ReadCloser;

// FileReadCloser wraps FILE and implements ReadCloser.
typedef struct {
  ReadCloser read_closer;
  FILE *f_;
} FileReadCloser;

// file_read_closer_init initializes the given FileReadCloser.
void file_read_closer_init(FileReadCloser *frw, FILE *f);

#ifdef XSTD_IMPLEMENTATION
void file_read_closer_init(FileReadCloser *frw, FILE *f) {
  *frw = (FileReadCloser){0};
  frw->read_closer.reader.vtable_ = &file_reader_vtable;
  frw->read_closer.reader.offset_ = offsetof(FileReadCloser, f_);

  frw->read_closer.closer.vtable_ = &file_closer_vtable;
  frw->read_closer.closer.offset_ =
      offsetof(FileReadCloser, f_) -
      offsetof(FileReadCloser, read_closer.closer);

  frw->f_ = f;
}
#endif

// file_read_closer allocates and initializes and returns a FileReadCloser that
// wraps the given file.
FileReadCloser *file_read_closer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileReadCloser *file_read_closer_new(Allocator *allocator, FILE *f) {
  FileReadCloser *frw = alloc_malloc(allocator, sizeof(FileReadCloser));
  file_read_closer_init(frw, f);
  return frw;
}
#endif

// file_read_closer initializes and returns a FileReadCloser that wraps the
// given file.
FileReadCloser file_read_closer(FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileReadCloser file_read_closer(FILE *f) {
  FileReadCloser frw = {0};
  file_read_closer_init(&frw, f);
  return frw;
}
#endif

#endif
