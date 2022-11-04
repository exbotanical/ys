#ifndef CONTEXT_H
#define CONTEXT_H

#include "array.h"

/**
 * @brief TODO: doc
 *
 */
typedef struct route_context {
	char *method;
	char *path;
	array_t *parameters;
} route_context_t;

/**
 * @brief Initializes an object containing request metadata for a matched route.
 *
 * @param method The HTTP method of the matched route
 * @param path The path of the matched route
 * @param parameters Any parameters derived from the matched route
 * @return route_context_t* Route context, or NULL if memory allocation failed
 */
route_context_t *route_context_init(char *method, char *path, array_t *parameters);

#endif /* CONTEXT_H */