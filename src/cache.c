#include "cache.h"

#include <errno.h>

#include "logger.h"

pcre *regex_cache_get(hash_table *regex_cache, char *pattern) {
  ht_record *r = ht_search(regex_cache, pattern);
  if (r) {
    return r->value;
  }

  const char *error;
  int erroffset;

  pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
  if (re == NULL) {
    free(re);
    LOG("[cache::regex_cache_get] %s %d: %s; errno: %d\n",
        "PCRE compilation failed at offset", erroffset, error, errno);
    return NULL;
  }

  ht_insert(regex_cache, pattern, re);

  return re;
}
