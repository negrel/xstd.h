// XSTD bundle from https://github.com/negrel/xstd.h

#ifdef __clang__
#define typeof __typeof__
#else
#ifndef typeof
#define typeof assert(0 && "typeof macro is not defined")
#endif
#endif

#define type_assert_eq(X, Y)                                                   \
  _Generic((Y), typeof(X): _Generic((X), typeof(Y): (void)NULL))
// Single file header allocator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ALLOC_H_INCLUDE
#define XSTD_ALLOC_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_ALLOC_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif

struct xstd_allocator_vtable {
  void *(*malloc)(void *allocator, size_t size);
  void (*free)(void *allocator, void *ptr);
  void *(*calloc)(void *allocator, size_t nmemb, size_t size);
  void *(*realloc)(void *allocator, void *ptr, size_t newsize);
};

// Allocator interface.
typedef struct {
  struct xstd_allocator_vtable *vtable_;
  size_t offset_;
} Allocator;

// alloc_malloc() allocates size bytes and returns a pointer to the allocated
// memory. The memory is not initialized.
void *alloc_malloc(Allocator *, size_t);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_malloc(Allocator *allocator, size_t size) {
  void *ptr = allocator->vtable_->malloc(
      (void *)((uintptr_t)allocator + allocator->offset_), size);
  assert(ptr != NULL);
  return ptr;
}
#endif

// alloc_free() frees the memory space pointed to by ptr, which must have been
// returned by a previous call to malloc() or related functions.
void alloc_free(Allocator *allocator, void *ptr);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void alloc_free(Allocator *allocator, void *ptr) {
  allocator->vtable_->free((void *)((uintptr_t)allocator + allocator->offset_),
                           ptr);
}
#endif

// The calloc() function allocates memory for an array of nmemb elements of size
// bytes each and returns a pointer to the allocated memory. The memory is set
// to zero.
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_calloc(Allocator *allocator, size_t nmemb, size_t size) {
  void *ptr = allocator->vtable_->calloc(
      (void *)((uintptr_t)allocator + allocator->offset_), nmemb, size);
  assert(ptr != NULL);
  return ptr;
}
#endif

// The realloc() function changes the size of the memory block pointed to by ptr
// to newsize bytes. The contents of the memory will be unchanged in the range
// from the start of the region up to the minimum of the old and new sizes. If
// the new size is larger than the old size, the added memory will not be
// initialized.
void *alloc_realloc(Allocator *allocator, void *ptr, size_t newsize);

#ifdef XSTD_ALLOC_IMPLEMENTATION
void *alloc_realloc(Allocator *allocator, void *ptr, size_t newsize) {
  ptr = allocator->vtable_->realloc(
      (void *)((uintptr_t)allocator + allocator->offset_), ptr, newsize);
  assert(ptr != NULL);
  return ptr;
}
#endif

Allocator *g_libc_allocator;

#ifdef XSTD_ALLOC_IMPLEMENTATION
static void *libc_malloc(void *alloc, size_t size) {
  (void)alloc;
  return malloc(size);
}

static void libc_free(void *alloc, void *ptr) {
  (void)alloc;
  free(ptr);
}

static void *libc_calloc(void *alloc, size_t nmemb, size_t size) {
  (void)alloc;
  void *ptr = calloc(nmemb, size);
  return ptr;
}

static void *libc_realloc(void *alloc, void *ptr, size_t size) {
  (void)alloc;
  return realloc(ptr, size);
}

static struct xstd_allocator_vtable libc_allocator_vtable = {
    .malloc = &libc_malloc,
    .free = &libc_free,
    .calloc = &libc_calloc,
    .realloc = &libc_realloc,
};

static Allocator libc_allocator = {
    .vtable_ = &libc_allocator_vtable,
    .offset_ = 0,
};

Allocator *g_libc_allocator = &libc_allocator;
#endif

#endif
#ifndef XSTD_IO_READER_H_INCLUDE
#define XSTD_IO_READER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IO_READER_IMPLEMENTATION
#include <errno.h>

#define XSTD_ALLOC_IMPLEMENTATION
#endif


struct xstd_reader_vtable {
  void (*read)(void *reader, uint8_t *p, size_t p_len, size_t *n, int *err);
};

