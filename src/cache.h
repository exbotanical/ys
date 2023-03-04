#ifndef CACHE_H
#define CACHE_H

#include <pcre.h>

#include "libhash/libhash.h"

/**
 * @brief Retrieves a pre-compiled pcre regex corresponding to the given pattern
 * from a hash-table cache. If one does not exist, it will be compiled and
 * cached for subsequent retrievals.
 *
 * @param regex_cache
 * @param pattern
 * @return pcre*
 */
pcre *regex_cache_get(h_table *regex_cache, char *pattern);

#endif /* CACHE_H */
