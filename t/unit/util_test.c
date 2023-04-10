#include "util.h"

#include <ctype.h>

#include "path.h"
#include "tap.c/tap.h"

const char *hex = "0123456789ABCDEF";
const char *nothex = "GHIJKLMNOPQRSTUVWXYZ";

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

  char *ret = str_join(arr, ", ");
  is(ret, "a, b, c");

  array_free(arr);
}

void test_str_cut() {
  const char *ts = "test&string";

  array_t *pair = str_cut(ts, "&");
  is(array_get(pair, 0), "test",
     "first element is the string before the delimiter");
  is(array_get(pair, 1), "string",
     "second element is the string after the delimiter");

  const char *ts2 = "test=string=world";

  array_t *pair2 = str_cut(ts2, "=");
  is(array_get(pair2, 0), "test",
     "first element is the string before the first instance of the delimiter");
  is(array_get(pair2, 1), "string=world",
     "second element is the string after the first instance of the delimiter");

  array_free(pair);
  array_free(pair2);
}

void test_str_cut_nomatch() {
  const char *ts = "test&string";

  array_t *pair = str_cut(ts, "?");
  is(array_get(pair, 0), ts,
     "returns string as-is in first element when no match");

  array_free(pair);
}

void test_str_cut_halfmatch() {
  const char *ts = "test&";

  array_t *pair = str_cut(ts, "&");
  is(array_get(pair, 0), "test",
     "first element is the string before the delimiter");
  is(array_get(pair, 1), NULL,
     "second element is NULL when the string ends with the delimiter");

  const char *ts2 = "&test";

  array_t *pair2 = str_cut(ts2, "&");
  is(array_get(pair2, 0), NULL,
     "first element is NULL when the string begins with the delimiter");
  is(array_get(pair2, 1), "test",
     "second element is the string after the delimiter");

  array_free(pair);
  array_free(pair2);
}

void test_str_contains_ctl_char() {
  ok(str_contains_ctl_char("some\x01") == true,
     "detects the control-byte");  // SOH
  ok(str_contains_ctl_char("some\x02") == true,
     "detects the control-byte");  // STC
  ok(str_contains_ctl_char("some\x03") == true,
     "detects the control-byte");  // ETX
  ok(str_contains_ctl_char("some\x04") == true,
     "detects the control-byte");  // EOT
  ok(str_contains_ctl_char("some\x05") == true,
     "detects the control-byte");  // ENQ
}

void test_ishex() {
  for (unsigned int i = 0; i < strlen(hex); i++) {
    char c = hex[i];
    ok(ishex(c) == true,
       "testing hex characters return true with no case sensitivity");
    ok(ishex(tolower(c)) == true,
       "testing hex characters return true with no case sensitivity");
  }

  for (unsigned int i = 0; i < strlen(nothex); i++) {
    char c = nothex[i];
    ok(ishex(c) == false,
       "testing non-hex characters return false with no case sensitivity");
    ok(ishex(tolower(c)) == false,
       "testing non-hex characters return false with no case sensitivity");
  }
}

int main() {
  plan(97);

  test_split_ok();
  test_split_no_match();
  test_split_empty_input();
  test_split_end_match();

  test_str_join();

  test_str_cut();
  test_str_cut_nomatch();
  test_str_cut_halfmatch();

  test_str_contains_ctl_char();
  test_ishex();

  done_testing();
}
