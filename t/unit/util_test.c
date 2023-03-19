#include "util.h"

#include "path.h"
#include "tap.c/tap.h"

void test_split_ok() {
  char *test_str = "aa:b:c:d";
  array_t *paths = split(test_str, PARAMETER_DELIMITER);

  ok(array_size(paths) == 4, "only the matched characters are captured");
  is(array_get(paths, 0), "aa", "substring is captured");
  is(array_get(paths, 1), "b", "substring is captured");
  is(array_get(paths, 2), "c", "substring is captured");
  is(array_get(paths, 3), "d", "substring is captured");

  array_free(paths);
}

void test_split_no_match() {
  char *test_str = "a:b:c:d";
  array_t *paths = split(test_str, PATH_ROOT);

  ok(array_size(paths) == 0, "split returns if delimiter not extant");

  array_free(paths);
}

void test_split_empty_input() {
  char *test_str = "";
  array_t *paths = split(test_str, PATH_ROOT);

  ok(array_size(paths) == 0, "split returns if input is empty");

  array_free(paths);
}

void test_split_end_match() {
  char *test_str = "a:b:c:d/test";
  array_t *paths = split(test_str, PATH_ROOT);

  ok(array_size(paths) == 2, "only the matched characters are captured");
  is(array_get(paths, 0), "a:b:c:d", "substring is captured");
  is(array_get(paths, 1), "test", "substring is captured");

  array_free(paths);
}

void test_index_of_ok() {
  char *test_str = "test_str";

  int idx = index_of(test_str, "_");
  ok(idx == 4, "finds the index of the target string");
}

void test_index_of_no_match() {
  char *test_str = "test_str";

  int idx = index_of(test_str, "/");
  ok(idx == -1, "returns -1 indicating no match");
}

void test_substr_ok() {
  char *test_str = "test_str";

  char *substring = substr(test_str, 4, 6, false);

  is(substring, "_s", "substring matches");
}

void test_substr_inclusive() {
  char *test_str = "test_str";

  char *substring = substr(test_str, 4, 6, true);

  is(substring, "_st", "inclusive substring matches");
}

void test_substr_no_range() {
  char *test_str = "test_str";

  char *substring = substr(test_str, 1, 1, false);

  is(substring, NULL, "substring sans range yields empty string");
}

void test_substr_no_range_inclusive() {
  char *test_str = "test_str";

  char *substring = substr(test_str, 1, 1, true);

  is(substring, "e",
     "inclusive substring sans range yields char at start index");
}

void test_safe_strcasecmp() {
  char *s1 = "Content-Type";
  char *s2 = "Content-Type-";
  ok(safe_strcasecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2,
     s1);

  s2 = "Content-Type";
  ok(safe_strcasecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2,
     s1);

  s1 = "CONTENT-TYPE";
  ok(safe_strcasecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2,
     s1);

  s2 = "CoNtenT-TyPe";
  ok(safe_strcasecmp(s1, s2) == 1, "compare s1: '%s' and s2: '%s' is true", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 1, "compare s2: '%s' and s1: '%s' is true", s2,
     s1);

  s1 = "CoNtenT_TyPe";
  ok(safe_strcasecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2,
     s1);

  s2 = "CoNtenT_Ty";
  ok(safe_strcasecmp(s1, s2) == 0, "compare s1: '%s' and s2: '%s' is false", s1,
     s2);
  ok(safe_strcasecmp(s2, s1) == 0, "compare s2: '%s' and s1: '%s' is false", s2,
     s1);
}

void test_str_join() {
  array_t *arr = array_init();
  array_push(arr, "a");
  array_push(arr, "b");
  array_push(arr, "c");

  char *ret = str_join(arr, ",");
  is(ret, "a,b,c");
}

void test_to_upper() {
  char *expected = "HELLO";
  char *ret = to_upper("hello");
  is(expected, ret, "upper-cases the string");

  ret = to_upper("HELLO");
  is(expected, ret, "upper-cases an already upper-cased string");

  ret = to_upper("hEllO");
  is(expected, ret, "upper-cases a partially upper-cased str");
}

test_collect() {
  const char *v1 = "v1";
  const char *v2 = "v2";
  const char *v3 = "v3";
  const char *v4 = "v4";

  const char *values[] = {v1, v2, v3, v4};

  array_t *arr = collect(v1, v2, v3, v4, NULL);
  ok(4 == array_size(arr), "collects only the values provided");

  for (unsigned int i = 0; i < array_size(arr); i++) {
    char *v = (char *)array_get(arr, i);
    is(v, values[i], "collects the correct values");
  }
}

int main() {
  plan(36);

  test_split_ok();
  test_split_no_match();
  test_split_empty_input();
  test_split_end_match();

  test_index_of_ok();
  test_index_of_no_match();

  test_substr_ok();
  test_substr_inclusive();
  test_substr_no_range();
  test_substr_no_range_inclusive();

  test_safe_strcasecmp();

  test_to_upper();

  test_collect();

  done_testing();
}
