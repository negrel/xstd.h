#ifndef XSTD_LIST_H_INCLUDE
#define XSTD_LIST_H_INCLUDE

#ifdef XSTD_IMPLEMENTATION
#include <stddef.h>
#include <stdint.h>
#endif

#include "internal.h"
#include "iter.h"

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

#ifdef XSTD_IMPLEMENTATION
void list_prepend_(void **list, void *element) {
  void **element_next = (void **)element;
  *element_next = *list;
  *list = element;
}
#endif

#define list_next(list) ((typeof(list))list_next_(list))

void *list_next_(void *list);

#ifdef XSTD_IMPLEMENTATION
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

#ifdef XSTD_IMPLEMENTATION
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

#ifdef XSTD_IMPLEMENTATION
void *list_remove_next_(void *list) {
  void **list_next = (void **)list;
  void *next = *list_next;
  *list_next = list_next_(next);

  return next;
}
#endif

void *list_iter_next(void *list);

#ifdef XSTD_IMPLEMENTATION
void *list_iter_next(void *list) {
  // 2nd field of list iterator (the list itself).
  void **iterator_list_field_ptr = (void **)list;

  void *next = *iterator_list_field_ptr;
  *iterator_list_field_ptr = list_next_(*iterator_list_field_ptr);

  return next;
}
#endif

struct xstd_iterator_vtable list_iterator_vtable;

#ifdef XSTD_IMPLEMENTATION
struct xstd_iterator_vtable list_iterator_vtable = {
    .next = &list_iter_next,
};
#endif

#define typedef_list_iterator(list_type, type_name)                            \
  typedef struct {                                                             \
    Iterator iterator;                                                         \
    list_type *list_;                                                          \
  } type_name

#define fndef_list_iterator_init(list_iter_type, list_type, fn_name)           \
  list_iter_type fn_name(list_type *list);                                     \
  list_iter_type fn_name(list_type *list) {                                    \
    list_iter_type iter = {0};                                                 \
    iter.iterator.vtable_ = &list_iterator_vtable;                             \
    iter.iterator.offset_ = offsetof(list_iter_type, list_);                   \
    iter.list_ = list;                                                         \
    return iter;                                                               \
  }

#endif
