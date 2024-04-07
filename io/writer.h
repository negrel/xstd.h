#ifndef XSTD_IO_WRITER_H_INCLUDE
#define XSTD_IO_WRITER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#endif

#include "alloc.h"

struct xstd_writer_vtable {
  void (*write)(void *writer, uint8_t *p, size_t p_len, size_t *n, int *err);
};

// Writer is the interface that wraps the basic Write method.
//
// Write writes len(p) bytes from p to the underlying data stream. It returns
// the number of bytes written from p (0 <= n <= len(p)) and any error
// encountered that caused the write to stop early. Write must return a non-nil
// error if it returns n < len(p). Write must not modify the slice data, even
// temporarily.
typedef struct {
  struct xstd_writer_vtable *vtable_;
  size_t offset_;
} Writer;

void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error);

#ifdef XSTD_IMPLEMENTATION
void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error) {
  writer->vtable_->write((void *)((uintptr_t)writer + writer->offset_), p,
                         p_len, n, error);
}
#endif

// FileWriter wraps FILE and implements Writer.
typedef struct {
  Writer writer;
  FILE *f_;
} FileWriter;

#ifdef XSTD_IMPLEMENTATION
static void file_writer_write(void *file, uint8_t *p, size_t p_len, size_t *n,
                              int *error) {
  FILE *f = *(void **)file;
  size_t write = fwrite(p, sizeof(*p), p_len, f);
  if (n != NULL)
    *n = write;

  if (error != NULL) {
    *error = errno;
    if (write == 0 && feof(f))
      *error = EOF;

    clearerr(f);
  }
}

#endif

// FileWriter virtual table for Writer interface.
struct xstd_writer_vtable file_writer_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_writer_vtable file_writer_vtable = {.write = &file_writer_write};
#endif

// file_writer_init initializes the given FileWriter.
void file_writer_init(FileWriter *fw, FILE *f);

#ifdef XSTD_IMPLEMENTATION
void file_writer_init(FileWriter *fw, FILE *f) {
  *fw = (FileWriter){0};
  fw->writer.vtable_ = &file_writer_vtable;
  fw->writer.offset_ = offsetof(FileWriter, f_);
  fw->f_ = f;
}
#endif

// file_writer_new allocates, initializes and returns a new FileWriter.
FileWriter *file_writer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileWriter *file_writer_new(Allocator *allocator, FILE *f) {
  FileWriter *fw = alloc_malloc(allocator, sizeof(FileWriter));
  file_writer_init(fw, f);
  return fw;
}
#endif

// file_writer initializes and returns a FileWriter that wraps the given file.
FileWriter file_writer(FILE *f);

#ifdef XSTD_IMPLEMENTATION
FileWriter file_writer(FILE *f) {
  FileWriter fw = {0};
  file_writer_init(&fw, f);
  return fw;
}
#endif

#endif
