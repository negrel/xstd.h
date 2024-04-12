#ifndef XSTD_IO_READ_WRITER_H_INCLUDE
#define XSTD_IO_READ_WRITER_H_INCLUDE

#include "iface.h"
#include "internal.h"
#include "io/reader.h"
#include "io/writer.h"

// ReadWriter is the interface that groups the basic Read and Write methods.
iface_merge_def(ReadWriter, Reader reader; Writer writer);

// FileReadWriter wraps FILE and implements ReadWriter.
iface_impl_def(ReadWriter, FileReadWriter, FILE *);

#undef type_init_proto
#define type_init_proto FILE *f
#undef type_init_args
#define type_init_args f
#undef type_init
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface.reader, &file_reader_vtable, f);                  \
    iface_impl_init(t, iface.writer, &file_writer_vtable, f);                  \
  }

def_type_constructors(FileReadWriter, file_read_writer)

#endif
