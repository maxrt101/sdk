#include <stdio.h>
#include "vfs/vfs.h"

FILE * fopen(const char * name, const char * type) {
  return (FILE *)vfs_open(&vfs, name);
}

int fclose(FILE * file) {
  return vfs_close((vfs_file_t *)file);
}

// int fflush(FILE * file) {
//   return E_NOTIMPL;
// }

size_t fread(void * buf, size_t size, size_t n, FILE * file) {
  vfs_read((vfs_file_t*)file, buf, size * n, VFS_READ_FLAG_NONE);
  return size; // ?
}

size_t fwrite(const void * buf , size_t size, size_t n, FILE * file) {
  vfs_write((vfs_file_t*)file, buf, size * n);
  return size; // ?
}

int	fgetpos(FILE * file, fpos_t * pos) {
  *pos = (fpos_t) vfs_tell((vfs_file_t*)file);
  return 0;
}

int fsetpos(FILE * file, const fpos_t * pos) {
  return vfs_seek((vfs_file_t*)file, *pos);
}

int	fseek(FILE * file, long offset, int origin) {
  switch (origin) {
    case SEEK_SET:
      return vfs_seek((vfs_file_t*)file, offset);
    case SEEK_CUR:
      return vfs_seek((vfs_file_t*)file, vfs_tell((vfs_file_t*)file) + offset);
    case SEEK_END:
      return vfs_seek((vfs_file_t*)file, VFS_SEEK_END);
    default:
      return E_INVAL;
  }
}

long ftell(FILE * file) {
  return vfs_tell((vfs_file_t*)file);
}

void rewind(FILE * file) {
  vfs_seek((vfs_file_t*)file, 0);
}

int remove(const char * name) {
  return vfs_remove(&vfs, name);
}

int rename(const char * old_name, const char * new_name) {
  return vfs_rename(&vfs, old_name, new_name);
}
