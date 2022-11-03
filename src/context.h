#ifndef CONTEXT_H
#define CONTEXT_H

#include "array.h"

typedef struct route_context {
	char *method;
	char *path;
	array_t *parameters;
} route_context_t;

route_context_t *route_context_init(char *method, char *path, array_t *parameters);

#endif /* CONTEXT_H */
