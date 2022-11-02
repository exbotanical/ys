#include "router.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

router_t *router_init() {
	// TODO: validate
	router_t *router = malloc(sizeof(router_t));
	return router;
}

// sentinel variant
ch_array_t *collect_methods(char *str, ...) {
	ch_array_t *methods = ch_array_init(0);
  va_list arg;

	va_start(arg, str);

	while (str) {
		ch_array_insert(methods, str);
		str = va_arg(arg, const char *);
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
