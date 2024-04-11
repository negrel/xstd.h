#ifndef XSTD_BYTES_BUFFER_H_INCLUDE
#define XSTD_BYTES_BUFFER_H_INCLUDE

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define XSTD_IMPLEMENTATION
#ifdef XSTD_IMPLEMENTATION
#include <assert.h>
#endif

#include "alloc.h"
#include "bytes/slice.h"
#include "internal.h"

// BytesBuffer define a resizable bytes array that owns its memory.
typedef struct {
  uint8_t *bytes_;
  Allocator *allocator_;
  size_t len_;
  size_t cap_;
} BytesBuffer;

size_t bytes_buffer_capacity(const BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
size_t bytes_buffer_capacity(const BytesBuffer *buffer) { return buffer->cap_; }
#endif

size_t bytes_buffer_length(const BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
size_t bytes_buffer_length(const BytesBuffer *buffer) { return buffer->len_; }
#endif

size_t bytes_buffer_available(const BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
size_t bytes_buffer_available(const BytesBuffer *buffer) {
  return buffer->cap_ - buffer->len_;
}
#endif

void bytes_buffer_reset(BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
void bytes_buffer_reset(BytesBuffer *buffer) { buffer->len_ = 0; }
#endif

Slice bytes_buffer_slice(const BytesBuffer *buffer, size_t start, size_t end);

#ifdef XSTD_IMPLEMENTATION
Slice bytes_buffer_slice(const BytesBuffer *buffer, size_t start, size_t end) {
  assert(buffer->len_ >= start);
  assert(buffer->len_ < end);
  assert(end >= start);

  return slice(&buffer->bytes_[start], end - start);
}
#endif

Slice bytes_buffer_bytes(const BytesBuffer *buffer);

#ifdef XSTD_IMPLEMENTATION
Slice bytes_buffer_bytes(const BytesBuffer *buffer) {
  return slice(buffer->bytes_, buffer->len_);
}
#endif

bool bytes_buffer_resize(BytesBuffer *buffer, size_t new_capacity);

#ifdef XSTD_IMPLEMENTATION
bool bytes_buffer_resize(BytesBuffer *buffer, size_t new_capacity) {
  uint8_t *bytes =
      alloc_realloc(buffer->allocator_, buffer->bytes_, new_capacity);
  if (bytes == NULL)
    return false;

  buffer->bytes_ = bytes;
  buffer->cap_ = new_capacity;

  if (buffer->cap_ < buffer->len_)
    buffer->len_ = buffer->cap_;
  return true;
}
#endif

#define bytes_buffer_append(buffer, data)                                      \
  bytes_buffer_append_bytes(buffer, data, sizeof(typeof(*data)))

#define bytes_buffer_append_char(buffer, data)                                 \
  bytes_buffer_append_bytes(buffer, data, sizeof(char))

size_t bytes_buffer_append_bytes(BytesBuffer *buffer, void *data, size_t size);

#ifdef XSTD_IMPLEMENTATION
size_t bytes_buffer_append_bytes(BytesBuffer *buffer, void *data, size_t size) {
  // Resize if needed.
  bool need_resize = bytes_buffer_available(buffer) < size;
  while (bytes_buffer_available(buffer) < size) {
    if (buffer->cap_ == 0)
      buffer->cap_ = 1;
    else
      buffer->cap_ *= 2;
  }
  if (need_resize)
    if (!bytes_buffer_resize(buffer, buffer->cap_))
      return 0;

  memcpy(&buffer->bytes_[buffer->len_], data, size);
  buffer->len_ += size;

  return size;
}
#endif

void bytes_buffer_fill(BytesBuffer *buffer, int c);

#ifdef XSTD_IMPLEMENTATION
void bytes_buffer_fill(BytesBuffer *buffer, int c) {
  memset(buffer->bytes_, c, buffer->cap_);
  buffer->len_ = buffer->cap_;
}
#endif

#define bytes_buffer_get(buffer, index, type)                                  \
  *(type *)(bytes_buffer_get_(buffer, index, sizeof(type)))

#define bytes_buffer_get_ptr(buffer, index, type)                              \
  ((type *)(bytes_buffer_get_(buffer, index, sizeof(type))))

void *bytes_buffer_get_(const BytesBuffer *buffer, size_t index,
                        size_t elem_size);

#ifdef XSTD_IMPLEMENTATION
void *bytes_buffer_get_(const BytesBuffer *buffer, size_t index,
                        size_t elem_size) {
  assert(index * elem_size < buffer->len_);
  return (void *)&buffer->bytes_[index * elem_size];
}
#endif

void bytes_buffer_init(BytesBuffer *buffer, Allocator *allocator);

#ifdef XSTD_IMPLEMENTATION
void bytes_buffer_init(BytesBuffer *buffer, Allocator *allocator) {
  *buffer = (BytesBuffer){0};
  buffer->allocator_ = allocator;
}
#endif

BytesBuffer *bytes_buffer_new(Allocator *allocator);

#ifdef XSTD_IMPLEMENTATION
BytesBuffer *bytes_buffer_new(Allocator *allocator) {
  BytesBuffer *buf = alloc_malloc(allocator, sizeof(BytesBuffer));
  bytes_buffer_init(buf, allocator);
  return buf;
}
#endif

BytesBuffer bytes_buffer(Allocator *allocator);

#ifdef XSTD_IMPLEMENTATION
BytesBuffer bytes_buffer(Allocator *allocator) {
  BytesBuffer buf = {0};
  bytes_buffer_init(&buf, allocator);
  return buf;
}
#endif

void bytes_buffer_deinit(BytesBuffer *buf);

#ifdef XSTD_IMPLEMENTATION
void bytes_buffer_deinit(BytesBuffer *buf) {
  alloc_free(buf->allocator_, buf->bytes_);
}
#endif

void bytes_buffer_free(BytesBuffer *buf);

#ifdef XSTD_IMPLEMENTATION
void bytes_buffer_free(BytesBuffer *buf) {
  if (buf == NULL)
    return;

  bytes_buffer_deinit(buf);
  alloc_free(buf->allocator_, buf);
}
#endif

#endif
