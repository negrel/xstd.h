#ifndef XSTD_BUF_READER_H_INCLUDE
#define XSTD_BUF_READER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>

#ifdef XSTD_IMPLEMENTATION
#include <string.h>
#endif

#include "io/reader.h"

struct xstd_buf_reader_body {
  Reader *reader;
  uint8_t *buf;
  size_t buf_len;
  int read_err;
  size_t read;
  size_t write;
};

// BufReader wraps a Reader
typedef struct {
  Reader reader;
  struct xstd_buf_reader_body body_;
} BufReader;

size_t buf_reader_buffered(BufReader *buf_reader);

#ifdef XSTD_IMPLEMENTATION
size_t buf_reader_buffered(BufReader *buf_reader) {
  return buf_reader->body_.read - buf_reader->body_.write;
}
#endif

#ifdef XSTD_IMPLEMENTATION
static void buf_reader_read(void *body, uint8_t *p, size_t p_len, size_t *n,
                            int *err) {
  // p is empty.
  if (p_len == 0)
    return;

  struct xstd_buf_reader_body *b = (struct xstd_buf_reader_body *)body;

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

void buf_reader_init(BufReader *buf_reader, Reader *r, uint8_t *buf,
                     size_t buf_len);

#ifdef XSTD_IMPLEMENTATION
void buf_reader_init(BufReader *buf_reader, Reader *r, uint8_t *buf,
                     size_t buf_len) {
  *buf_reader = (BufReader){0};
  buf_reader->reader.vtable_ = &buf_reader_vtable;
  buf_reader->reader.offset_ = offsetof(BufReader, body_);

  buf_reader->body_.reader = r;
  buf_reader->body_.buf = buf;
  buf_reader->body_.buf_len = buf_len;
}
#endif

BufReader *buf_reader_new(Allocator *allocator, Reader *r, uint8_t *buf,
                          size_t buf_len);

#ifdef XSTD_IMPLEMENTATION
BufReader *buf_reader_new(Allocator *allocator, Reader *r, uint8_t *buf,
                          size_t buf_len) {
  BufReader *buf_reader = alloc_malloc(allocator, sizeof(BufReader));
  buf_reader_init(buf_reader, r, buf, buf_len);
  return buf_reader;
}
#endif

BufReader buf_reader(Reader *r, uint8_t *buf, size_t buf_len);

#ifdef XSTD_IMPLEMENTATION
BufReader buf_reader(Reader *r, uint8_t *buf, size_t buf_len) {
  BufReader buf_reader = {0};
  buf_reader_init(&buf_reader, r, buf, buf_len);
  return buf_reader;
}
#endif

#endif
