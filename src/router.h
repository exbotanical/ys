#ifndef ROUTER_H
#define ROUTER_H

#include "buffer.h"
#include "lib.hash/hash.h" /* for TODO: */

static const char* PATH_ROOT = "/";
static const char* PATH_DELIMITER = PATH_ROOT;
static const char* PARAMETER_DELIMITER = ":";
static const char* PARAMETER_DELIMITER_START = "[";
static const char* PARAMETER_DELIMITER_END = "]";
static const char* PATTERN_WILDCARD = "(.+)";

typedef struct node {
	char* label;
	h_table* children;
	h_table* actions;
} node_t;

typedef struct trie {
	node_t* root;
} trie_t;

typedef struct action {
	handler_t handler;
} action_t;

typedef struct parameter {
	char* key;
	char* value;
} parameter_t;

typedef struct result {
	action_t action;
	parameter_t parameter[];
} result_t;

enum method {
	GET,
	HEAD,
	POST,
	PUT,
	PATCH, // RFC 5789
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
};

typedef struct handler {

} handler_t;

typedef struct router {
	trie_t* trie;
	handler_t* not_found_handler;
	handler_t* method_not_allowed_handler;
} router_t;

typedef struct bitfield {

} bitfield_t;

typedef struct route {
	bitfield methods;
	char* path;
	handler_t handler;
} route_t;

static h_table* regex_cache = h_init_table(0);

trie_t* trie_init();
void trie_insert(method methods[], const char* path, handler_t* handler);
result_t* trie_search(method method, const char* search_path);
result_t* result_init();

char* expand_path(const char* path);
char* derive_label_pattern(const char* label);
char* derive_parameter_key(const char* label);

router_t* router_init();

int router_register(const char* path, method methods[], handler_t* handler);

char* regex_cache_get(char* pattern);

#endif /* ROUTER_H */
