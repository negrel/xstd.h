#ifndef XSTD_SLICE_H_INCLUDE
#define XSTD_SLICE_H_INCLUDE

#include <stddef.h>
#include <stdint.h>

#ifdef XSTD_IMPLEMENTATION
#include <assert.h>
#endif

// Slice define a slice/portion of a buffer. Slice doesn't own memory it
// contains.
typedef struct {
  uint8_t *const data;
  const size_t len;
} Slice;

size_t slice_len(Slice *slice);

#ifdef XSTD_IMPLEMENTATION
size_t slice_len(Slice *slice) { return slice->len; }
#endif

Slice slice(uint8_t *data, size_t length);

#ifdef XSTD_IMPLEMENTATION
Slice slice(uint8_t *data, size_t length) {
  Slice s = {.data = data, .len = length};
  return s;
}
#endif

#endif
