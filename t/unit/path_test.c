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

int main() {
  plan(14);

  test_expand_path_ok();
  test_expand_no_match();

  test_derive_label_pattern();
  test_derive_parameter_key();

  done_testing();
}
