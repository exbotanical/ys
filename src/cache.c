#include "cache.h"

#include "regexpr.h"

pcre *regex_cache_get(hash_table *cache, const char *pattern) {
  ht_record *r = ht_search(cache, pattern);
  if (r) {
    return r->value;
  }

  pcre *re = regex_compile(pattern);
  if (!re) {
    return NULL;
  }

  ht_insert(cache, pattern, re);

  return re;
}
