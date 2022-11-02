#ifndef ROUTER_H
#define ROUTER_H

#include "trie.h"
#include "util.h"
#include "array.h"

#include "lib.hash/hash.h" /* for TODO: */

typedef struct router {
	trie_t *trie;
	void*(*not_found_handler)(void*);
	void*(*method_not_allowed_handler)(void*);
} router_t;

typedef struct route {
	ch_array_t *methods;
	char* path;
	void*(*handler)(void*);
} route_t;

router_t *router_init();

int router_register(const char* path, ch_array_t *methods[], void*(*handler)(void*));

route_t *route_init(ch_array_t *methods, char *path, void*(*handler)(void*));

ch_array_t *collect_methods(char *str, ...);

char* regex_cache_get(char* pattern);

#endif /* ROUTER_H */
