#ifndef XSTD_CHAR_ITER_H_INCLUDE
#define XSTD_CHAR_ITER_H_INCLUDE

#include <stddef.h>
#include <stdint.h>

#include "io/reader.h"
#include "iter.h"

struct xstd_reader_iter_body {
  Reader *reader;
  size_t cursor;
  uint8_t *buf;
  size_t buf_len;
};

typedef struct {
  Iterator iterator;
  struct xstd_reader_iter_body body_;
} ReaderIterator;

struct xstd_iterator_vtable reader_iterator_vtable;

#ifdef XSTD_IMPLEMENTATION
static void *reader_iterator_next(void *body) {
  struct xstd_reader_iter_body *b = (struct xstd_reader_iter_body *)body;

  if (b->cursor >= b->buf_len) {
    b->cursor = 0;
    size_t read = 0;
    int error = 0;
    reader_read(b->reader, b->buf, b->buf_len, &read, &error);
    if (error != 0 || read == 0) {
      return NULL;
    }
  }

  uint8_t *c = &b->buf[b->cursor];
  b->cursor++;

  return c;
}

struct xstd_iterator_vtable reader_iterator_vtable = {
    .next = &reader_iterator_next,
};
#endif

void reader_iterator_init(ReaderIterator *char_iter, Reader *reader,
                          uint8_t *buf, size_t buf_len);

#ifdef XSTD_IMPLEMENTATION
void reader_iterator_init(ReaderIterator *char_iter, Reader *reader,
                          uint8_t *buf, size_t buf_len) {
  *char_iter = (ReaderIterator){0};
  char_iter->iterator.vtable_ = &reader_iterator_vtable;
  char_iter->iterator.offset_ = offsetof(ReaderIterator, body_);

  char_iter->body_.reader = reader;
  char_iter->body_.cursor = buf_len;
  char_iter->body_.buf = buf;
  char_iter->body_.buf_len = buf_len;
}
#endif

#endif
