#ifndef XSTD_BYTES_WRITER_H_INCLUDE
#define XSTD_BYTES_WRITER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <errno.h>
#include <stddef.h>
#endif

#include "bytes/buffer.h"
#include "io/writer.h"

typedef struct {
  Writer writer;
  BytesBuffer *buffer_;
} BytesBufferWriter;

#ifdef XSTD_IMPLEMENTATION
static void bytes_buffer_writer_write(void *writer, uint8_t *p, size_t p_len,
                                      size_t *n, int *err) {

  BytesBuffer *buffer = (BytesBuffer *)writer;
  size_t appended = bytes_buffer_append_bytes(buffer, p, p_len);
  if (n != NULL)
    *n = appended;

  if (err != NULL && appended != p_len)
    *err = ENOMEM;
}
#endif

struct xstd_writer_vtable bytes_buffer_writer_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_writer_vtable bytes_buffer_writer_vtable = {
    .write = &bytes_buffer_writer_write,
};
#endif

BytesBufferWriter bytes_buffer_writer(BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
BytesBufferWriter bytes_buffer_writer(BytesBuffer *buffer) {
  BytesBufferWriter bbw = {0};
  bbw.writer.vtable_ = &bytes_buffer_writer_vtable;
  bbw.writer.offset_ = offsetof(BytesBufferWriter, buffer_);
  bbw.buffer_ = buffer;
  return bbw;
}
#endif

#endif
