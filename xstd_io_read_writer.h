#ifndef XSTD_IO_READ_WRITER_H_INCLUDE
#define XSTD_IO_READ_WRITER_H_INCLUDE

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
#define XSTD_IO_READER_IMPLEMENTATION
#define XSTD_IO_WRITER_IMPLEMENTATION
#endif

#include "xstd_io_reader.h"
#include "xstd_io_writer.h"

// ReaderWriter interface / virtual table. ReadWriter is the interface that
// groups the basic Read and Write methods.
typedef struct {
  Reader reader;
  Writer writer;
} ReadWriter;

// read_writer_read calls the read method of the read writer.
void read_writer_read(ReadWriter *read_writer, uint8_t *p, size_t p_len,
                      size_t *n, int *err);

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
void read_writer_read(ReadWriter *read_writer, uint8_t *p, size_t p_len,
                      size_t *n, int *err) {
  read_writer->reader.read(&read_writer->reader, p, p_len, n, err);
}
#endif

// read_writer_write calls the write method of the read writer.
void read_writer_write(ReadWriter *read_writer, uint8_t *p, size_t p_len,
                       size_t *n, int *err);

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
void read_writer_write(ReadWriter *read_writer, uint8_t *p, size_t p_len,
                       size_t *n, int *err) {
  read_writer->writer.write(&read_writer->writer, p, p_len, n, err);
}
#endif

// FileReadWriter wraps FILE and implements ReadWriter.
typedef struct {
  ReadWriter read_writer_;
  FILE *f_;
} FileReadWriter;

// file_read_writer_read implements Reader.
void file_read_writer_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                           int *err);

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
void file_read_writer_read(Reader *reader, uint8_t *p, size_t p_len, size_t *n,
                           int *err) {
  FileReadWriter *frw = (FileReadWriter *)reader;
  FileReader fr = file_reader(frw->f_);
  file_reader_read((Reader *)&fr, p, p_len, n, err);
}
#endif

// file_read_writer_write implements Writer.
void file_read_writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                            int *err);

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
void file_read_writer_write(Writer *writer, uint8_t *p, size_t p_len, size_t *n,
                            int *err) {
  FileReadWriter *frw = (FileReadWriter *)writer;
  FileWriter fw = file_writer(frw->f_);
  file_writer_write((Writer *)&fw, p, p_len, n, err);
}
#endif

// file_read_writer creates and returns a FileReadWriter that wraps the given
// file.
FileReadWriter file_read_writer(FILE *f);

#ifdef XSTD_IO_READ_WRITER_IMPLEMENTATION
FileReadWriter file_read_writer(FILE *f) {
  FileReadWriter frw = {0};
  frw.read_writer_.reader.read = &file_read_writer_read;
  frw.read_writer_.writer.write = &file_read_writer_write;
  frw.f_ = f;
  return frw;
}
#endif

#endif
