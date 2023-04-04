#include "regexpr.h"

#include <errno.h>
#include <string.h>

#include "logger.h"

pcre *regex_compile(const char *pattern) {
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

  return re;
}

bool regex_match(pcre *re, char *cmp) {
  int ovecsize = 30;  // TODO: size
  int ovector[ovecsize];

  return pcre_exec(re, NULL, cmp, strlen(cmp), 0, 0, ovector, ovecsize) > 0;
}
