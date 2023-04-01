#include "jsob.h"

#include <stdio.h>
#include <string.h>

#include "libutil/libutil.h"
#include "yyjson.h"

/**
 * split splits a string on all instances of a delimiter.
 *
 * @param s The string to split
 * @param delim The delimiter on which to split `str`
 * @return array_t* An array of matches, if any; NULL if erroneous
 *
 * @internal
 */
static array_t *split(const char *s, const char *delim) {
  if (s == NULL || delim == NULL) {
    return NULL;
  }

  // see:
  // https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
  char *input = s_copy(s);

  array_t *tokens = array_init();
  if (tokens == NULL) {
    return NULL;
  }

  // If the input doesn't even contain the delimiter, return early and avoid
  // further computation
  if (!strstr(input, delim)) {
    return tokens;
  }

  // If the input *is* the delimiter, just return the empty array
  if (s_equals(input, delim)) {
    return tokens;
  }

  char *token = strtok(input, delim);
  if (token == NULL) {
    return tokens;
  }

  while (token != NULL) {
    array_push(tokens, s_copy(token));
    token = strtok(NULL, delim);
  }

  free(input);

  return tokens;
}

char *jsob_getstr(const char *json, const char *getter) {
  yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);

  if (strchr(getter, '.') == NULL) {
    return yyjson_get_str(yyjson_obj_get(root, getter));
  }

  array_t *fields = split(getter, ".");

  for (unsigned int i = 0; i < array_size(fields); i++) {
    char *field = array_get(fields, i);
    root = yyjson_obj_get(root, field);
  }

  return yyjson_get_str(root);
}

int jsob_getint(const char *json, const char *getter) {
  yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
  yyjson_val *root = yyjson_doc_get_root(doc);

  if (strchr(getter, '.') == NULL) {
    return yyjson_get_int(yyjson_obj_get(root, getter));
  }

  array_t *fields = split(getter, ".");

  for (unsigned int i = 0; i < array_size(fields); i++) {
    char *field = array_get(fields, i);
    root = yyjson_obj_get(root, field);
  }

  return yyjson_get_int(root);
}
