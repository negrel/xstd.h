#ifndef XSTD_IO_WRITER_H_INCLUDE
#define XSTD_IO_WRITER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#endif

#include "constructor.h"
#include "iface.h"

// Writer is the interface that wraps the basic Write method.
//
// Write writes len(p) bytes from p to the underlying data stream. It returns
// the number of bytes written from p (0 <= n <= len(p)) and any error
// encountered that caused the write to stop early. Write must return a non-nil
// error if it returns n < len(p). Write must not modify the slice data, even
// temporarily.
iface_def(
    Writer, struct xstd_writer_vtable {
      void (*write)(InterfaceImpl writer, uint8_t *p, size_t p_len, size_t *n,
                    int *err);
    });

void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error);

#ifdef XSTD_IMPLEMENTATION
void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error) {
  iface_call(writer, write, p, p_len, n, error);
}
#endif

// FileWriter wraps FILE and implements Writer.
iface_impl_def(Writer, FileWriter, FILE *);

#ifdef XSTD_IMPLEMENTATION
static void file_writer_write(InterfaceImpl impl, uint8_t *p, size_t p_len,
                              size_t *n, int *error) {
  typeof_iface_impl(FileWriter) f = cast_iface_impl(FileWriter, impl);

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

// Generate file_writer_init, file_writer_new and file_writer constructors.

#undef type_init_proto
#define type_init_proto FILE *f
#undef type_init_args
#define type_init_args f
#undef type_init
#define type_init                                                              \
  { iface_impl_init(t, iface, &file_writer_vtable, f); }

def_type_constructors(FileWriter, file_writer)

#endif
