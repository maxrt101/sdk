#include <util.h>

#if has_module(loader)

#include <loader.h>
#include <loader/export.h>

#include <sys/mman.h>

#include <string.h>
#include <stdio.h>

error_t loader_file_load(loader_file_t * file, void * data, size_t size) {
  ASSERT_RETURN(file && data, E_NULL);
  ASSERT_RETURN(size, E_EMPTY);

  int flags = MAP_PRIVATE | MAP_ANON;

#if defined(__APPLE__) && defined(__arm64__)
  flags |= MAP_JIT;
#endif

  file->data = mmap(NULL, size, PROT_READ | PROT_WRITE, flags, -1, 0);
  if (file->data == MAP_FAILED) {
    return E_NOMEM;
  }

  memcpy(file->data, data, size);

  file->size     = size;
  file->resolver = export_symbol_lookup;

  error_t err = E_NOMEM;

  LDS_FOR_EACH(loader_api, api) {
    if (api->probe(file)) {
      file->api = api;
      err = file->api->load(file);
      break;
    }
  }

  if (err != E_OK) {
    loader_file_unload(file);
    return err;
  }

  if (mprotect(file->data, size, PROT_READ | PROT_EXEC) != 0) {
    loader_file_unload(file);
    return E_PERM;
  }

  return E_OK;
}

error_t loader_file_unload(loader_file_t * file) {
  ASSERT_RETURN(file && file->data, E_NULL);

  munmap(file->data, file->size);

  memset(file, 0, sizeof(loader_file_t));

  return E_OK;
}

#endif