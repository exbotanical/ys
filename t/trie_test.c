#include "tap.c/tap.h"

#include "trie.h"

#include "path.h"
#include "router.h"

#include <string.h>
#include <stdio.h>

void* test_handler(void* arg) {
	// printf("RESULT %s\n", arg);
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

void test_trie_search_ok () {
	typedef struct {
		char *method;
		char *path;
	} search_query;

	typedef struct {
		char *name;
		search_query search;
	} test_case;

	route_t records[] = {
		{
			.path = PATH_ROOT,
			.methods = collect_methods("GET", NULL),
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
			.path = "/test/path/:id[^\\d+$]",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
		{
			.path = "/foo",
			.methods = collect_methods("GET", NULL),
			.handler = test_handler,
		},
		{
			.path = "/bar/:id[^\\d+$]/:user[^\\D+$]",
			.methods = collect_methods("POST", NULL),
			.handler = test_handler,
		},
		{
			.path = "/:*[(.+)]",
			.methods = collect_methods("OPTIONS", NULL),
			.handler = test_handler,
		}
	};

	test_case tests[] = {
		{
			.name = "SearchRoot",
			.search = {
				.method = "GET",
				.path =  "/",
			}
		},
		{
			.name = "SearchTrailingPath",
			.search = {
				.method = "GET",
				.path = "/test/",
			},
		},
		{
			.name = "SearchWithParams",
			.search = {
				.method = "GET",
				.path = "/test/path/12",
			},
		},
		{
			.name = "SearchNestedPath",
			.search = {
				.method = "GET",
				.path = "/test/path/paths",
			},
		},
		{
			.name = "SearchPartialPath",
			.search = {
				.method = "POST",
				.path = "/test/path",
			},
		},
		{
			.name = "SearchPartialPathOtherMethod",
			.search = {
				.method = "GET",
				.path = "/test/path",
			},
		},
		{
			.name = "SearchAdditionalBasePath",
			.search = {
				.method = "GET",
				.path = "/foo",
			},
		},
		{
			.name = "SearchAdditionalBasePathTrailingSlash",
			.search = {
				.method = "GET",
				.path = "/foo/",
			},
		},
		{
			.name = "SearchComplexRegex",
			.search = {
				.method = "POST",
				.path = "/bar/123/alice",
			},
		},
		{
			.name = "SearchWildcardRegex",
			.search = {
				.method = "OPTIONS",
				.path = "/wildcard",
			},
		},
	};

	trie_t *trie = trie_init();

	int i;

	for (i = 0; i < sizeof(records) / sizeof(route_t); i++) {
		route_t record = records[i];
		trie_insert(trie, record.methods, record.path, record.handler);
	}

	for (i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
		test_case test = tests[i];

		result_t *result = trie_search(trie, test.search.method, test.search.path);

		void *(*h)(void *);

		lives_ok({
			h = result->action->handler;
		}, "%s test - accessible handler with %s", test.name, test.search.path);

		lives_ok({
			h("DUDE");
		}, "%s test - callable handler", test.name);
	}

	free(trie);
}

int main (int argc, char *argv[]) {
	plan(29);

	test_trie_init();
	test_trie_insert();
	test_trie_search_ok();

  done_testing();
}
