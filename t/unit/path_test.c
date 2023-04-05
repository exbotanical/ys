#include "path.h"

#include "tap.c/tap.h"

typedef struct {
  char *name;
  char *input;
  char *expected;
} test_case_t;

void test_expand_path_ok() {
  char *test_str = "/path/to/route";
  array_t *paths = expand_path(test_str);

  ok(array_size(paths) == 3, "only the matched characters are captured");
  is(array_get(paths, 0), "path", "substring is captured");
  is(array_get(paths, 1), "to", "substring is captured");
  is(array_get(paths, 2), "route", "substring is captured");

  array_free(paths);
}

void test_expand_no_match() {
  char *test_str = "path:to:[^route]";

  array_t *paths = expand_path(test_str);
  ok(array_size(paths) == 0, "no matches recorded");

  array_free(paths);
}

void test_derive_label_pattern() {
  test_case_t tests[] = {
      {.name = "BasicRegex", .input = ":id[^\\d+$]", .expected = "^\\d+$"},
      {.name = "EmptyRegex", .input = ":id[]", .expected = NULL},
      {.name = "NoRegex", .input = ":id", .expected = "(.+)"},
      {.name = "LiteralRegex", .input = ":id[xxx]", .expected = "xxx"},
      {.name = "WildcardRegex", .input = ":id[*]", .expected = "*"}};

  for (int i = 0; i < sizeof(tests) / sizeof(test_case_t); i++) {
    test_case_t test_case = tests[i];

    char *pattern = derive_label_pattern(test_case.input);
    is(pattern, test_case.expected, test_case.name);
  }
}

void test_derive_parameter_key() {
  test_case_t tests[] = {
      {.name = "BasicKey", .input = ":id[^\\d+$]", .expected = "id"},
      {.name = "BasicKeyEmptyRegex", .input = ":val[]", .expected = "val"},
      {.name = "BasicKeyWildcardRegex", .input = ":ex[(.*)]", .expected = "ex"},
      {.name = "BasicKeyNoRegex", .input = ":id", .expected = "id"}};

  for (int i = 0; i < sizeof(tests) / sizeof(test_case_t); i++) {
    test_case_t test_case = tests[i];

    char *pattern = derive_parameter_key(test_case.input);
    is(pattern, test_case.expected, test_case.name);
  }
}

void test_path_split_first_delim() {
  typedef struct {
    char *input;
    array_t *expected;
  } test_case;

  test_case tests[] = {
      {.input = "/api", .expected = array_collect("/api", "/")},
      {.input = "/api/demo", .expected = array_collect("/api", "/demo")},
      {.input = "/api/demo/cookie",
       .expected = array_collect("/api", "/demo/cookie")},
      {.input = "/", .expected = array_init()},
      {.input = "", .expected = array_init()},
      {.input = "api", .expected = array_collect("/api", "/")},
      {.input = "api/", .expected = array_collect("/api", "/")},
      {.input = "api/demo", .expected = array_collect("api", "/demo")},
      {.input = "/api/", .expected = array_collect("/api", "/")}

  };

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    array_t *actual = path_split_first_delim(test.input);
    if (array_size(test.expected) == 0) {
      ok(array_size(actual) == 0,
         "returns an empty array if the path could not be split on a slash");
    } else {
      ok(array_size(actual) == array_size(test.expected),
         "returns a tuple if the path was split on a slash");
      is(array_get(actual, 0), array_get(test.expected, 0),
         "returns the expected 'prefix'");
      is(array_get(actual, 1), array_get(test.expected, 1),
         "returns the expected 'suffix'");
    }

    array_free(test.expected);
    array_free(actual);
  }
}

void test_path_split_dir() {
  typedef struct {
    char *input;
    array_t *expected;
  } test_case;

  test_case tests[] = {
      {.input = "/api", .expected = array_collect("/", "api")},
      {.input = "/api/demo", .expected = array_collect("/api/", "demo")},
      {.input = "/api/demo/cookie",
       .expected = array_collect("/api/demo/", "cookie")},
      {.input = "/", .expected = array_collect("/", NULL)},
      {.input = "", .expected = array_init()},
      {.input = "api", .expected = array_collect(NULL, "api")},
      {.input = "api/", .expected = array_collect("api/", NULL)},
      {.input = "api/demo", .expected = array_collect("api/", "demo")},
      {.input = "/api/", .expected = array_collect("/api/", NULL)},
      {.input = "./api/demo/cookie",
       .expected = array_collect("./api/demo/", "cookie")}

  };

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    array_t *actual = path_split_dir(test.input);

    ok(array_size(actual) == 2, "returns a tuple");
    is(array_get(actual, 0), array_get(test.expected, 0),
       "returns the expected 'prefix'");
    is(array_get(actual, 1), array_get(test.expected, 1),
       "returns the expected 'suffix'");

    array_free(test.expected);
    array_free(actual);
  }
}

void test_path_get_pure() {
  char *p = "/some/path/with/query?remove=me";
  is(path_get_pure(p), "/some/path/with/query", "extracts the path sans query");
  is(p, "/some/path/with/query?remove=me",
     "leaves the original path unmodified");
}

void test_path_get_pure_no_query() {
  char *p = "/some/path/with/query";
  is(path_get_pure(p), "/some/path/with/query", "returns the path as-is");
  is(p, "/some/path/with/query", "leaves the original path unmodified");
}

int main() {
  plan(71);

  test_expand_path_ok();
  test_expand_no_match();

  test_derive_label_pattern();
  test_derive_parameter_key();

  test_path_split_first_delim();
  test_path_split_dir();

  test_path_get_pure();
  test_path_get_pure_no_query();

  done_testing();
}
