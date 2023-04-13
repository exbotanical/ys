#include "trie.h"

#include "libutil/libutil.h"
#include "path.h"
#include "router.h"
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

array_t *collect_methods(http_method method, ...) {
  array_t *methods = array_init();
  va_list args;
  va_start(args, method);

  while (method != 0) {
    array_push(methods, s_copy(http_method_names[method]));
    method = va_arg(args, http_method);
  }

  va_end(args);

  return methods;
}

void *test_handler(void *a, void *b) { return NULL; }

void test_trie_init(void) {
  route_trie *trie;

  lives_ok({ trie = trie_init(); }, "initializes the trie");

  ok(trie->root != NULL, "root is initialized");
}

void test_trie_insert(void) {
  route_record records[] = {
      {.path = s_copy(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = s_copy(PATH_ROOT),
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
       .handler = test_handler}};

  route_trie *trie = trie_init();

  for (int i = 0; i < sizeof(records) / sizeof(route_record); i++) {
    route_record route = records[i];

    lives_ok({ trie_insert(trie, route.methods, route.path, route.handler); },
             "inserts the trie node");

    array_free(route.methods);
  }
}

void test_trie_search_ok(void) {
  route_record records[] = {{.path = s_copy(PATH_ROOT),
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
                             .handler = test_handler},
                            {.path = "/futon",
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
       .search = {.method = "OPTIONS", .path = "/wildcard"}},
      {.name = "WithQueryStringIgnored",
       .search = {.method = "OPTIONS", .path = "/futon/?ohno=1&heu=2"}}};

  route_trie *trie = trie_init();

  unsigned int i;

  for (i = 0; i < sizeof(records) / sizeof(route_record); i++) {
    route_record record = records[i];
    trie_insert(trie, record.methods, record.path, record.handler);

    array_free(record.methods);
  }

  for (i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    route_result *result =
        trie_search(trie, test.search.method, test.search.path);

    void *(*h)(void *, void *);

    lives_ok({ h = result->action->handler; },
             "%s test - accessible handler with %s", test.name,
             test.search.path);

    lives_ok({ h("TEST", "TEST"); }, "%s test - callable handler", test.name);
  }

  free(trie);
}

void test_trie_search_no_match(void) {
  route_record records[] = {
      {.path = s_copy(PATH_ROOT),
       .methods = collect_methods(METHOD_GET, NULL),
       .handler = test_handler},
      {.path = s_copy(PATH_ROOT),
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

  test_case tests[] = {{.name = "SearchComplexRegex",
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
                        .expected_flag = NOT_ALLOWED_MASK}};

  route_trie *trie = trie_init();

  int i;

  for (i = 0; i < sizeof(records) / sizeof(route_record); i++) {
    route_record record = records[i];
    trie_insert(trie, record.methods, record.path, record.handler);
  }

  for (i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    route_result *result =
        trie_search(trie, test.search.method, test.search.path);

    ok(((result->flags & test.expected_flag) == test.expected_flag),
       "%s test - the record contains the appropriate no match flag",
       test.name);
  }

  free(trie);
}

void test_trie_search_ignore_trailing_slash(void) {
  route_trie *trie = trie_init();

  trie_insert(trie, array_collect("GET"), "/foo", test_handler);
  route_result *r = trie_search(trie, "GET", "/foo/");
  isnt(r, NULL, "trie search ignores trailing slash");

  lives_ok({ r->action->handler(NULL, NULL); },
           "TrieSearchIgnoreTrailingSlash - callable handler");

  trie_insert(trie, array_collect("GET"), "/bar/", test_handler);
  route_result *r2 = trie_search(trie, "GET", "/bar/");
  isnt(r2, NULL, "trie insert ignores trailing slash");

  lives_ok({ r2->action->handler(NULL, NULL); },
           "TrieSearchIgnoreTrailingSlash - callable handler");

  free(trie);
}

void test_trie_search_with_queries(void) {
  route_trie *trie = trie_init();

  trie_insert(trie, array_collect("GET"), "/foo", test_handler);

  route_result *r = trie_search(trie, "GET", "/foo/?cookie=1&value=12");

  ht_record *rr = ht_search(r->queries, "cookie");

  is(array_get(ht_get(r->queries, "cookie"), 0), "1",
     "stores the query key/value pairs");
  is(array_get(ht_get(r->queries, "value"), 0), "12",
     "stores the query key/value pairs");
  ok(r->queries->count, "has only 2 query results");

  free(trie);
}

void test_trie_search_april2023_bugs(void) {
  route_trie *trie = trie_init();

  trie_insert(trie, array_collect("GET"), "/", test_handler);
  route_result *r = trie_search(trie, "GET", "/?cookie");

  ok(r->queries->count == 0,
     "query count is zero when invalid query (missing value)");
  ok(r->action->handler == test_handler,
     "returns the root path handler, ignoring the query");

  route_result *r2 = trie_search(trie, "GET", "/?cookie=1&noop&value=2");
  ok(r2->queries->count == 2,
     "query count is commensurate with number of valid queries");
}

int main() {
  plan(46);

  test_trie_init();
  test_trie_insert();
  test_trie_search_ok();
  test_trie_search_no_match();
  test_trie_search_ignore_trailing_slash();
  test_trie_search_with_queries();

  test_trie_search_april2023_bugs();

  done_testing();
}
