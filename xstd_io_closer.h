#ifndef XSTD_IO_CLOSER_H_INCLUDE
#define XSTD_IO_CLOSER_H_INCLUDE

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

// Closer interface / virtual table. Closer is the interface that wraps basic
// Close method. The behavior of Close after the first call is undefined.
// Specific implementations may document their own behavior.
typedef struct xstd_closer {
  void (*close)(struct xstd_closer *closer, int *error);
} Closer;

// closer_close calls the close method of the closer.
void closer_close(Closer *closer, int *error);

#ifdef XSTD_IO_CLOSER_IMPLEMENTATION
void closer_close(Closer *closer, int *error) { closer->close(closer, error); }
#endif

// FileCloser wraps FILE and implements the Closer interface.
typedef struct {
  Closer closer;
  FILE *f_;
} FileCloser;

// file_closer_close implements Closer.
void file_closer_close(Closer *closer, int *error);
void file_closer_close(Closer *closer, int *error) {
  FileCloser *fcloser = (FileCloser *)closer;
  if (!fclose(fcloser->f_)) {
    *error = errno;
  }
}

// file_closer creates and returns a FileCloser that wraps the given file.
FileCloser file_closer(FILE *f);

#ifdef XSTD_IO_CLOSER_IMPLEMENTATION
FileCloser file_closer(FILE *f) {
  FileCloser fcloser = {0};
  fcloser.closer.close = &file_closer_close;
  fcloser.f_ = f;
  return fcloser;
}
#endif

#endif
