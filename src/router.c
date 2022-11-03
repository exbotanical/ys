#include "router.h"
#include "context.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

router_t *router_init() {
	// TODO: validate
	router_t *router = malloc(sizeof(router_t));
	router->trie = trie_init();

	return router;
}

// sentinel variant
ch_array_t *collect_methods(char *method, ...) {
	ch_array_t *methods = ch_array_init();
  va_list arg;

	va_start(arg, method);

	while (method) {
		ch_array_insert(methods, method);
		method = va_arg(arg, const char *);
	}

	va_end(arg);
	return methods;
}

route_t *route_init(ch_array_t *methods, char *path, void*(*handler)(void*)) {
	route_t *route_record = malloc(sizeof(route_t));
	if (!route_record) {
		return NULL;
	}

	route_record->methods = methods;
	route_record->path = path;
	route_record->handler = handler;

	return route_record;
}

// TODO: doc, val
int router_register(router_t *router, ch_array_t *methods, const char *path, void*(*handler)(void*)) {
	trie_insert(router->trie, methods, path, handler);
	return 1;
}

void router_run(router_t *router, char *method, char *path) {
	result_t *result = trie_search(router->trie, method, path); // TODO: args same order
	if (!result) { // TODO: NotFound vs NotAllowed
		// return router->not_found_handler;
		return;
	}

	void *(*h)(void *) = result->action->handler;
	route_context_t *context = route_context_init(method, path, result->parameters);

	h(context);
}
