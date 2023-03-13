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

int main() {
  plan(16);

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

  done_testing();
}
