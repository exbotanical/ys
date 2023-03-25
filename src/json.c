#include <stdio.h>
#include <string.h>

#include "libutil/libutil.h"
#include "util.h"
#include "yyjson/yyjson.h"

char *json_getstr(const char *json, const char *field) {
  yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);

  if (strchr(field, '.') == NULL) {
    return yyjson_get_str(yyjson_obj_get(root, field));
  }

  array_t *fields = split(field, ".");

  for (unsigned int i = 0; i < array_size(fields); i++) {
    char *f = array_get(fields, i);
    root = yyjson_obj_get(root, f);
  }

  return yyjson_get_str(root);
}

int json_getint(const char *json, const char *field) {
  yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);

  if (strchr(field, '.') == NULL) {
    return yyjson_get_int(yyjson_obj_get(root, field));
  }

  array_t *fields = split(field, ".");

  for (unsigned int i = 0; i < array_size(fields); i++) {
    char *f = array_get(fields, i);
    root = yyjson_obj_get(root, f);
  }

  return yyjson_get_int(root);
}
