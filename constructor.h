#ifndef XSTD_CONSTRUCTOR_H_INCLUDE
#define XSTD_CONSTRUCTOR_H_INCLUDE

// Used by def_type_constructors macros.
#include <assert.h>

// Used by def_type_constructors macros.
#include "alloc.h"

#define type_init_proto
#define type_init_args
#define type_init                                                              \
  {}

#ifdef XSTD_IMPLEMENTATION
#define def_type_constructors(type, type_snake)                                \
  void type_snake##_init(type *t, type_init_proto);                            \
  void type_snake##_init(type *t, type_init_proto) {                           \
    assert(t != NULL);                                                         \
    *t = (type){0};                                                            \
    type_init                                                                  \
  }                                                                            \
                                                                               \
  type *type_snake##_new(Allocator *allocator, type_init_proto);               \
  type *type_snake##_new(Allocator *allocator, type_init_proto) {              \
    type *t = alloc_malloc(allocator, sizeof(type));                           \
    type_snake##_init(t, type_init_args);                                      \
    return t;                                                                  \
  }                                                                            \
                                                                               \
  type type_snake(type_init_proto);                                            \
  type type_snake(type_init_proto) {                                           \
    type t = {0};                                                              \
    type_snake##_init(&t, type_init_args);                                     \
    return t;                                                                  \
  }
#else
#define def_type_constructors(type, type_snake)                                \
  void type_snake##_init(type *t, type_init_proto);                            \
                                                                               \
  type *type_snake##_new(Allocator *allocator, type_init_proto);               \
                                                                               \
  type type_snake(type_init_proto);
#endif

#ifdef XSTD_IMPLEMENTATION
#define def_type_constructors_no_args(type, type_snake)                        \
  void type_snake##_init(type *t);                                             \
  void type_snake##_init(type *t) {                                            \
    assert(t != NULL);                                                         \
    *t = (type){0};                                                            \
    type_init                                                                  \
  }                                                                            \
                                                                               \
  type *type_snake##_new(Allocator *allocator);                                \
  type *type_snake##_new(Allocator *allocator) {                               \
    type *t = alloc_malloc(allocator, sizeof(type));                           \
    type_snake##_init(t);                                                      \
    return t;                                                                  \
  }                                                                            \
                                                                               \
  type type_snake(void);                                                       \
  type type_snake(void) {                                                      \
    type t = {0};                                                              \
    type_snake##_init(&t);                                                     \
    return t;                                                                  \
  }
#else
#define def_type_constructors_no_args(type, type_snake)                        \
  void type_snake##_init(type *t);                                             \
                                                                               \
  type *type_snake##_new(Allocator *allocator);                                \
                                                                               \
  type type_snake(void);
#endif

#endif
