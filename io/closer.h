#ifndef XSTD_IO_CLOSER_H_INCLUDE
#define XSTD_IO_CLOSER_H_INCLUDE

#include <stdbool.h>
#include <stdio.h>

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#include <stddef.h>
#endif

#include "constructor.h"
#include "iface.h"

// Closer interface / virtual table. Closer is the interface that wraps basic
// Close method. The behavior of Close after the first call is undefined.
// Specific implementations may document their own behavior.
iface_def(
    Closer, struct xstd_closer_vtable {
      void (*close)(InterfaceImpl closer, int *error);
    });

// closer_close calls the close method of the closer.
void closer_close(Closer *closer, int *error);

#ifdef XSTD_IMPLEMENTATION
void closer_close(Closer *closer, int *error) {
  iface_call(closer, close, error);
}
#endif

// NopCloser is a no-op Closer implementation.
iface_impl_def_empty(Closer, NopCloser);

#ifdef XSTD_IMPLEMENTATION
static void nop_closer_close(void *_, int *error) {
  (void)_;
  (void)error;
}
#endif

struct xstd_closer_vtable nop_closer_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_closer_vtable nop_closer_vtable = {.close = &nop_closer_close};
#endif

// Generate nop_closer_init, nop_closer_new and nop_closer constructors.

#undef type_init
#define type_init                                                              \
  { iface_empty_impl_init(t, iface, &nop_closer_vtable) }

def_type_constructors_no_args(NopCloser, nop_closer)

    // FileCloser wraps FILE and implements the Closer interface.
    iface_impl_def(Closer, FileCloser, FILE *);

#ifdef XSTD_IMPLEMENTATION
static void file_closer_close(InterfaceImpl impl, int *error) {
  typeof_iface_impl(FileCloser) f = cast_iface_impl(FileCloser, impl);
  if (!fclose(f) && error != NULL)
    *error = errno;
}
#endif

struct xstd_closer_vtable file_closer_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_closer_vtable file_closer_vtable = {.close = &file_closer_close};
#endif

// Generate file_closer_init, file_closer_new and file_closer constructors.

#undef type_init_proto
#undef type_init_args
#undef type_init
#define type_init_proto FILE *f
#define type_init_args f
#define type_init                                                              \
  { iface_impl_init(t, iface, &file_closer_vtable, f) }

def_type_constructors(FileCloser, file_closer)

#endif