// Reader is the interface that wraps the basic read method.
//
// read reads up to len(p) bytes into p. It returns the number of bytes read
// (0 <= n <= p_len) and any error encountered. Even if read returns n < p_len,
// it may use all of p as scratch space during the call. If some data is
// available but not p_len bytes, read conventionally returns what is available
// instead of waiting for more.
//
// When read encounters an error or end-of-file condition after successfully
// reading n > 0 bytes, it returns the number of bytes read. It may return the
// (non-null) error from the same call or return the error (and n == 0) from a
// subsequent call. An instance of this general case is that a Reader returning
// a non-zero number of bytes at the end of the input stream may return either
// err == EOF or err == nil. The next read should return 0, EOF.
//
// Callers should always process the n > 0 bytes returned before considering the
// error err. Doing so correctly handles I/O errors that happen after reading
// some bytes and also both of the allowed EOF behaviors.
//
// If p_len == 0, read should always return n == 0. It may return a non-null
// error if some error condition is known, such as EOF.
//
// Implementations of read are discouraged from returning a zero byte count with
// a nil error, except when len(p) == 0. Callers should treat a return of 0 and
// nil as indicating that nothing happened; in particular it does not indicate
// EOF.
//
// Implementations must not retain p.
typedef struct xstd_reader {
  struct xstd_reader_vtable *vtable_;
  size_t offset_;
} Reader;

// reader_read calls the read method of the reader.
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n, int *err);

#ifdef XSTD_IO_READER_IMPLEMENTATION
void reader_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                 int *err) {
  reader->vtable_->read((void *)((uintptr_t)reader + reader->offset_), p, p_len,
                        n, err);
}
#endif

// FileReader wraps FILE and implements Reader.
typedef struct {
  Reader reader;
  FILE *f_;
} FileReader;

// file_reader_read implements Reader.
#ifdef XSTD_IO_READER_IMPLEMENTATION
static void file_reader_read(void *file, uint8_t *p, size_t p_len, size_t *n,
                             int *error) {
  FILE *f = *(void **)file;
  size_t read = fread(p, sizeof(*p), p_len, f);
  if (n != NULL)
    *n = read;

  if (error != NULL) {
    *error = errno;
    if (read == 0 && feof(f)) {
      *error = EOF;
    }
    clearerr(f);
  }
}
#endif

// FileReader virtual table for Reader interface.
struct xstd_reader_vtable file_reader_vtable;

#ifdef XSTD_IO_READER_IMPLEMENTATION
struct xstd_reader_vtable file_reader_vtable = {.read = &file_reader_read};
#endif

// file_reader_init initializes the given FileReader.
void file_reader_init(FileReader *freader, FILE *f);

#ifdef XSTD_IO_READER_IMPLEMENTATION
void file_reader_init(FileReader *freader, FILE *f) {
  *freader = (FileReader){0};
  freader->reader.vtable_ = &file_reader_vtable;
  freader->reader.offset_ = offsetof(FileReader, f_);
  freader->f_ = f;
}
#endif

// file_reader_new allocates, initializes and returns a new FileReader.
FileReader *file_reader_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IO_READER_IMPLEMENTATION
FileReader *file_reader_new(Allocator *allocator, FILE *f) {
  FileReader *freader = alloc_malloc(allocator, sizeof(FileReader));
  file_reader_init(freader, f);
  return freader;
}
#endif

// file_reader initializes and returns a FileReader that wraps the given file.
FileReader file_reader(FILE *f);

#ifdef XSTD_IO_READER_IMPLEMENTATION
FileReader file_reader(FILE *f) {
  FileReader freader = {0};
  file_reader_init(&freader, f);
  return freader;
}
#endif

#endif
#ifndef XSTD_IO_WRITER_H_INCLUDE
#define XSTD_IO_WRITER_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
#include <errno.h>

#define XSTD_ALLOC_IMPLEMENTATION
#endif


struct xstd_writer_vtable {
  void (*write)(void *writer, uint8_t *p, size_t p_len, size_t *n, int *err);
};

// Writer is the interface that wraps the basic Write method.
//
// Write writes len(p) bytes from p to the underlying data stream. It returns
// the number of bytes written from p (0 <= n <= len(p)) and any error
// encountered that caused the write to stop early. Write must return a non-nil
// error if it returns n < len(p). Write must not modify the slice data, even
// temporarily.
typedef struct {
  struct xstd_writer_vtable *vtable_;
  size_t offset_;
} Writer;

