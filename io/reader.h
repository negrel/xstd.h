#ifndef XSTD_IO_READER_H_INCLUDE
#define XSTD_IO_READER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#endif

#include "constructor.h"
#include "iface.h"

// Reader is the interface that wraps the basic read method.
//
// read reads up to len(p) bytes into p. It returns the number of bytes read
// (0 <= n <= p_len) and any error encountered. Even if read returns n < p_len,
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
iface_def(
    Reader, struct xstd_reader_vtable {
      void (*read)(InterfaceImpl reader, uint8_t *p, size_t p_len, size_t *n,
                   int *err);
    });

// reader_read calls the read method of the reader.
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n, int *err);

#ifdef XSTD_IMPLEMENTATION
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                 int *err) {
  iface_call(reader, read, p, p_len, n, err);
}
#endif

// FileReader wraps FILE and implements Reader.
iface_impl_def(Reader, FileReader, FILE *);

// file_reader_read implements Reader.
#ifdef XSTD_IMPLEMENTATION
static void file_reader_read(InterfaceImpl impl, uint8_t *p, size_t p_len,
                             size_t *n, int *error) {
  typeof_iface_impl(FileReader) f = cast_iface_impl(FileReader, impl);
  size_t read = fread(p, sizeof(*p), p_len, f);
  if (n != NULL)
    *n = read;

  if (error != NULL) {
    *error = errno;
    if (read == 0 && feof(f)) {
      *error = EOF;
    }
    clearerr(f);
  }
}
#endif

// FileReader virtual table for Reader interface.
struct xstd_reader_vtable file_reader_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_reader_vtable file_reader_vtable = {.read = &file_reader_read};
#endif

// Generate file_writer_init, file_writer_new and file_writer constructors.

#undef type_init_proto
#undef type_init_args
#undef type_init

#define type_init_proto FILE *f
#define type_init_args f
#define type_init                                                              \
  { iface_impl_init(t, iface, &file_reader_vtable, f) }

def_type_constructors(FileReader, file_reader)

#endif
