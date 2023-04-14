#ifndef CACHE_H
#define CACHE_H

#include <pcre.h>

#include "libhash/libhash.h"

/**
 * regex_cache_get retrieves a pre-compiled pcre regex corresponding to the
 * given pattern from a hash-table cache. If one does not exist, it will be
 * compiled and cached for subsequent retrievals.
 */
pcre *regex_cache_get(hash_table *cache, const char *pattern);

#endif /* CACHE_H */
