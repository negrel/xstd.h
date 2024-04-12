#ifndef XSTD_IFACE_H_INCLUDE
#define XSTD_IFACE_H_INCLUDE

// Used by iface_call macros.
#include <stdint.h>

#include "internal.h"

// InterfaceImpl define an implementation of an interface.
typedef void *InterfaceImpl;

// Define an interface type and its virtual table.
#define iface_def(name, vtable)                                                \
  typedef struct {                                                             \
    vtable *vtable_;                                                           \
    size_t offset_;                                                            \
  } name

// Call and a method of an iface instance.

#define iface_call(instance, method, ...)                                      \
  instance->vtable_->method((void *)((uintptr_t)instance + instance->offset_), \
                            __VA_ARGS__)

#define iface_call_empty(instance, method)                                     \
  instance->vtable_->method((void *)((uintptr_t)instance + instance->offset_))

#define iface_impl_def_empty(interface, name)                                  \
  typedef struct {                                                             \
    interface iface;                                                           \
  } name

#define iface_impl_def(interface, name, impl_fields)                           \
  typedef struct {                                                             \
    interface iface;                                                           \
    impl_fields body_;                                                         \
  } name

#define iface_impl_init(instance, field, vtable, body)                         \
  do {                                                                         \
    (instance)->field = (typeof((instance)->field)){                           \
        .vtable_ = vtable,                                                     \
        .offset_ = offsetof(typeof(*instance), body_) -                        \
                   offsetof(typeof(*instance), field)};                        \
    (instance)->body_ = (body);                                                \
  } while (0);

#define iface_empty_impl_init(instance, field, vtable)                         \
  do {                                                                         \
    (instance)->field = (typeof((instance)->field)){                           \
        .vtable_ = vtable,                                                     \
        .offset_ = 0,                                                          \
    };                                                                         \
  } while (0);

#define typeof_iface_impl(impl_type) typeof(((impl_type *){0})->body_)

#define cast_iface_impl_ptr(impl_type, ptr) (typeof_iface_impl(impl_type) *)ptr
#define cast_iface_impl(impl_type, ptr) *cast_iface_impl_ptr(impl_type, ptr)

// Merged interfaces.

#define iface_merge_def(name, ...)                                             \
  typedef struct {                                                             \
    __VA_ARGS__;                                                               \
  } name

#endif