void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
void writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                  int *error) {
  writer->vtable_->write((void *)((uintptr_t)writer + writer->offset_), p,
                         p_len, n, error);
}
#endif

// FileWriter wraps FILE and implements Writer.
typedef struct {
  Writer writer;
  FILE *f_;
} FileWriter;

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
static void file_writer_write(void *file, uint8_t *p, size_t p_len, size_t *n,
                              int *error) {
  FILE *f = *(void **)file;
  size_t write = fwrite(p, sizeof(*p), p_len, f);
  if (n != NULL)
    *n = write;

  if (error != NULL) {
    *error = errno;
    if (write == 0 && feof(f))
      *error = EOF;

    clearerr(f);
  }
}

#endif

// FileWriter virtual table for Writer interface.
struct xstd_writer_vtable file_writer_vtable;

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
struct xstd_writer_vtable file_writer_vtable = {.write = &file_writer_write};
#endif

// file_writer_init initializes the given FileWriter.
void file_writer_init(FileWriter *fw, FILE *f);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
void file_writer_init(FileWriter *fw, FILE *f) {
  *fw = (FileWriter){0};
  fw->writer.vtable_ = &file_writer_vtable;
  fw->writer.offset_ = offsetof(FileWriter, f_);
  fw->f_ = f;
}
#endif

// file_writer_new allocates, initializes and returns a new FileWriter.
FileWriter *file_writer_new(Allocator *allocator, FILE *f);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
FileWriter *file_writer_new(Allocator *allocator, FILE *f) {
  FileWriter *fw = alloc_malloc(allocator, sizeof(FileWriter));
  file_writer_init(fw, f);
  return fw;
}
#endif

// file_writer initializes and returns a FileWriter that wraps the given file.
FileWriter file_writer(FILE *f);

#ifdef XSTD_IO_WRITER_IMPLEMENTATION
FileWriter file_writer(FILE *f) {
  FileWriter fw = {0};
  file_writer_init(&fw, f);
  return fw;
}
#endif

#endif
// Single file header iterator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_ITER_H_INCLUDE
#define XSTD_ITER_H_INCLUDE

#include <stdint.h>
#include <string.h>

typedef struct xstd_iterator {
  void *(*next)(struct xstd_iterator *);
} Iterator;

#define iter_foreach(iter, type, iterator)                                     \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        type value;                                                            \
      } iterator =                                                             \
          {                                                                    \
              .index = 0,                                                      \
             .value = iter_next(iter),                                         \
          };                                                                   \
      iterator.value != NULL;                                                  \
      iterator.index++, iterator.value = iter_next(iter))

void *iter_next(Iterator *);

#ifdef XSTD_ITER_IMPLEMENTATION
void *iter_next(Iterator *iter) { return iter->next(iter); }
#endif

#define range_foreach(iterator, from, to, step)                                \
  for (struct {                                                                \
         short i;                                                              \
         RangeIterator iter;                                                   \
       } tmp = {.i = 0, .iter = range_iter(from, to, step)};                   \
       tmp.i == 0; tmp.i = 1)                                                  \
  iter_foreach((Iterator *)&tmp.iter, size_t *, iterator)

#define range_to_foreach(i, to) range_foreach(i, 0, to, 1)
#define range_from_to_foreach(i, from, to) range_foreach(i, from, to, 1)

typedef struct xstd_range_iter {
  Iterator iterator;
  int64_t value, start, end, step;
} RangeIterator;

void *range_iter_next(Iterator *it);
#ifdef XSTD_ITER_IMPLEMENTATION
void *range_iter_next(Iterator *it) {
  RangeIterator *iter = (RangeIterator *)it;

  iter->value += iter->step;
  if (iter->value >= iter->end) {
    iter->value -= iter->step;
    return NULL;
  }

  return &iter->value;
}
#endif

RangeIterator range_iter(int64_t start, int64_t end, int64_t step);

#ifdef XSTD_ITER_IMPLEMENTATION
RangeIterator range_iter(int64_t start, int64_t end, int64_t step) {
  RangeIterator iter = {0};
  iter.iterator.next = &range_iter_next;
  iter.value = start - step;
  iter.end = end;
  iter.step = step;

  return iter;
}
#endif

#endif
#ifndef XSTD_ARENA_H_INCLUDE
#define XSTD_ARENA_H_INCLUDE

