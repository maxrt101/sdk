/** ========================================================================= *
 *
 * @file tests_vfs.c
 * @date 10-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "test/test.h"
#include "trace_alloc/trace_alloc.h"
#include "vfs/vfs.h"
#include "log/log.h"
#include <stdio.h>

/* Defines ================================================================== */

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
error_t vfs_table_pool_alloc(vfs_table_pool_t * pool, table_t ** table);
error_t vfs_node_init(vfs_node_t * node, vfs_node_type_t type, const char * name, bool allocated);
vfs_node_t * vfs_find_node(vfs_t * vfs, const char * path);

TEST_SUITE_DECLARE(VFS, 32);

TEST_DECLARE(VFS, sizeof) {
  TEST_LOG(
    "sizeof(vfs_t)                  %lu\n"
    "sizeof(vfs_node_t)             %lu\n"
    "sizeof(vfs_node_head_t)        %lu\n"
    "sizeof(vfs_node_folder_t)      %lu\n"
    "sizeof(vfs_node_file_t)        %lu\n"
    "sizeof(vfs_node_block_t)       %lu\n"
    "sizeof(vfs_node_symlink_t)     %lu\n"
    "sizeof(vfs_node_hardlink_t)    %lu\n"
    "\n"
    "sizeof(vfs_node_pool_t)        %lu\n"
    "sizeof(vfs_node_container_t)   %lu\n"
    "sizeof(vfs_table_pool_t)       %lu\n"
    "sizeof(vfs_table_container_t)  %lu\n"
    "\n"
    "sizeof(table_t)                %lu\n"
    "sizeof(table_node_t)           %lu\n"
    "",
    sizeof(vfs_t),
    sizeof(vfs_node_t),
    sizeof(vfs_node_head_t),
    sizeof(vfs_node_folder_t),
    sizeof(vfs_node_file_t),
    sizeof(vfs_node_block_t),
    sizeof(vfs_node_symlink_t),
    sizeof(vfs_node_hardlink_t),
    sizeof(vfs_node_pool_t),
    sizeof(vfs_node_container_t),
    sizeof(vfs_table_pool_t),
    sizeof(vfs_table_container_t),
    sizeof(table_t),
    sizeof(table_node_t)
  );

  return true;
}

TEST_DECLARE(VFS, vfs_path_split) {
  const char * expected[] = {"dev", "console", "0"};
  const char * path = "/dev/console/0";

  vfs_path_split_ctx_t split;

  TEST_ASSERT_ERROR(vfs_path_split(path, &split), "vfs_path_split error");

  TEST_LOG("tokens[%zu]: ", split.tokens.size);

  for (size_t i = 0; i < split.tokens.size; ++i) {
    TEST_LOG("'%s' ", split.tokens.buffer[i]);
    TEST_ASSERT_STR_EQ(split.tokens.buffer[i], expected[i], "Unexpected token");
  }

  TEST_LOG("\n");

  return true;
}

TEST_DECLARE(VFS, vfs_path_concat) {
  const char * expected = "/dev/console/0";

  char path_dest[20];
  const char * path_src = "/0";

  strcpy(path_dest, "/dev/console/");
  TEST_ASSERT_ERROR(vfs_path_concat(path_dest, path_src, sizeof(path_dest)), "vfs_path_concat failed");
  TEST_LOG("vfs_path_concat: '%s'\n", path_dest);
  TEST_ASSERT_STR_EQ(path_dest, expected, "unexpected result");

  path_src = "0";
  strcpy(path_dest, "/dev/console");
  TEST_ASSERT_ERROR(vfs_path_concat(path_dest, path_src, sizeof(path_dest)), "vfs_path_concat failed");
  TEST_LOG("vfs_path_concat: '%s'\n", path_dest);
  TEST_ASSERT_STR_EQ(path_dest, expected, "unexpected result");

  path_src = "/0";
  strcpy(path_dest, "/dev/console");
  TEST_ASSERT_ERROR(vfs_path_concat(path_dest, path_src, sizeof(path_dest)), "vfs_path_concat failed");
  TEST_LOG("vfs_path_concat: '%s'\n", path_dest);
  TEST_ASSERT_STR_EQ(path_dest, expected, "unexpected result");

  path_src = "0";
  strcpy(path_dest, "/dev/console/");
  TEST_ASSERT_ERROR(vfs_path_concat(path_dest, path_src, sizeof(path_dest)), "vfs_path_concat failed");
  TEST_LOG("vfs_path_concat: '%s'\n", path_dest);
  TEST_ASSERT_STR_EQ(path_dest, expected, "unexpected result");

  return true;
}

