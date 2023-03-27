#include "cache.h"

#include <errno.h>

#include "logger.h"

pcre *regex_cache_get(hash_table *cache, const char *pattern) {
  ht_record *r = ht_search(cache, pattern);
  if (r) {
    return r->value;
  }

  const char *error;
  int erroffset;

  pcre *re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
  if (re == NULL) {
    printlogf(
        LOG_INFO,
        "[cache::%s] PCRE compilation failed at offset %d: %s; errno: %d\n",
        __func__, erroffset, error, errno);
    return NULL;
  }

  ht_insert(cache, pattern, re);

  return re;
}
