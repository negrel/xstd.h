#ifndef XSTD_BUF_READER_H_INCLUDE
#define XSTD_BUF_READER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>

#ifdef XSTD_IMPLEMENTATION
#include <string.h>
#endif

#include "constructor.h"
#include "iface.h"
#include "io/reader.h"

// BufReader wraps a Reader
iface_impl_def(
    Reader, BufReader, struct {
      Reader *reader;
      uint8_t *buf;
      size_t buf_len;
      int read_err;
      size_t read;
      size_t write;
    });

size_t buf_reader_buffered(BufReader *buf_reader);

#ifdef XSTD_IMPLEMENTATION
size_t buf_reader_buffered(BufReader *buf_reader) {
  return buf_reader->body_.read - buf_reader->body_.write;
}
#endif

#ifdef XSTD_IMPLEMENTATION
static void buf_reader_read(InterfaceImpl impl, uint8_t *p, size_t p_len,
                            size_t *n, int *err) {
  // p is empty.
  if (p_len == 0)
    return;

  typeof_iface_impl(BufReader) *b = cast_iface_impl_ptr(BufReader, impl);

  size_t buffered = b->read - b->write;

  // No buffered data.
  if (buffered <= 0) {
    if (b->read_err != 0) {
      if (n != NULL)
        *n = 0;
      if (err != NULL)
        *err = b->read_err;
      return;
    }

    // p is larger than internal buffer,
    // read directly into p to avoid copy.
    if (p_len > b->buf_len) {
      reader_read(b->reader, p, p_len, n, err);
      return;
    }

    // Reset cursors.
    b->read = 0;
    b->write = 0;
    // Read into internal buffer.
    reader_read(b->reader, b->buf, b->buf_len, &b->read, &b->read_err);
    if (b->read == 0) {
      if (err != NULL)
        *err = b->read_err;
      return;
    }
    buffered = b->read;
  }

  size_t copied = buffered < p_len ? buffered : p_len;
  memcpy(p, &b->buf[b->write], copied);
  b->write += copied;

  if (n != NULL)
    *n = copied;
}
#endif

struct xstd_reader_vtable buf_reader_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_reader_vtable buf_reader_vtable = {.read = &buf_reader_read};
#endif

#undef type_init_proto
#undef type_init_args
#undef type_init
#define type_init_proto Reader *r, uint8_t *buf, size_t buf_len
#define type_init_args r, buf, buf_len
#define type_init                                                              \
  {                                                                            \
    iface_impl_init(t, iface, &buf_reader_vtable,                              \
                    ((typeof_iface_impl(BufReader)){                           \
                        .reader = r,                                           \
                        .buf = buf,                                            \
                        .buf_len = buf_len,                                    \
                    }));                                                       \
  }

def_type_constructors(BufReader, buf_reader)

#endif