#include <stddef.h>

#ifdef XSTD_ARENA_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#define XSTD_ALLOC_IMPLEMENTATION
#endif


struct xstd_arena_allocator_body {
  Allocator *parent_allocator_;
  size_t arena_size_;
  struct xstd_arena *arena_list_;
  size_t cursor_;
};

// ArenaAllocator is an arena allocator.
typedef struct xstd_arena_allocator {
  Allocator allocator;
  struct xstd_arena_allocator_body body_;
} ArenaAllocator;

struct xstd_arena {
  // Header
  struct xstd_arena *next;

  // Body
  // ...
};

void arena_allocator_init(ArenaAllocator *arena, Allocator *parent,
                          size_t arena_size);

#ifdef XSTD_ARENA_IMPLEMENTATION
static void *arena_calloc(void *b, size_t nmemb, size_t size) {
  struct xstd_arena_allocator_body *arena_body =
      (struct xstd_arena_allocator_body *)b;

  // Detect overflow.
  size_t alloc_size = nmemb * size;
  if (alloc_size / size != nmemb ||
      alloc_size + sizeof(struct xstd_arena) < alloc_size) {
    return NULL;
  }

  if (alloc_size > arena_body->arena_size_) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     alloc_size + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = arena_body->arena_size_; // Allocated arena as full.
    return (void *)((uintptr_t)new_arena + sizeof(struct xstd_arena));
  }

  // Allocate at least size of a pointer to ensure all pointer are aligned.
  alloc_size = alloc_size < sizeof(void *) ? sizeof(void *) : alloc_size;

  // No current arena.
  if (arena_body->arena_list_ == NULL) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     arena_body->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = 0;
  } else if (arena_body->arena_size_ - arena_body->cursor_ < alloc_size) {
    struct xstd_arena *new_arena =
        alloc_calloc(arena_body->parent_allocator_, 1,
                     arena_body->arena_size_ + sizeof(struct xstd_arena));
    if (new_arena == NULL)
      return NULL;

    new_arena->next = arena_body->arena_list_;
    arena_body->arena_list_ = new_arena;
    arena_body->cursor_ = 0;
  }

  void *ptr = (void *)((uintptr_t)arena_body->arena_list_ +
                       sizeof(struct xstd_arena) + arena_body->cursor_);
  arena_body->cursor_ += alloc_size;
  return ptr;
}

static void *arena_malloc(void *allocator, size_t size) {
  return arena_calloc(allocator, 1, size);
}

static void *arena_realloc(void *allocator, void *ptr, size_t size) {
  (void)ptr;
  // TODO: if last alloc, return same pointer.
  // TODO: if new size is smaller, return same pointer.

  return arena_malloc(allocator, size);
}

static void arena_free(void *allocator, void *ptr) {
  (void)allocator;
  (void)ptr;
}

static struct xstd_allocator_vtable arena_allocator_vtable = {
    .malloc = &arena_malloc,
    .free = &arena_free,
    .calloc = &arena_calloc,
    .realloc = &arena_realloc,
};

void arena_allocator_init(ArenaAllocator *arena, Allocator *parent,
                          size_t arena_size) {
  assert(arena_size > sizeof(void *) &&
         "arena size must be greater than size of a pointer");

  arena->body_.parent_allocator_ = parent;
  arena->body_.arena_size_ = arena_size;
  arena->body_.arena_list_ = NULL;
  arena->allocator = (Allocator){0};
  arena->allocator.vtable_ = &arena_allocator_vtable;
  arena->allocator.offset_ = offsetof(ArenaAllocator, body_);
}
#endif

ArenaAllocator arena_allocator(Allocator *parent, size_t arena_size);

#ifdef XSTD_ARENA_IMPLEMENTATION
ArenaAllocator arena_allocator(Allocator *parent, size_t arena_size) {
  ArenaAllocator alloc = {0};
  arena_allocator_init(&alloc, parent, arena_size);
  return alloc;
}
#endif

void arena_alloc_reset(ArenaAllocator *arena);

#ifdef XSTD_ARENA_IMPLEMENTATION
void arena_alloc_reset(ArenaAllocator *arena_alloc) {
  struct xstd_arena **arena = &arena_alloc->body_.arena_list_;
  while (*arena != NULL) {
    struct xstd_arena *a = *arena;
    *arena = a->next;
    alloc_free(arena_alloc->body_.parent_allocator_, a);
  }

  arena_allocator_init(arena_alloc, arena_alloc->body_.parent_allocator_,
                       arena_alloc->body_.arena_size_);
}
#endif

