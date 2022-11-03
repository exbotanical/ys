#include "context.h"

route_context_t *route_context_init(char *method, char *path, array_t *parameters) {
	route_context_t *context = malloc(sizeof(route_context_t)); // TODO: validate
	context->method = method;
	context->path = path;
	context->parameters = parameters;

	return context;
}
