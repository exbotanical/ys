// IN PROGRESS
#include "fs_router.h"

#include <dirent.h>
#include <dlfcn.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libutil/libutil.h"
#include "logger.h"
#include "xmalloc.h"

static router_t *router;
// TODO: make atomic
static const char *ignore_dir;

char *create_temp_file(char *filename_template) {
  int fd = -1;
  char *temp_filename = NULL;

  // Copy filename template to a buffer we can modify
  size_t template_len = strlen(filename_template);
  char *template_copy = xmalloc(template_len + 1);
  strcpy(template_copy, filename_template);

  // Call mkstemp to create a unique filename
  fd = mkstemp(template_copy);
  if (fd == -1) {
    perror("mkstemp");
    goto cleanup;
  }

  // Close the file descriptor, we don't need it anymore
  close(fd);

  // Allocate memory for the final filename and copy it
  temp_filename = xmalloc(template_len + 1);
  strcpy(temp_filename, template_copy);

  // Rename the temporary file to the final filename
  if (rename(template_copy, temp_filename) == -1) {
    perror("rename");
    free(temp_filename);
    temp_filename = NULL;
    goto cleanup;
  }

cleanup:
  free(template_copy);
  return temp_filename;
}

static void load_route(const char *file_path) {
  const char *library =
      create_temp_file("/tmp/__route__XXXXXX");  // TODO: just reuse

  const char *template = "gcc -shared -fPIC -o %s %s";
  char compile_cmd[strlen(library) + strlen(template) + strlen(file_path)];
  snprintf(compile_cmd, sizeof(compile_cmd), template, library, file_path);

  if (system(compile_cmd) != 0) {
    perror("system");
    // TODO: handle
    return;
  }

  void *h = dlopen(library, RTLD_LAZY);
  if (!h) {
    printf("failed: %s\n", dlerror());
    // TODO: handle
    return;
  }

  for (http_method_t i = METHOD_GET; i <= METHOD_OPTIONS; i++) {
    handler_t *fn;
    *(void **)(&fn) = dlsym(h, http_method_names[i]);
    if (!fn) {
      continue;
    }

    // remove file extension and sync dir (+ / prefix)
    char *route_path =
        str_truncate(str_truncate(file_path, -2), strlen(ignore_dir));
    // TODO: multiple methods
    router_register(router, route_path, fn, NULL, i, NULL);
  }

  // dlclose(h);
}

static void traverse_dir(const char *dir_path) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    perror("opendir");
    // TODO: handle
    return;
  }

  struct dirent *ent;
  while ((ent = readdir(dir))) {
    if (ent->d_type == DT_DIR) {
      // Ignore "." and ".." directories
      if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        continue;

      // Recursively traverse the subdirectory
      char sub_dir_path[PATH_MAX];

      snprintf(sub_dir_path, sizeof(sub_dir_path), "%s/%s", dir_path,
               ent->d_name);
      traverse_dir(sub_dir_path);
    } else if (ent->d_type == DT_REG) {
      // Process regular files
      char file_path[PATH_MAX];
      snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, ent->d_name);

      load_route(file_path);
    }
  }

  closedir(dir);
}

router_t *sync_file_router(const char *dir_path) {
  if (!router) {
    router_attr_t attr = ROUTE_ATTR_INITIALIZER;
    router = router_init(attr);
  }

  ignore_dir = dir_path;
  traverse_dir(dir_path);

  return router;
}