#endif
// Single file header vector (growable array)
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_VEC_H_INCLUDE
#define XSTD_VEC_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>

#ifdef XSTD_VEC_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define XSTD_ALLOC_IMPLEMENTATION
#endif


typedef void *Vec;

struct xstd_vector {
  // Header
  Allocator *allocator_;
  size_t cap_;
  size_t len_;
  size_t elem_size_;

  // Body
  // ...
};

// vec_foreach is a foreach macro for the vector type.
// The first parameter is the vector on which to iterate. The
// second is the name of the iterator. The index is accessible
// with `iterator.index` and the value with `iterator.value`.
#define vec_foreach(vec, iterator)                                             \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(*vec) value;                                                   \
       } iterator = {.value = vec[0]};                                         \
       iterator.index < vec_len(vec); iterator.value = vec[++iterator.index])

// vec_foreach_ptr does the same as vec_foreach
// but the iterator contains a ptr instead of a copy.
#define vec_foreach_ptr(vec, iterator)                                         \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(vec) value;                                                    \
       } iterator = {.value = &vec[0]};                                        \
       iterator.index < vec_len(vec); iterator.value = &vec[++iterator.index])

// vec_push returns a pointer to an element at the end of the vector.
// This function takes a double pointer to a vector to reallocate in case it's
// full.
#define vec_push(vec) ((typeof(*vec))vec_push_((Vec *)vec))

// vec_unshift adds an element at the beginning of the vector and shift
// other elements.
#define vec_unshift(vec) ((typeof(*vec))vec_unshift_((void **)vec))

#define bodyof_vec(vecptr) ((uintptr_t)(vecptr) + sizeof(struct xstd_vector))
#define headerof_vec(vecptr)                                                   \
  ((struct xstd_vector *)((uintptr_t)vecptr - sizeof(struct xstd_vector)))

#ifdef XSTD_VEC_IMPLEMENTATION
static size_t sizeof_vector(struct xstd_vector *v) {
  return sizeof(struct xstd_vector) + v->cap_ * v->elem_size_;
}
#endif

// vec_new returns a new zeroed vector with the given capacity to store
// element of the given size. It the returns a pointer to the first element of
// the vector.
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size);

#ifdef XSTD_VEC_IMPLEMENTATION
Vec vec_new(Allocator *allocator, size_t cap, size_t elem_size) {
  size_t header = sizeof(struct xstd_vector);
  size_t body = cap * elem_size;

  struct xstd_vector *vec = alloc_calloc(allocator, header + body, 1);
  if (vec == NULL)
    return NULL;

  vec->allocator_ = allocator;
  vec->len_ = 0;
  vec->cap_ = cap;
  vec->elem_size_ = elem_size;

  return (void *)bodyof_vec(vec);
}
#endif

