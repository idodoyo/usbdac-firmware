#include <sys/stat.h>

int _close(int file) { return -1; }

int _fstat(int file, struct stat *st) {
 st->st_mode = S_IFCHR;
 return 0;
}

int _isatty(int file) { return 1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _open(const char *name, int flags, int mode) { return -1; }

int _read(int file, char *ptr, int len) { return 0; }

caddr_t _sbrk(int incr) {
  extern char _end; // Actually the start of the heap.
  static char *heap_end = &_end;

  char *prev_heap_end = heap_end;
  heap_end += incr;
  return (caddr_t)prev_heap_end;
}

int _write(int file, char *ptr, int len) { return len; }
