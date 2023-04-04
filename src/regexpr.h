#ifndef REGEXPR_H
#define REGEXPR_H

#include <pcre.h>
#include <stdbool.h>

pcre *regex_compile(const char *pattern);

bool regex_match(pcre *re, char *cmp);

#endif /* REGEXPR_H */
