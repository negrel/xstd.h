#ifndef XSTD_IO_READ_CLOSER_H_INCLUDE
#define XSTD_IO_READ_CLOSER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#endif

#include "io/closer.h"
#include "io/reader.h"

// ReadCloser is the interface that groups the basic Read and Close methods.
iface_merge_def(ReadCloser, Reader reader; Closer closer);

// FileReadCloser wraps FILE and implements ReadCloser.
iface_impl_def(ReadCloser, FileReadCloser, FILE *);

#undef type_init_proto
#define type_init_proto FILE *f
#undef type_init_args
#define type_init_args f
#undef type_init
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface.reader, &file_reader_vtable, f);                  \
    iface_impl_init(t, iface.closer, &file_closer_vtable, f);                  \
  }

def_type_constructors(FileReadCloser, file_read_closer)

#endif
