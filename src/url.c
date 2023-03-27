#include <stdlib.h>
#include <string.h>

#include "libutil/libutil.h"
#include "util.h"

/**
 * unescape unescapes a string `s`
 *
 * @param s
 * @return char*
 */
static char* unescape(const char* s) {
  // Count %, check that they're well-formed
  int n = 0;
  bool has_plus = false;

  for (unsigned int i = 0; i < strlen(s);) {
    switch (s[i]) {
      case '%':
        n++;
        if (i + 2 >= strlen(s) || !ishex(s[i + 1]) || !ishex(s[i + 2])) {
          s = s_substr(s, i, strlen(s), true);
          if (strlen(s) > 3) {
            s = s_substr(s, 0, 3, true);
          }

          return NULL;  // err
        }

        i += 3;
      case '+':
        has_plus = true;
        i++;
      default:
        i++;
    }
  }

  if (n == 0 && !has_plus) {
    return s;
  }

  buffer_t* b = buffer_init(NULL);

  for (unsigned int i = 0; i < strlen(s); i++) {
    switch (s[i]) {
      case '%':
        char* sp = tostr(unhex(s[i + 1]) << 4 | unhex(s[i + 2]));
        buffer_append(b, sp);
        i += 2;
        break;
      case '+':
        buffer_append(b, " ");
        break;
      default:
        char* spp = tostr(s[i]);
        buffer_append(b, spp);
    }
  }

  return buffer_state(b);
}

hash_table* parse_query(const char* query) {
  hash_table* ht = ht_init(0);

  while (!s_nullish(query)) {
    array_t* pair1 = str_cut(query, "&");
    if (!pair1) {
      return ht;
    }

    char* key = array_get(pair1, 0);
    query = array_get(pair1, 1);
    if (strchr(key, ';')) {
      continue;
    }

    if (s_nullish(key)) {
      continue;
    }

    array_t* pair2 = str_cut(key, "=");
    if (!pair2) {
      return ht;
    }

    key = array_get(pair2, 0);
    char* value = array_get(pair2, 1);

    key = unescape(key);

    if (!key) {
      continue;
    }

    value = unescape(value);

    if (!value) {
      continue;
    }

    array_t* r = (array_t*)ht_get(ht, key);
    if (!r) {
      ht_insert(ht, key, array_collect(value));
    } else {
      array_push(r, value);
    }
  }

  return ht;
}

bool has_query_string(const char* url) {
  const char* qmark = strchr(url, '?');
  if (!qmark) {
    return false;
  }

  // Check if the question mark is the last character in the URL
  if (qmark == url + strlen(url) - 1) {
    return false;
  }

  // Check if the URL has more than one question mark
  if (strchr(qmark + 1, '?')) {
    return false;
  }

  // Check if the URL has a fragment identifier
  const char* hash = strchr(url, '#');
  if (hash && hash < qmark) {
    // Fragment identifier appears before query string, so it's invalid
    return false;
  }

  return true;
}
