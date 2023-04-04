#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "libutil/libutil.h"
#include "router.h"

typedef struct {
  route_handler *handler;
  array_t *ignore_paths;
} middleware_handler;

#endif /* MIDDLEWARE_H */
