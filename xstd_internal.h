#ifdef __clang__
#define typeof __typeof__
#else
#ifndef typeof
#define typeof assert(0 && "typeof macro is not defined")
#endif
#endif
