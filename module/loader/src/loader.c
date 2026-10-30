#include <loader.h>
#include <loader/export.h>
#include <log/optional.h>
#include <string.h>

LDS_DEFINE(loader_api);
LDS_DEFINE(export_symbols);

LOGGER_DECLARE(loader);

__WEAK error_t loader_file_load(loader_file_t * file, void * data, size_t size) {
  ASSERT_RETURN(file && data, E_NULL);
  ASSERT_RETURN(size, E_EMPTY);

  file->data     = data;
  file->size     = size;
  file->resolver = export_symbol_lookup;

  error_t err = E_NOTIMPL;

  LDS_FOR_EACH(loader_api, api) {
    if (api->probe(file)) {
      file->api = api;
      err = file->api->load(file);
      break;
    }
  }

  return err;
}

__WEAK error_t loader_file_unload(loader_file_t * file) {
  memset(file, 0, sizeof(loader_file_t));
  return E_OK;
}

void * loader_file_symbol_lookup(loader_file_t * file, const char * name) {
  ASSERT_RETURN(file && file->api, NULL);

  return file->api->sym_lookup(file, name);
}

void * export_symbol_lookup(const char * name) {
  LDS_FOR_EACH(export_symbols, sym) {
    if (!strcmp(name, sym->name)) {
      return sym->addr;
    }
  }

  return NULL;
}
