#ifndef XSTD_IO_READ_WRITER_H_INCLUDE
#define XSTD_IO_READ_WRITER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#endif

#include "alloc.h"
#include "io/reader.h"
#include "io/writer.h"

// ReadWriter is the interface that groups the basic Read and Write methods.
typedef struct {
  Reader reader;
  Writer writer;
} ReadWriter;

// FileReadWriter wraps FILE and implements ReadWriter.
typedef struct {
  ReadWriter read_writer;
  FILE *f_;
} FileReadWriter;

// file_read_writer_init initializes the given FileReadWriter.
void file_read_writer_init(FileReadWriter *frw, FILE *f);

#ifdef XSTD_IMPLEMENTATION
void file_read_writer_init(FileReadWriter *frw, FILE *f) {
  *frw = (FileReadWriter){0};
  frw->read_writer.reader.vtable_ = &file_reader_vtable;
  frw->read_writer.reader.offset_ = offsetof(FileReadWriter, f_);

  frw->read_writer.writer.vtable_ = &file_writer_vtable;
  frw->read_writer.writer.offset_ =
      offsetof(FileReadWriter, f_) -
      offsetof(FileReadWriter, read_writer.writer);

  frw->f_ = f;
}
#endif

// file_read_writer allocates and initializes and returns a FileReadWriter that
// wraps the given file.
FileReadWriter *file_read_writer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileReadWriter *file_read_writer_new(Allocator *allocator, FILE *f) {
  FileReadWriter *frw = alloc_malloc(allocator, sizeof(FileReadWriter));
  file_read_writer_init(frw, f);
  return frw;
}
#endif

// file_read_writer initializes and returns a FileReadWriter that wraps the
// given file.
FileReadWriter file_read_writer(FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileReadWriter file_read_writer(FILE *f) {
  FileReadWriter frw = {0};
  file_read_writer_init(&frw, f);
  return frw;
}
#endif

#endif