TEST_DECLARE(VFS, suffix_prefix) {
  char path_remove_suffix[VFS_MAX_PATH];
  strcpy(path_remove_suffix, "/dev/console/0");
  TEST_ASSERT_ERROR(vfs_path_remove_suffix(path_remove_suffix, 2), "vfs_path_remove_suffix failed");
  TEST_LOG("vfs_path_remove_suffix: '%s'\n", path_remove_suffix);
  TEST_ASSERT_STR_EQ(path_remove_suffix, "/dev", "unexpected suffix");

  char path_remove_prefix[VFS_MAX_PATH];
  strcpy(path_remove_prefix, "/dev/console/0");
  TEST_ASSERT_ERROR(vfs_path_remove_prefix(path_remove_prefix, 2), "vfs_path_remove_prefix failed");
  TEST_LOG("vfs_path_remove_prefix: '%s'\n", path_remove_prefix);
  TEST_ASSERT_STR_EQ(path_remove_prefix, "0", "unexpected prefix");

  return true;
}

TEST_DECLARE(VFS, parent_and_name) {
  char parent[VFS_MAX_PATH];
  strcpy(parent, "/dev/console/0");
  TEST_ASSERT_ERROR(vfs_path_parent(parent), "vfs_path_parent failed");
  TEST_LOG("vfs_path_parent: '%s'\n", parent);
  TEST_ASSERT_STR_EQ(parent, "/dev/console", "unexpected parent");

  char parent_dev[VFS_MAX_PATH];
  strcpy(parent_dev, "/dev");
  TEST_ASSERT(vfs_path_parent(parent_dev) == E_UNDERFLOW, "vfs_path_parent failed");
  TEST_LOG("vfs_path_parent: '%s'\n", parent_dev);
  TEST_ASSERT_STR_EQ(parent_dev, "/dev", "unexpected parent");

  char name[VFS_MAX_PATH];
  strcpy(name, "/dev/console/0");
  TEST_ASSERT_ERROR(vfs_path_name(name), "vfs_path_name failed");
  TEST_LOG("vfs_path_name: '%s'\n", name);
  TEST_ASSERT_STR_EQ(name, "0", "unexpected name");

  char name_dev[VFS_MAX_PATH];
  strcpy(name_dev, "/dev");
  TEST_ASSERT_ERROR(vfs_path_name(name_dev), "vfs_path_name failed");
  TEST_LOG("vfs_path_name: '%s'\n", name_dev);
  TEST_ASSERT_STR_EQ(name_dev, "dev", "unexpected name");

  return true;
}

