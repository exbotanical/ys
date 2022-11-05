#include "router.h"

#include "http.h"
#include "logger.h"
#include "server.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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

router_t *router_init(
	void*(*not_found_handler)(void*),
	void*(*method_not_allowed_handler)(void*)
) {
	router_t *router = malloc(sizeof(router_t));
	if (!router) {
		router_free(router);
		LOG("[router::router_init] %s\n", "failed to allocate router_t");

		return NULL;
	}

	router->trie = trie_init();
	if (router->trie == NULL) {
		router_free(router);
		LOG("[router::router_init] %s\n", "failed to allocate trie via trie_init");

		return NULL;
	}

	if (not_found_handler == NULL) {
		LOG(
			"[router::router_init] %s\n",
			"not_found_handler is NULL, using fallback handler"
		);
		router->not_found_handler = default_not_found_handler;
	} else {
		router->not_found_handler = not_found_handler;
	}

	if (method_not_allowed_handler == NULL) {
		LOG(
			"[router::router_init] %s\n",
			"method_not_allowed_handler is NULL, using fallback handler"
		);
		router->method_not_allowed_handler = default_method_not_allowed_handler;
	} else {
		router->method_not_allowed_handler = method_not_allowed_handler;
	}

	return router;
}

bool router_register(router_t *router, ch_array_t *methods, const char *path, void*(*handler)(void*)) {
	if (!router || !methods || !path || !handler) {
		DIE(EXIT_FAILURE, "%s\n", "invariant violation - router_register arguments cannot be NULL");
	}

	if (!trie_insert(router->trie, methods, path, handler)) {
		char *message = "failed to insert route record";
		LOG("[route::router_register] %s\n", message);
		STDERR("%s\n", message);

		return false;
	}

	return true;
}

bool router_run(router_t *router, route_context_t *context) {
	if (!context) {
		LOG("[router::router_run] context initialization via route_context_init \
			failed with method %s, path %s",
		 	context->method,
			context->path
		);

		return false;
	}

  response_t *response;
	result_t *result = trie_search(router->trie, context->method, context->path); // TODO: args same order
	if (!result) { // TODO: NotFound vs NotAllowed
		LOG(
			"[router::router_run] no route matched for method %s at %s; selecting 404 handler",
			context->method,
			context->path
		);
    response = router->not_found_handler(context->client_socket);
    send_response(context->client_socket, response);

    return false;
	}

  context->parameters = result->parameters;
  response_t *(*h)(void *) = result->action->handler;

  response = h(context);
  send_response(context->client_socket, response);

	return true;
}

void router_free(router_t *router) {
	free(router);
}

route_t *route_init(ch_array_t *methods, char *path, void*(*handler)(void*)) {
	route_t *route_record = malloc(sizeof(route_t));
	if (!route_record) {
		free(route_record);
		LOG("[router::route_init] %s\n", "failed to allocate route_record");

		return NULL;
	}

	route_record->methods = methods;
	route_record->path = path;
	route_record->handler = handler;

	return route_record;
}

ch_array_t *collect_methods(char *method, ...) {
	ch_array_t *methods = ch_array_init();
	if (methods == NULL) {
		LOG(
			"[router::collect_methods] %s\n",
			"failed to allocate methods array via ch_array_init"
		);

		return NULL;
	}

  va_list args;
	va_start(args, method);

	while (method) {
		if (!ch_array_insert(methods, method)) {
			free(methods);
			LOG("[router::collect_methods] %s\n", "failed to insert into methods array");

			return NULL;
		}

		method = va_arg(args, char *);
	}

	va_end(args);
	return methods;
}
