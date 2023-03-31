#ifndef COOKIE_H
#define COOKIE_H

#include <time.h>

#include "libhttp.h"

typedef struct {
  // IETF RFC 6265, Section 5.2.2
  // If delta-seconds is less than or equal to zero (0), let expiry-time
  // be the earliest representable date and time.  Otherwise, let the
  // expiry-time be the current date and time plus delta-seconds seconds.
  int max_age;
  time_t expires;
  same_site_mode same_site;
  char* name;
  char* value;
  char* path;
  char* domain;
  bool http_only;
  bool secure;
} cookie_internal;

#endif /* COOKIE_H */