TEST_DECLARE(VFS, vfs_find_node) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  vfs_node_t folder_dev;
  vfs_node_t folder_console;
  vfs_node_t file_0;

  TEST_ASSERT_ERROR(vfs_node_init(&folder_dev, VFS_FOLDER, "dev", false), "vfs_node_init failed");
  TEST_ASSERT_ERROR(vfs_node_init(&folder_console, VFS_FOLDER, "console", false), "vfs_node_init failed");
  TEST_ASSERT_ERROR(vfs_node_init(&file_0, VFS_FILE, "0", false), "vfs_node_init failed");

  table_t * table = NULL;
  TEST_ASSERT_ERROR(vfs_table_pool_alloc(vfs.table_pool, &table), "vfs_table_pool_alloc failed");
  folder_dev.folder.children = table;

  table = NULL;
  TEST_ASSERT_ERROR(vfs_table_pool_alloc(vfs.table_pool, &table), "vfs_table_pool_alloc failed");
  folder_console.folder.children = table;

  TEST_ASSERT_ERROR(table_add_str(vfs.root.folder.children, "dev", &folder_dev), "table_add_str failed");
  TEST_ASSERT_ERROR(table_add_str(folder_dev.folder.children, "console", &folder_console), "table_add_str failed");
  TEST_ASSERT_ERROR(table_add_str(folder_console.folder.children, "0", &file_0), "table_add_str failed");

  TEST_LOG("/dev %p\n", vfs_find_node(&vfs, "/dev"));
  TEST_LOG("/dev/console %p\n", vfs_find_node(&vfs, "/dev/console"));
  TEST_LOG("/dev/console/0 %p\n", vfs_find_node(&vfs, "/dev/console/0"));

  vfs_node_t * node = vfs_find_node(&vfs, "/dev/console/0");

  TEST_LOG("node %p '%s' %d\n",
    node,
    node ? node->file.name : "<null>",
    node ? node->head.type : 0
  );

  TEST_ASSERT_NEQ(node, NULL, "node is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_create) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create(&vfs, "/dev", VFS_FILE), "vfs_create failed");

  vfs_node_t * node = vfs_find_node(&vfs, "/dev");

  TEST_LOG("vfs_find_node('/dev'): %p '%s' %d\n",
    node,
    node ? node->file.name : "<null>",
    node ? node->head.type : 0
  );

  TEST_ASSERT_NEQ(node, NULL, "node is null");

  return true;
}

TEST_DECLARE(VFS, vfs_create_folder) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");

  vfs_node_t * node = vfs_find_node(&vfs, "/dev");

  TEST_ASSERT_NEQ(node, NULL, "node is null");

  return true;
}

TEST_DECLARE(VFS, vfs_create_multiple_in_folder) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");

  TEST_ASSERT_NEQ(vfs_find_node(&vfs, "/dev"), NULL, "node is null");

  TEST_ASSERT_ERROR(vfs_create(&vfs, "/dev/0", VFS_FILE), "vfs_create failed");
  TEST_ASSERT_ERROR(vfs_create(&vfs, "/dev/1", VFS_FILE), "vfs_create failed");

  TEST_ASSERT_NEQ(vfs_find_node(&vfs, "/dev/0"), NULL, "node is null");
  TEST_ASSERT_NEQ(vfs_find_node(&vfs, "/dev/1"), NULL, "node is null");

  return true;
}

