#ifndef ROUTER_H
#define ROUTER_H

#include "trie.h"
#include "util.h"

#include "lib.hash/hash.h" /* for TODO: */

typedef struct router {
	trie_t* trie;
	handler_t* not_found_handler;
	handler_t* method_not_allowed_handler;
} router_t;

typedef struct route {
	char* methods;
	char* path;
	handler_t handler;
} route_t;

router_t* router_init();

int router_register(const char* path, method methods[], handler_t* handler);

char* regex_cache_get(char* pattern);

#endif /* ROUTER_H */
