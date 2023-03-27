#include "cache.h"

#include <pcre.h>

#include "tap.c/tap.h"

void sanity_test_regex_cache_get() {
  hash_table *ht = ht_init(0);
  const char *regex = "^\\d+$";

  pcre *compiled_regex = regex_cache_get(ht, regex);
  isnt(compiled_regex, NULL, "compiled regex is not NULL");
  int ovecsize = 30;
  int ovector[ovecsize];

  ok(pcre_exec(compiled_regex, NULL, "23", 2, 0, 0, ovector, ovecsize) > 0,
     "retrieved regex is valid");
}

int main() {
  plan(2);

  sanity_test_regex_cache_get();

  done_testing();
}