TEST_DECLARE(VFS, vfs_create_file) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/test", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_node_t * test_file = vfs_find_node(&vfs, "/test");

  if (test_file) {
    for (size_t i = 0; i < test_file->file.data.size; ++i) {
      TEST_LOG("0x%02x ", test_file->file.data.buffer[i]);
      if (i && (i + 1) % 4 == 0) {
        TEST_LOG("\n");
      }
    }
  }

  TEST_ASSERT_NEQ(test_file, NULL, "test_file is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_create_block) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_block(&vfs, "/test", &(vfs_block_data_t){
    (void*) 0xFAFAFBFBFCFCFDFD,
    (vfs_block_open_fn_t) 0xF1F2F3F4F5F6F7F8,
    (vfs_block_close_fn_t) 0xF8F7F6F5F4F3F2F1,
    (vfs_block_read_fn_t) 0xF1F2F3F4F5F6F7F8,
    (vfs_block_write_fn_t) 0xF8F7F6F5F4F3F2F1,
    (vfs_block_ioctl_fn_t) 0xF1F2F3F4F5F6F7F8
  }), "vfs_create_block failed");

  vfs_node_t * test_block = vfs_find_node(&vfs, "/test");

  if (test_block) {
    TEST_LOG("ctx=%p\nopen=%p\nclose=%p\nread=%p\nwrite=%p\nioctl=%p\n",
      test_block->block.data.ctx,
      test_block->block.data.open,
      test_block->block.data.close,
      test_block->block.data.read,
      test_block->block.data.write,
      test_block->block.data.ioctl);
  }

  TEST_ASSERT_NEQ(test_block, NULL, "test_block is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_create_symlink) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_symlink(&vfs, "/test_link", "/tmp/link_target"), "vfs_create_symlink");

  vfs_node_t * test_link = vfs_find_node(&vfs, "/test_link");

  if (test_link) {
    TEST_LOG("'%s'\n", test_link->symlink.path);
  }

  TEST_ASSERT_NEQ(test_link, NULL, "test_link is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_create_hardlink) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_hardlink(&vfs, "/test_link", (vfs_node_t*) 0xDEADBABE), "vfs_create_hardlink");

  vfs_node_t * test_hardlink = vfs_find_node(&vfs, "/test_link");

  if (test_hardlink) {
    TEST_LOG("%p\n", test_hardlink->hardlink.node);
  }

  TEST_ASSERT_NEQ(test_hardlink, NULL, "test_hardlink is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_remove) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/test"), "vfs_create_folder");
  vfs_node_t * node = vfs_find_node(&vfs, "/test");
  TEST_LOG("after vfs_create /test %p\n", node);
  TEST_ASSERT_NEQ(node, NULL, "node is NULL after create");

  TEST_ASSERT_ERROR(vfs_remove(&vfs, "/test"), "vfs_remove failed");
  node = vfs_find_node(&vfs, "/test");
  TEST_LOG("after vfs_remove /test %p\n", node);
  TEST_ASSERT_EQ(node, NULL, "node is not NULL after remove");

  return true;
}

TEST_DECLARE(VFS, vfs_rename) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/test"), "vfs_create_folder");

  vfs_node_t * node = vfs_find_node(&vfs, "/test");
  TEST_LOG("after vfs_create /test %p\n", node);
  TEST_ASSERT_NEQ(node, NULL, "test node is NULL after create");

  node = vfs_find_node(&vfs, "/folder");
  TEST_LOG("after vfs_create /folder %p\n", node);
  TEST_ASSERT_EQ(node, NULL, "folder node is not NULL after create");

  TEST_ASSERT_ERROR(vfs_rename(&vfs, "/test", "folder"), "vfs_rename");

  node = vfs_find_node(&vfs, "/test");
  TEST_LOG("after vfs_rename /test %p\n", node);
  TEST_ASSERT_EQ(node, NULL, "test node is not NULL after create");

  node = vfs_find_node(&vfs, "/folder");
  TEST_LOG("after vfs_rename /folder %p\n", node);
  TEST_ASSERT_NEQ(node, NULL, "folder node is NULL after create");

  return true;
}

TEST_DECLARE(VFS, vfs_mkdir) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 5);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 5);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_mkdir(&vfs, "/dev/console/test"), "vfs_mkdir failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/test/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_node_t * node = vfs_find_node(&vfs, "/dev/console/test/0");

  TEST_ASSERT_NEQ(node, NULL, "node is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_dynamic) {
  trace_alloc_start(NULL);

  vfs_t vfs;

  TEST_ASSERT_ERROR(vfs_init(&vfs, NULL, NULL), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_node_t * node = vfs_find_node(&vfs, "/dev/console/0");

  TEST_LOG("vfs_find_node('/dev/console/0'): %p '%s' %d\n",
    node,
    node ? node->file.name : "<null>",
    node ? node->head.type : 0
  );

  TEST_ASSERT_NEQ(node, NULL, "node is NULL");

  TEST_ASSERT_ERROR(vfs_remove(&vfs, "/dev/console/0"), "vfs_remove failed");

  node = vfs_find_node(&vfs, "/dev/console/0");

  TEST_ASSERT_EQ(node, NULL, "node is not NULL");

  TEST_ASSERT_ERROR(vfs_deinit(&vfs), "vfs_deinit");

  size_t leaks = trace_alloc_end(NULL);

  TEST_LOG("leaks: %zu\n", leaks);

  TEST_ASSERT_EQ(leaks, 0, "memory leaks detected");

  return true;
}

TEST_DECLARE(VFS, vfs_open) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");


  vfs_file_t * file = vfs_open(&vfs, "/dev/console/0");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  return true;
}

