#ifdef __clang__
#define typeof __typeof__
#else
#ifdef __GNUC__
#define typeof __typeof__
#endif
#endif

#define type_assert_eq(X, Y)                                                   \
  _Generic((Y), typeof(X): _Generic((X), typeof(Y): (void)NULL))
