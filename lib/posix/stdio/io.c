#include <stdio.h>
#include "log/log.h"

int	printf(const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int size = log_printf(fmt, args);
  va_end(args);
  return size;
}

int	fprintf(FILE * file, const char * fmt, ...) {
  char buf[LOG_LINE_SIZE];

  va_list args;
  va_start(args, fmt);
  size_t size = vsnprintf(buf, sizeof(buf) - 1, fmt, args);
  va_end(args);

  buf[size] = '\0';

  vfs_write((vfs_file_t *)file, buf, size);

  return (int) size;
}

int	fgetc(FILE * file) {
  uint8_t ch = '\0';
  vfs_read((vfs_file_t*)file, &ch, 1, VFS_READ_FLAG_NONE);
  return ch;
}

char * fgets(char * buf, int size, FILE * file) {
  vfs_read((vfs_file_t*)file, (uint8_t *)buf, size, VFS_READ_FLAG_NONE);
  return buf;
}

int	fputc(int ch, FILE * file) {
  char c = ch;
  return vfs_write((vfs_file_t*)file, (uint8_t *)&c, 1);
}

int	fputs(const char * str, FILE * file) {
  return vfs_write((vfs_file_t*)file, (uint8_t *)str, strlen(str));
}

int getc(FILE * file) {
  return fgetc(file);
}

int	getchar(void) {
  return fgetc(stdin);
}

char * gets(char * buf) {
  size_t size = 0;

  while (1) {
    char c = getchar();

    buf[size++] = c;

    if (c == '\n') {
      break;
    }
  }

  buf[size] = '\0';

  return buf;
}

int	putc(int ch, FILE * file) {
  return fputc(ch, file);
}

int	putchar(int ch) {
  return fputc(ch, stdout);
}

int	puts(const char * str) {
  return fputs(str, stdout);
}

int	ungetc(int ch, FILE * file) {
  return E_NOTIMPL;
}