TEST_DECLARE(VFS, vfs_read) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");


  vfs_file_t * file = vfs_open(&vfs, "/dev/console/0");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  uint8_t buf[4] = {0};

  TEST_ASSERT_ERROR(vfs_read(file, buf, sizeof(buf)), "vfs_read failed");

  for (size_t i = 0; i < sizeof(buf); ++i) {
    TEST_ASSERT_EQ(test_file_data[i], buf[i], "buf != file data");
  }

  return true;
}

TEST_DECLARE(VFS, vfs_write) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_file_t * file = vfs_open(&vfs, "/dev/console/0");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  uint8_t write_buf[4] = {0xF1, 0xF2, 0xF3, 0xF4};

  TEST_ASSERT_ERROR(vfs_write(file, write_buf, sizeof(write_buf)), "vfs_write failed");

  file->file.data.offset = 0;

  uint8_t read_buf[4] = {0};

  TEST_ASSERT_ERROR(vfs_read(file, read_buf, sizeof(read_buf)), "vfs_read failed");

  for (size_t i = 0; i < sizeof(read_buf); ++i) {
    TEST_ASSERT_EQ(write_buf[i], read_buf[i], "buf != file data");
  }

  return true;
}

TEST_DECLARE(VFS, vfs_seek) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_file_t * file = vfs_open(&vfs, "/dev/console/0");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  TEST_ASSERT_ERROR(vfs_seek(file, 4), "vfs_seek");

  uint8_t buf[4] = {0};

  TEST_ASSERT_ERROR(vfs_read(file, buf, sizeof(buf)), "vfs_read failed");

  for (size_t i = 0; i < sizeof(buf); ++i) {
    TEST_ASSERT_EQ(test_file_data[i+4], buf[i], "buf != file data");
  }

  return true;
}

TEST_DECLARE(VFS, vfs_tell) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev"), "vfs_create_folder failed");
  TEST_ASSERT_ERROR(vfs_create_folder(&vfs, "/dev/console"), "vfs_create_folder failed");

  uint8_t test_file_data[16] = {
    0xEB, 0xAC, 0x0C, 0x01,
    0xEB, 0xAC, 0x0C, 0x02,
    0xEB, 0xAC, 0x0C, 0x03,
    0xEB, 0xAC, 0x0C, 0x04,
  };

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/dev/console/0", &(vfs_file_data_t){
    test_file_data, sizeof(test_file_data), sizeof(test_file_data), 0
  }), "vfs_create_file failed");

  vfs_file_t * file = vfs_open(&vfs, "/dev/console/0");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  uint8_t buf[4] = {0};

  TEST_ASSERT_ERROR(vfs_read(file, buf, sizeof(buf)), "vfs_read failed");

  TEST_ASSERT_EQ(vfs_tell(file), 4, "tell returned bad value after read");

  return true;
}

struct {
  bool block_open_called;
  bool block_close_called;
  bool block_read_called;
  bool block_write_called;
  bool block_ioctl_called;
  bool block_seek_called;
  bool block_tell_called;
} block_test_ctx;

error_t block_open(void * ctx, vfs_file_t * file) {
  block_test_ctx.block_open_called = true;

  return E_OK;
}

error_t block_close(void * ctx, vfs_file_t * file) {
  block_test_ctx.block_close_called = true;

  return E_OK;
}

error_t block_read(void * ctx, vfs_file_t * file, uint8_t * buf, size_t size) {
  block_test_ctx.block_read_called = true;

  return E_OK;
}

error_t block_write(void * ctx, vfs_file_t * file, const uint8_t * buf, size_t size) {
  block_test_ctx.block_write_called = true;

  return E_OK;
}

