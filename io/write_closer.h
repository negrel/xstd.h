#ifndef XSTD_IO_WRITE_CLOSER_H_INCLUDE
#define XSTD_IO_WRITE_CLOSER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#endif

#include "alloc.h"
#include "io/closer.h"
#include "io/writer.h"

// WriteCloser is the interface that groups the basic Write and Close methods.
typedef struct {
  Writer writer;
  Closer closer;
} WriteCloser;

// FileWriteCloser wraps FILE and implements WriteCloser.
typedef struct {
  WriteCloser write_closer;
  FILE *f_;
} FileWriteCloser;

// file_write_closer_init initializes the given FileWriteCloser.
void file_write_closer_init(FileWriteCloser *frw, FILE *f);

#ifdef XSTD_IMPLEMENTATION
void file_write_closer_init(FileWriteCloser *frw, FILE *f) {
  *frw = (FileWriteCloser){0};
  frw->write_closer.writer.vtable_ = &file_writer_vtable;
  frw->write_closer.writer.offset_ = offsetof(FileWriteCloser, f_);

  frw->write_closer.closer.vtable_ = &file_closer_vtable;
  frw->write_closer.closer.offset_ =
      offsetof(FileWriteCloser, f_) -
      offsetof(FileWriteCloser, write_closer.closer);

  frw->f_ = f;
}
#endif

// file_write_closer allocates and initializes and returns a FileWriteCloser
// that wraps the given file.
FileWriteCloser *file_write_closer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileWriteCloser *file_write_closer_new(Allocator *allocator, FILE *f) {
  FileWriteCloser *frw = alloc_malloc(allocator, sizeof(FileWriteCloser));
  file_write_closer_init(frw, f);
  return frw;
}
#endif

// file_write_closer initializes and returns a FileWriteCloser that wraps the
// given file.
FileWriteCloser file_write_closer(FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileWriteCloser file_write_closer(FILE *f) {
  FileWriteCloser frw = {0};
  file_write_closer_init(&frw, f);
  return frw;
}
#endif

#endif
