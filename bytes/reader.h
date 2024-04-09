#ifndef XSTD_BYTES_READER_H_INCLUDE
#define XSTD_BYTES_READER_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <string.h>
#endif

#include "bytes/buffer.h"
#include "io/reader.h"

typedef struct {
  Reader reader;
  BytesBuffer *buffer_;
} BytesBufferReader;

#ifdef XSTD_IMPLEMENTATION
static void bytes_buffer_reader_read(void *reader, uint8_t *p, size_t p_len,
                                     size_t *n, int *err) {
  (void)err;

  BytesBuffer *buffer = (BytesBuffer *)reader;
  size_t copied = p_len > buffer->len_ ? buffer->len_ : p_len;
  memcpy(p, buffer->bytes_, copied);
  if (n != NULL)
    *n = copied;
}
#endif

struct xstd_reader_vtable bytes_buffer_reader_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_reader_vtable bytes_buffer_reader_vtable = {
    .read = &bytes_buffer_reader_read,
};
#endif

BytesBufferReader bytes_buffer_reader(BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
BytesBufferReader bytes_buffer_reader(BytesBuffer *buffer) {
  BytesBufferReader bbw = {0};
  bbw.reader.vtable_ = &bytes_buffer_reader_vtable;
  bbw.reader.offset_ = offsetof(BytesBufferReader, buffer_);
  bbw.buffer_ = buffer;
  return bbw;
}
#endif

#endif