// vec_clone creates a clone of the given vector and returns it.
Vec vec_clone(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
Vec vec_clone(const Vec vec) {
  assert(vec != NULL);

  struct xstd_vector *v = headerof_vec(vec);
  size_t size_v = sizeof_vector(v);

  void *clone = alloc_malloc(v->allocator_, size_v);
  if (clone == NULL)
    return NULL;

  memcpy(clone, v, size_v);

  return (void *)bodyof_vec(clone);
}
#endif

// vec_len returns the current len of the vector.
size_t vec_len(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
size_t vec_len(const Vec vec) {
  assert(vec != NULL);

  return headerof_vec(vec)->len_;
}
#endif

// vec_cap returns the current capacity of the vector.
size_t vec_cap(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
size_t vec_cap(const Vec vec) {
  assert(vec != NULL);

  return headerof_vec(vec)->cap_;
}
#endif

// vec_isfull returns true if the vector is full.
bool vec_isfull(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
bool vec_isfull(const Vec vec) {
  assert(vec != NULL);

  return vec_len(vec) == vec_cap(vec);
}
#endif

// vec_isempty returns true if the vector size is 0.
bool vec_isempty(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
bool vec_isempty(const Vec vec) {
  assert(vec != NULL);

  return vec_len(vec) == 0;
}
#endif

// vec_push_ returns a pointer to an element at the end of the vector.
// This function takes a pointer to a vector to reallocate it if it's full.
// An handy vec_push macro exists so you don't have to cast arguments and
// return type.
void *vec_push_(Vec *);

#ifdef XSTD_VEC_IMPLEMENTATION
void *vec_push_(void **vec) {
  assert(vec != NULL);
  assert(*vec != NULL);

  if (vec_isfull(*vec)) {
    // Let's double the capacity of our vector
    struct xstd_vector *old = headerof_vec(*vec);

    void *new = vec_new(old->allocator_, old->cap_ * 2, old->elem_size_);
    headerof_vec(new)->len_ = old->len_;

    memcpy(new, (void *)bodyof_vec(old), old->cap_ * old->elem_size_);
    alloc_free(old->allocator_, old);

    *vec = new;
  }

  struct xstd_vector *v = headerof_vec(*vec);
  size_t offset = v->elem_size_ * v->len_;

  v->len_++;

  return (void *)(bodyof_vec(v) + offset);
}
#endif

// vec_pop removes the last element of the vector and store it in popped
// if not null.
void vec_pop(Vec, void *);

#ifdef XSTD_VEC_IMPLEMENTATION

void vec_pop(Vec vec, void *popped) {
  assert(vec != NULL);

  if (vec_isempty(vec))
    return;

  struct xstd_vector *v = headerof_vec(vec);
  v->len_--;

  if (popped != NULL)
    memcpy(popped, (void *)((uintptr_t)vec + v->len_ * v->elem_size_),
           v->elem_size_);
}
#endif

// vec_shift removes the first element of the vector and store it shifted
// if not null. Remaining elements are copied to their index - 1.
void vec_shift(Vec, void *);

#ifdef XSTD_VEC_IMPLEMENTATION
void vec_shift(Vec vec, void *shifted) {
  assert(vec != NULL);

  if (vec_isempty(vec))
    return;

  struct xstd_vector *v = headerof_vec(vec);
  if (shifted != NULL)
    memcpy(shifted, vec, v->elem_size_);

  v->len_--;

  memmove((void *)bodyof_vec(v),
          (void *)((uintptr_t)bodyof_vec(v) + v->elem_size_),
          v->len_ * v->elem_size_);
}
#endif

// vec_unshift_ adds an element at the beginning of the vector and shift
// other elements.
// An handy vec_unshift macro exists so you don't have to cast arguments and
// return type.
void *vec_unshift_(Vec *);

#ifdef XSTD_VEC_IMPLEMENTATION

void *vec_unshift_(void **vec) {
  assert(vec != NULL);
  assert(*vec != NULL);

  if (vec_isfull(*vec)) {
    // Let's double the capacity of our vector
    struct xstd_vector *old = headerof_vec(*vec);

    Vec new = vec_new(old->allocator_, old->cap_ * 2, old->elem_size_);
    headerof_vec(new)->len_ = old->len_ + 1;

    // Copy old elements in new at index 1
    memcpy((void *)((uintptr_t) new + old->elem_size_), (void *)bodyof_vec(old),
           old->cap_ * old->elem_size_);
    alloc_free(old->allocator_, old);

    *vec = new;

    return new;
  }

  struct xstd_vector *v = headerof_vec(*vec);
  // Move all elements by 1
  memmove((void *)((uintptr_t)*vec + v->elem_size_), *vec,
          v->len_ * v->elem_size_);
  v->len_++;

  return *vec;
}
#endif

// vec_free free the given vector.
void vec_free(const Vec);

#ifdef XSTD_VEC_IMPLEMENTATION
void vec_free(Vec vec) {
  assert(vec != NULL);

  struct xstd_vector *v = headerof_vec(vec);
  free(v);
}
#endif

#define vec_iter_foreach(vec, iterator)                                        \
  for (struct {                                                                \
         short i;                                                              \
         VecIterator iter;                                                     \
       } tmp = {.i = 0, .iter = vec_iter(vec)};                                \
       tmp.i == 0; tmp.i = 1)                                                  \
  iter_foreach((Iterator *)&tmp.iter, typeof(vec), iterator)

// VecIterator is a wrapper around Vec that implements the Iterator interface.
// VecIterator pointer can safely be casted to Iterator *.
typedef struct {
  Iterator iterator_;
  size_t cursor_;
  Vec vec_;
} VecIterator;

// vec_iter_next implements Iterator interface for VecIterator.
void *vec_iter_next(Iterator *it);

#ifdef XSTD_VEC_IMPLEMENTATION
void *vec_iter_next(Iterator *it) {
  if (it == NULL)
    return NULL;

  VecIterator *iter = (VecIterator *)it;
  void *result = NULL;

  struct xstd_vector *vec = headerof_vec(iter->vec_);
  if (iter->cursor_ < vec->len_) {
    result =
        (void *)((uintptr_t)(iter->vec_) + iter->cursor_ * vec->elem_size_);
    iter->cursor_++;
  }

  return result;
}
#endif

// vec_iter creates a VecIterator that wraps the given vector.
VecIterator vec_iter(const Vec vec);

#ifdef XSTD_VEC_IMPLEMENTATION
VecIterator vec_iter(const Vec vec) {
  VecIterator iterator = {0};
  iterator.iterator_.next = &vec_iter_next;
  iterator.cursor_ = 0;
  iterator.vec_ = vec;

  return iterator;
}
#endif

#endif
#ifndef XSTD_LIST_H_INCLUDE
#define XSTD_LIST_H_INCLUDE

#ifdef XSTD_LIST_IMPLEMENTATION
#define XSTD_ITER_IMPLEMENTATION

#include <stddef.h>
#include <stdint.h>
#endif


// typedef a List structure with the given name and of the given type.
#define typedef_list(type, name)                                               \
  typedef struct _xstd_##name {                                                \
    struct name *next_;                                                        \
    type value;                                                                \
  } name

#define list_prepend(list, element)                                            \
  do {                                                                         \
    type_assert_eq(*(list), element);                                          \
    list_prepend_((void **)list, (void *)element);                             \
  } while (0)

// list_prepend_ adds element to beginning of list, replacing its current head.
// An handy list_prepend macro exists so you don't have to cast arguments.
void list_prepend_(void **list, void *element);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_prepend_(void **list, void *element) {
  void **element_next = (void **)element;
  *element_next = *list;
  *list = element;
}
#endif

#define list_next(list) ((typeof(list))list_next_(list))

void *list_next_(void *list);

#ifdef XSTD_LIST_IMPLEMENTATION
void *list_next_(void *list) {
  if (list == NULL)
    return NULL;
  return *((void **)list);
}
#endif

#define list_remove(list, element)                                             \
  do {                                                                         \
    type_assert_eq(list, element);                                             \
    list_remove_((void *)list, (void *)element);                               \
  } while (0)

// list_remove_ removes element from list but leave element untouched.
// An handy list_remove macro exists so you don't have to cast arguments.
void list_remove_(void *list, void *element);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_remove_(void *list, void *element) {
  if (element == NULL)
    return;

  void **list_next = (void **)list;

  while (*list_next != NULL && *list_next != element) {
    list_next = (void **)*list_next;
  }

  *list_next = list_next_(element);
}
#endif

#define list_remove_next(list) (typeof(list))list_remove_next_(list)

// list_remove_next_ removes next element of the given list without altering the
// next element.
void *list_remove_next_(void *list);

#ifdef XSTD_LIST_IMPLEMENTATION
void *list_remove_next_(void *list) {
  void **list_next = (void **)list;
  void *next = *list_next;
  *list_next = list_next_(next);

  return next;
}
#endif

void *list_iter_next(Iterator *iterator);

#ifdef XSTD_LIST_IMPLEMENTATION
void *list_iter_next(Iterator *iterator) {
  // 2nd field of list iterator (the list itself).
  void **iterator_list_field_ptr =
      (void **)((uintptr_t)iterator + sizeof(Iterator));

  void *next = *iterator_list_field_ptr;
  *iterator_list_field_ptr = list_next_(*iterator_list_field_ptr);

  return next;
}
#endif

#define typedef_list_iterator(list_type, type_name)                            \
  typedef struct {                                                             \
    Iterator iterator;                                                         \
    list_type *list;                                                           \
  } type_name

#define fndef_list_iterator_init(list_iter_type, list_type, fn_name)           \
  list_iter_type fn_name(list_type *list);                                     \
  list_iter_type fn_name(list_type *list) {                                    \
    list_iter_type iter = {0};                                                 \
    iter.iterator.next = &list_iter_next;                                      \
    iter.list = list;                                                          \
    return iter;                                                               \
  }

#endif
