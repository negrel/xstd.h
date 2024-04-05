#ifndef XSTD_IO_READER_H_INCLUDE
#define XSTD_IO_READER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IO_READER_IMPLEMENTATION
#include <errno.h>
#endif

// Reader interface / virtual table. Reader is the interface that wraps the
// basic read method.
//
// read reads up to len(p) bytes into p. It returns the number of bytes read (0
// <= n <= p_len) and any error encountered. Even if read returns n < p_len,
// it may use all of p as scratch space during the call. If some data is
// available but not p_len bytes, read conventionally returns what is available
// instead of waiting for more.
//
// When read encounters an error or end-of-file condition after successfully
// reading n > 0 bytes, it returns the number of bytes read. It may return the
// (non-null) error from the same call or return the error (and n == 0) from a
// subsequent call. An instance of this general case is that a Reader returning
// a non-zero number of bytes at the end of the input stream may return either
// err == EOF or err == nil. The next read should return 0, EOF.
//
// Callers should always process the n > 0 bytes returned before considering the
// error err. Doing so correctly handles I/O errors that happen after reading
// some bytes and also both of the allowed EOF behaviors.
//
// If p_len == 0, read should always return n == 0. It may return a non-null
// error if some error condition is known, such as EOF.
//
// Implementations of read are discouraged from returning a zero byte count with
// a nil error, except when len(p) == 0. Callers should treat a return of 0 and
// nil as indicating that nothing happened; in particular it does not indicate
// EOF.
//
// Implementations must not retain p.
typedef struct xstd_reader {
  void (*read)(struct xstd_reader *reader, uint8_t *p, size_t p_len, size_t *n,
               int *err);
} Reader;

// reader_read calls the read method of the reader.
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n, int *err);

#ifdef XSTD_IO_READER_IMPLEMENTATION
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                 int *err) {
  reader->read(reader, p, p_len, n, err);
}
#endif

// FileReader wraps FILE and implements Reader.
typedef struct {
  Reader reader;
  FILE *f_;
} FileReader;

// file_reader_read implements Reader.
void file_reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                      int *error);

#ifdef XSTD_IO_READER_IMPLEMENTATION
void file_reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                      int *error) {
  FileReader *freader = (FileReader *)reader;
  size_t read = fread(p, sizeof(*p), p_len, freader->f_);
  if (n != NULL)
    *n = read;

  if (error != NULL) {
    *error = errno;
    if (read == 0 && feof(freader->f_)) {
      *error = EOF;
    }
    clearerr(freader->f_);
  }
}
#endif

// file_reader creates and returns a FileReader that wraps the given file.
FileReader file_reader(FILE *f);

#ifdef XSTD_IO_READER_IMPLEMENTATION
FileReader file_reader(FILE *f) {
  FileReader freader = {0};
  freader.reader.read = &file_reader_read;
  freader.f_ = f;
  return freader;
}
#endif

#endif
