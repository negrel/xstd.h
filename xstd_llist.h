#ifndef XSTD_LINKED_LIST_H_INCLUDE
#define XSTD_LINKED_LIST_H_INCLUDE

#include <stddef.h>
#define XSTD_LINKED_LIST_IMPLEMENTATION
#ifdef XSTD_LINKED_LIST_IMPLEMENTATION
#include <stdlib.h>
#endif

typedef struct linked_list_item {
  // Header
  struct linked_list_item *next;

  // Body
  // ...
} LinkedListItem;

typedef struct {
  LinkedListItem *first;
  LinkedListItem **last;
} LinkedList;

#ifdef __clang__
#define typeof __typeof__
#endif

#define headerof_llist_item(ll_item_ptr)                                       \
  (LinkedListItem *)((uintptr_t)ll_item_ptr - sizeof(LinkedListItem))

#define bodyof_llist_item(ll_item_ptr)                                         \
  ((uintptr_t)ll_item_ptr + sizeof(LinkedListItem))

#define llist_item_value(ll_item_ptr, type)                                    \
  *(type *)bodyof_llist_item(ll_item_ptr)

#define llist_foreach(ll, type, iterator)                                      \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        LinkedListItem *item;                                                  \
        type value;                                                            \
      } iterator =                                                             \
          {                                                                    \
              .index = 0,                                                      \
             .item = (ll)->first,                                              \
             .value = (ll)->first != NULL &&                                   \
                      *(type *)bodyof_llist_item((ll)->first),                 \
          };                                                                   \
      iterator.item != NULL;                                                   \
      iterator.index += 1, iterator.item = iterator.item->next,                \
        iterator.value = iterator.item != NULL &&                              \
                         *(type *)bodyof_llist_item(iterator.item))

#define llist_foreach_ptr(ll, type, iterator)                                  \
  for (                                                                        \
      struct {                                                                 \
        size_t index;                                                          \
        LinkedListItem *item;                                                  \
        type *value;                                                           \
      } iterator =                                                             \
          {                                                                    \
              .index = 0,                                                      \
             .item = (ll)->first,                                              \
             .value = (ll)->first != NULL &&                                   \
                      (type *)bodyof_llist_item((ll)->first),                  \
          };                                                                   \
      iterator.item != NULL;                                                   \
      iterator.index += 1, iterator.item = iterator.item->next,                \
        iterator.value =                                                       \
            iterator.item != NULL && (type *)bodyof_llist_item(iterator.item))

void llist_init(LinkedList *ll);

#ifdef XSTD_LINKED_LIST_IMPLEMENTATION
void llist_init(LinkedList *ll) {
  ll->first = NULL;
  ll->last = &ll->first;
}
#endif

#define llist_append(ll, item)                                                 \
  do {                                                                         \
    LinkedListItem *_new_last =                                                \
        calloc(1, sizeof(LinkedListItem) + sizeof(typeof(item)));              \
    *(typeof(item) *)bodyof_llist_item(_new_last) = item;                      \
    *(ll)->last = _new_last;                                                   \
    (ll)->last = &_new_last->next;                                             \
  } while (0)

#define llist_prepend(ll, item)                                                \
  do {                                                                         \
    LinkedListItem *_new_first =                                               \
        calloc(1, sizeof(LinkedListItem) + sizeof(typeof(item)));              \
    *(typeof(item) *)bodyof_llist_item(_new_first) = item;                     \
    if ((ll)->first == NULL) {                                                 \
      (ll)->last = &_new_first->next;                                          \
    }                                                                          \
    _new_first->next = (ll)->first;                                            \
    (ll)->first = _new_first;                                                  \
  } while (0)

#endif
