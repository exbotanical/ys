#include "trie.h"

#include "path.h"
#include "router.h"
#include "strdup/strdup.h"
#include "tap.c/tap.h"

typedef struct {
  const char *method;
  const char *path;
} search_query;

typedef struct {
  const char *name;
  search_query search;
  const unsigned int expected_flag;
} test_case;

array_t *collect_methods(http_method_t method, ...) {
  array_t *methods = array_init();
  va_list args;
  va_start(args, method);

  while (method != 0) {
    array_push(methods, strdup(http_method_names[method]));
    method = va_arg(args, http_method_t);
  }

  va_end(args);

  return methods;
}

void *test_handler(void *a, void *b) { return NULL; }

void test_trie_init() {
  trie_t *trie;

  lives_ok({ trie = trie_init(); }, "initializes the trie");

  ok(trie->root != NULL, "root is initialized");
}

void test_trie_insert() {
  route_t records[] = {
      {.path = strdup(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = strdup(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, METHOD_POST, NULL),
       .handler = test_handler},
      {.path = "/test",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/test/path",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/test/path",
       .methods = collect_methods(METHOD_POST, NULL),
       .handler = test_handler},
      {.path = "/test/path/paths",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/foo/bar",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
  };

  trie_t *trie = trie_init();

  for (int i = 0; i < sizeof(records) / sizeof(route_t); i++) {
    route_t route = records[i];

    lives_ok({ trie_insert(trie, route.methods, route.path, route.handler); },
             "inserts the trie node");
  }
}

void test_trie_search_ok() {
  route_t records[] = {{.path = strdup(PATH_ROOT),
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/test",
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/test/path",
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/test/path",
                        .methods = collect_methods(METHOD_POST, NULL),
                        .handler = test_handler},
                       {.path = "/test/path/paths",
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/test/path/:id[^\\d+$]",
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/foo",
                        .methods = collect_methods(METHOD_GET, NULL),
                        .handler = test_handler},
                       {.path = "/bar/:id[^\\d+$]/:user[^\\D+$]",
                        .methods = collect_methods(METHOD_POST, NULL),
                        .handler = test_handler},
                       {.path = "/:*[(.+)]",
                        .methods = collect_methods(METHOD_OPTIONS, NULL),
                        .handler = test_handler}};

  test_case tests[] = {
      {.name = "SearchRoot", .search = {.method = "GET", .path = "/"}},
      {.name = "SearchTrailingPath",
       .search = {.method = "GET", .path = "/test/"}},
      {.name = "SearchWithParams",
       .search = {.method = "GET", .path = "/test/path/12"}},
      {.name = "SearchNestedPath",
       .search = {.method = "GET", .path = "/test/path/paths"}},
      {.name = "SearchPartialPath",
       .search = {.method = "POST", .path = "/test/path"}},
      {.name = "SearchPartialPathOtherMethod",
       .search = {.method = "GET", .path = "/test/path"}},
      {.name = "SearchAdditionalBasePath",
       .search = {.method = "GET", .path = "/foo"}},
      {.name = "SearchAdditionalBasePathTrailingSlash",
       .search = {.method = "GET", .path = "/foo/"}},
      {.name = "SearchComplexRegex",
       .search = {.method = "POST", .path = "/bar/123/alice"}},
      {.name = "SearchWildcardRegex",
       .search = {.method = "OPTIONS", .path = "/wildcard"}}};

  trie_t *trie = trie_init();

  int i;

  for (i = 0; i < sizeof(records) / sizeof(route_t); i++) {
    route_t record = records[i];
    trie_insert(trie, record.methods, record.path, record.handler);
  }

  for (i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    result_t *result = trie_search(trie, test.search.method, test.search.path);

    void *(*h)(void *, void *);

    lives_ok({ h = result->action->handler; },
             "%s test - accessible handler with %s", test.name,
             test.search.path);

    lives_ok(
        {
          h("TEST", "TEST");  // TODO: pass route context and test
        },
        "%s test - callable handler", test.name);
  }

  free(trie);
}

void test_trie_search_no_match() {
  route_t records[] = {
      {.path = strdup(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = strdup(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, METHOD_POST, NULL),
       .handler = test_handler},
      {.path = "/test",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/test/path",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/test/path",
       .methods = collect_methods(METHOD_POST, NULL),
       .handler = test_handler},
      {.path = "/test/path/paths",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = "/test/path/:id[^\\d+$]",
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler}};

  test_case tests[] = {
      {.name = "SearchComplexRegex",
       .search = {.method = "GET", .path = "/test/path/12/31"},
       .expected_flag = NOT_FOUND_MASK},
      {.name = "SearchNestedPath",
       .search = {.method = "GET", .path = "/test/path/path"},
       .expected_flag = NOT_FOUND_MASK},
      {.name = "SearchSpaceInPath",
       .search = {.method = "POST", .path = "/test/pat h"},
       .expected_flag = NOT_FOUND_MASK},
      {.name = "SearchNestedPathAlt",
       .search = {.method = "GET", .path = "/test/path/world"},
       .expected_flag = NOT_FOUND_MASK},
      {.name = "SearchMethodNotRegistered",
       .search = {.method = "DELETE", .path = "/test/path"},
       .expected_flag = NOT_ALLOWED_MASK},
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

    ok(((result->flags & test.expected_flag) == test.expected_flag),
       "%s test - the record contains the appropriate no match flag",
       test.name);
  }

  free(trie);
}

int main(int argc, char *argv[]) {
  plan(34);

  test_trie_init();
  test_trie_insert();
  test_trie_search_ok();
  test_trie_search_no_match();

  done_testing();
}
