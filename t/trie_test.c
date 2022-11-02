#include "tap.c/tap.h"

#include "trie.h"

#include "path.h"
#include "router.h"

#include <string.h>
#include <stdio.h>

void* test_handler(void* arg) {
	return NULL;
}

void test_trie_init() {
	trie_t *trie;

	lives_ok({
		trie = trie_init();
	}, "initializes the trie");

	isnt(trie->root, NULL, "root is initialized");
}

void test_trie_insert() {
	route_t records[] = {
		{
			.path = PATH_ROOT,
			.methods = collect_methods("GET", NULL),
			.handler = test_handler
		},
		{
			.path = PATH_ROOT,
			.methods = collect_methods("GET", "POST", NULL),
			.handler = test_handler,
		},
		{
			.path = "/test",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
		{
			.path = "/test/path",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
		{
			.path = "/test/path",
			.methods = collect_methods("POST", NULL),
			.handler = test_handler,
		},
		{
			.path = "/test/path/paths",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
		{
			.path = "/foo/bar",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
	};

	trie_t *trie = trie_init();

	for (int i = 0; i < sizeof(records) / sizeof(route_t); i++) {
		route_t route = records[i];

		lives_ok({
			trie_insert(trie, route.methods, route.path, route.handler);
		}, "inserts the trie node");
	}
}

int main (int argc, char *argv[]) {
	plan(9);

	test_trie_init();
	test_trie_insert();

  done_testing();
}
