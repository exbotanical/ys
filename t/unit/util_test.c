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

void test_str_join() {
  array_t *arr = array_init();
  array_push(arr, "a");
  array_push(arr, "b");
  array_push(arr, "c");

  char *ret = str_join(arr, ",");
  is(ret, "a,b,c");
}

int main() {
  plan(11);

  test_split_ok();
  test_split_no_match();
  test_split_empty_input();
  test_split_end_match();

  test_str_join();

  done_testing();
}
