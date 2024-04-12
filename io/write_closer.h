#ifndef XSTD_IO_WRITE_CLOSER_H_INCLUDE
#define XSTD_IO_WRITE_CLOSER_H_INCLUDE

#include "constructor.h"
#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#endif

#include "iface.h"
#include "io/closer.h"
#include "io/writer.h"

// WriteCloser is the interface that groups the basic Write and Close methods.
iface_merge_def(WriteCloser, Writer writer; Closer closer);

// FileWriteCloser wraps FILE and implements WriteCloser.
iface_impl_def(WriteCloser, FileWriteCloser, FILE *);

// file_write_closer_init initializes the given FileWriteCloser.
void file_write_closer_init(FileWriteCloser *frw, FILE *f);

#undef type_init_proto
#undef type_init_args
#undef type_init
#define type_init_proto FILE *f
#define type_init_args f
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface.writer, &file_writer_vtable, f);                  \
    iface_impl_init(t, iface.closer, &file_closer_vtable, f);                  \
  }

def_type_constructors(FileWriteCloser, file_write_closer)

#endif