error_t block_ioctl(void * ctx, vfs_file_t * file, int cmd, va_list args) {

  switch (cmd) {
    case VFS_IOCTL_SEEK:
      block_test_ctx.block_seek_called = true;
      break;

    case VFS_IOCTL_TELL:
      block_test_ctx.block_tell_called = true;
      break;

    case VFS_IOCTL_RESERVED_128:
      block_test_ctx.block_ioctl_called = true;
      break;

    default:
      return E_INVAL;
  }

  return E_OK;
}

TEST_DECLARE(VFS, vfs_block) {
  memset(&block_test_ctx, 0, sizeof(block_test_ctx));

  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_block(&vfs, "/test", &(vfs_block_data_t){
    NULL, block_open, block_close, block_read, block_write, block_ioctl
  }), "vfs_create_block failed");

  vfs_file_t * file = vfs_open(&vfs, "/test");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  TEST_ASSERT_ERROR(vfs_read(file, (uint8_t *)0x1, 1), "vfs_read failed");
  TEST_ASSERT_ERROR(vfs_write(file, (uint8_t *)0x1, 1), "vfs_write failed");
  TEST_ASSERT_ERROR(vfs_ioctl(file, VFS_IOCTL_RESERVED_128), "vfs_ioctl failed");
  TEST_ASSERT_ERROR(vfs_ioctl(file, VFS_IOCTL_SEEK), "vfs_ioctl failed");
  TEST_ASSERT_ERROR(vfs_ioctl(file, VFS_IOCTL_TELL), "vfs_ioctl failed");

  TEST_ASSERT_ERROR(vfs_close(file), "vfs_close failed");

  TEST_ASSERT(block_test_ctx.block_open_called, "open wasn't called");
  TEST_ASSERT(block_test_ctx.block_close_called, "close wasn't called");
  TEST_ASSERT(block_test_ctx.block_read_called, "read wasn't called");
  TEST_ASSERT(block_test_ctx.block_write_called, "write wasn't called");
  TEST_ASSERT(block_test_ctx.block_ioctl_called, "ioctl wasn't called");
  TEST_ASSERT(block_test_ctx.block_seek_called, "seek wasn't called");
  TEST_ASSERT(block_test_ctx.block_tell_called, "tell wasn't called");

  return true;
}

TEST_DECLARE(VFS, vfs_dynamic_file) {
  vfs_t vfs;
  VFS_DECLARE_NODE_POOL(vfs_node_pool, 4);
  VFS_DECLARE_TABLE_POOL(vfs_table_pool, 4);

  TEST_ASSERT_ERROR(vfs_init(&vfs, &vfs_node_pool, &vfs_table_pool), "vfs_init failed");

  TEST_ASSERT_ERROR(vfs_create_file(&vfs, "/test", &(vfs_file_data_t){NULL, 16}), "vfs_crate_file failed");

  vfs_file_t * file = vfs_open(&vfs, "/test");

  TEST_ASSERT_NEQ(file, NULL, "file is NULL");

  uint8_t write_buffer[4] = {0xFA, 0xFB, 0xFC, 0xFD};

  TEST_ASSERT_ERROR(vfs_write(file, write_buffer, sizeof(write_buffer)), "vfs_write failed");

  TEST_ASSERT_ERROR(vfs_seek(file, 0), "vfs_seek failed");

  uint8_t read_buffer[4] = {0};

  TEST_ASSERT_ERROR(vfs_read(file, read_buffer, sizeof(read_buffer)), "vfs_read failed");

  for (size_t i = 0; i < sizeof(read_buffer); ++i) {
    TEST_ASSERT_EQ(write_buffer[i], read_buffer[i], "write_buf data != read_buf data");
  }

  return true;
}

/* Shared functions ========================================================= */
int main(int argc, char ** argv) {
  TEST_LOG_PORT(COLOR_BOLD "====================[ VFS ]====================\n" COLOR_RESET);

  return tests_run(&VFS, argc, argv);
}

