#include "context.h"

#include "logger.h"

#include <malloc.h>

route_context_t *route_context_init(
	int client_socket,
	char *method,
	char *path,
	array_t *parameters
) {
	route_context_t *context = malloc(sizeof(route_context_t));
	if (!context) {
		free(context);

		LOG(
			"[context::route_context_init] %s\n",
			"failed to allocate route_context_t"
		);

		return NULL;
	}

	context->client_socket = client_socket;
	context->method = method;
	context->path = path;
	context->parameters = parameters;

	return context;
}
