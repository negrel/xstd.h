#ifdef __clang__
#define typeof __typeof__
#else
#ifndef typeof
#define typeof assert(0 && "typeof macro is not defined")
#endif
#endif

#define type_assert_eq(X, Y)                                                   \
  _Generic((Y), typeof(X): _Generic((X), typeof(Y): (void)NULL))
