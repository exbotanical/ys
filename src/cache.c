#include "cache.h"

#include "logger.h"

pcre *regex_cache_get(h_table *regex_cache, char *pattern) {
  h_record *r = h_search(regex_cache, pattern);
  if (r) {
    return r->value;
  }

  const char *error;
  int erroffset;

  pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
  if (re == NULL) {
    free(re);
    LOG("[cache::regex_cache_get] %s %d: %s\n",
        "PCRE compilation failed at offset", erroffset, error);

    return NULL;
  }

  h_insert(regex_cache, pattern, re);

  return re;
}
