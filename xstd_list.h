// Single file header iterator interface / vtable
// from xstd (https://github.com/negrel/xstd.h).

#ifndef XSTD_LIST_H_INCLUDE
#define XSTD_LIST_H_INCLUDE

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "xstd_internal.h"

#ifdef XSTD_LIST_IMPLEMENTATION
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#endif

typedef void *List;

struct xstd_list {
  // Header
  struct xstd_list *next;

  // Body
  // ...
};

#ifdef XSTD_LIST_IMPLEMENTATION
// private macros
#define bodyof_list(listptr) ((uintptr_t)(listptr) + sizeof(struct xstd_list))
#define headerof_list(listptr)                                                 \
  ((struct xstd_list *)((uintptr_t)listptr - sizeof(struct xstd_list)))
#endif

#define list_foreach(list, iterator)                                           \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        typeof(*list) value;                                                   \
        typeof(list) item;                                                     \
      } iterator = {.value = *list, .item = list};                             \
      iterator.item != NULL; iterator.item = list_next(iterator.item),         \
        iterator.value = iterator.item != NULL ? *iterator.item                \
                                               : (typeof(*list)){0})

#define list_foreach_ptr(list, iterator)                                       \
  for (struct {                                                                \
         size_t index;                                                         \
         typeof(list) value;                                                   \
       } iterator = {.value = list};                                           \
       iterator.value != NULL; iterator.value = list_next(iterator.value))

#define list_push(list)                                                        \
  ((typeof(*list))_list_push((void **)list, sizeof(typeof(*list))))

void *_list_push(List *, size_t);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_push(void **list, size_t item_size) {
  assert(list != NULL);

  void *head =
      (void *)bodyof_list(calloc(1, sizeof(struct xstd_list) + item_size));

  headerof_list(head)->next = headerof_list(*list);

  *list = head;

  return head;
}
#endif

#define list_next(list) (typeof(list))_list_next(list)

void *_list_next(const List);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_next(const List l) {
  if (l == NULL)
    return NULL;

  return (void *)bodyof_list(headerof_list(l)->next);
}
#endif

#define list_remove_head(list) _list_remove_head((void **)list)

// list_remove_head removes head element from the list. You must not call this
// fuction with an element in a middle of a list otherwise you will encounter a
// use after free error (segfault).
void _list_remove_head(List *);

#ifdef XSTD_LIST_IMPLEMENTATION
void _list_remove_head(void **l) {
  if (*l == NULL)
    return;

  void *tmp = headerof_list(*l);
  *l = list_next(*l);
  free(tmp);
}
#endif

// list_remove_next removes next element from the given list.
void list_remove_next(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_remove_next(List l) {
  if (l == NULL)
    return;

  List next = list_next(l);
  if (next != NULL) {
    struct xstd_list *list = headerof_list(l);
    list->next = headerof_list(next)->next;
    free(headerof_list(next));
  }
}
#endif

// list_free frees all elements contained in the list. You must not call this
// fuction with an element in a middle of a list otherwise you will encounter a
// use after free error (segfault).
void list_free(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_free(List l) {
  if (l == NULL)
    return;

  while (l != NULL) {
    List tmp = l;
    l = list_next(l);
    free(headerof_list(tmp));
  }
}
#endif

// list_free_next frees all elements following the given list.
void list_free_next(List);

#ifdef XSTD_LIST_IMPLEMENTATION
void list_free_next(List l) {
  if (l == NULL)
    return;

  List next = list_next(l);
  if (next != NULL)
    list_free(next);

  headerof_list(l)->next = headerof_list(NULL);
}
#endif

#define list_end(list) ((typeof(list))_list_end(list))

// _list_end returns a pointer to last element of list.
void *_list_end(List l1);

#ifdef XSTD_LIST_IMPLEMENTATION
void *_list_end(List l) {
  void *result = NULL;

  list_foreach_ptr(l, iter) { result = iter.value; }

  return result;
}
#endif

#endif
