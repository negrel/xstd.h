#ifndef XSTD_IO_WRITER_H_INCLUDE
#define XSTD_IO_WRITER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
#include <errno.h>
#endif

// Writer interface / virtual table. Writer is the interface that wraps the
// basic Write method.
//
// Write writes len(p) bytes from p to the underlying data stream. It returns
// the number of bytes written from p (0 <= n <= len(p)) and any error
// encountered that caused the write to stop early. Write must return a non-nil
// error if it returns n < len(p). Write must not modify the slice data, even
// temporarily.
typedef struct xstd_writer {
  void (*write)(struct xstd_writer *writer, uint8_t *p, size_t p_len, size_t *n,
                int *err);
} Writer;

void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error) {
  writer->write(writer, p, p_len, n, error);
}
#endif

// FileWriter wraps FILE and implements Writer.
typedef struct {
  Writer writer;
  FILE *f_;
} FileWriter;

// file_writer_write implements Writer.
void file_writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                       int *error);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
void file_writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                       int *error) {
  FileWriter *fwriter = (FileWriter *)writer;
  size_t write = fwrite(p, sizeof(*p), p_len, fwriter->f_);
  if (n != NULL)
    *n = write;

  if (error != NULL) {
    *error = errno;
    if (write == 0 && feof(fwriter->f_)) {
      *error = EOF;
    }
    clearerr(fwriter->f_);
  }
}
#endif

// file_writer creates and returns a FileWriter that wraps the given file.
FileWriter file_writer(FILE *f);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
FileWriter file_writer(FILE *f) {
  FileWriter fwriter = {0};
  fwriter.writer.write = &file_writer_write;
  fwriter.f_ = f;
  return fwriter;
}
#endif

#endif
