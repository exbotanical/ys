#include "header.h"

#include <stdio.h>

#include "libhash/libhash.h"
#include "tap.c/tap.h"

typedef struct {
  char *in;
  char *expected;
} test_case;

char expected_tokens[] = {
    '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '0', '1', '2',
    '3', '4', '5', '6', '7', '8',  '9', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L',  'M', 'N', 'O', 'P', 'Q', 'R', 'S',
    'T', 'U', 'W', 'V', 'X', 'Y',  'Z', '^', '_', '`', 'a', 'b', 'c',
    'd', 'e', 'f', 'g', 'h', 'i',  'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v',  'w', 'x', 'y', 'z', '|', '~'};

void test_token_table() {
  unsigned int expected_matches = 0;
  unsigned int actual_matches = 0;

  for (unsigned int i = 0; i < 256; i++) {
    if (token_table[i]) {
      actual_matches++;
    }
  }

  for (unsigned int i = 0; i < sizeof(expected_tokens); i++) {
    if (token_table[expected_tokens[i]]) {
      expected_matches++;
    } else {
      fail("expected token %c to be true in the token table, but it wasn't",
           expected_tokens[i]);
    }
  }

  ok(expected_matches == actual_matches,
     "has the expected number of true values in token table");
}

void test_to_canonical_MIME_header_key() {
  test_case tests[] = {
      {.in = "a-b-c", .expected = "A-B-C"},
      {.in = "a-1-c", .expected = "A-1-C"},
      {.in = "User-Agent", .expected = "User-Agent"},
      {.in = "uSER-aGENT", .expected = "User-Agent"},
      {.in = "user-agent", .expected = "User-Agent"},
      {.in = "USER-AGENT", .expected = "User-Agent"},

      // Other valid char bytes
      {.in = "foo-bar_baz", .expected = "Foo-Bar_baz"},
      {.in = "foo-bar$baz", .expected = "Foo-Bar$baz"},
      {.in = "foo-bar~baz", .expected = "Foo-Bar~baz"},
      {.in = "foo-bar*baz", .expected = "Foo-Bar*baz"},

      // Non-ASCII or anything with spaces or non-token chars remains unchanged
      {.in = "üser-agenT", .expected = "üser-agenT"},
      {.in = "a B", .expected = "a B"},

      // erroneous space
      {.in = "C Ontent-Transfer-Encoding",
       .expected = "C Ontent-Transfer-Encoding"},
      {.in = "foo bar", .expected = "foo bar"},
  };

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    char *in = tests[i].in;
    char *expected = tests[i].expected;

    char *actual = to_canonical_MIME_header_key(in);

    is(expected, actual,
       "canonicalization of '%s' yields the expected value '%s'", in, expected);
  }
}

void test_req_header_get() {
  const char *k1 = "k1";
  const char *v1 = "v1";
  const char *v2 = "v2";
  const char *v3 = "v3";

  hash_table *ht = ht_init(0);
  array_t *arr = array_init();

  array_push(arr, (char *)v1);
  array_push(arr, (char *)v2);
  array_push(arr, (char *)v3);

  ht_insert(ht, k1, arr);

  is(req_header_get(ht, k1), v1, "retrieves the first header value");
  is(req_header_get(ht, v1), NULL,
     "returns NULL if the header key does not exist");
}

void test_req_header_values() {
  const char *k1 = "k1";
  const char *v1 = "v1";
  const char *v2 = "v2";
  const char *v3 = "v3";

  hash_table *ht = ht_init(0);
  array_t *arr = array_init();

  array_push(arr, (char *)v1);
  array_push(arr, (char *)v2);
  array_push(arr, (char *)v3);

  ht_insert(ht, k1, arr);

  is(req_header_values(ht, k1)[0], v1,
     "returns a char array containing all header values");
  is(req_header_values(ht, k1)[1], v2,
     "returns a char array containing all header values");
  is(req_header_values(ht, k1)[2], v3,
     "returns a char array containing all header values");

  is(req_header_values(ht, v1), NULL,
     "returns NULL if the header key does not exist");
}

void test_insert_header() {
  const char *k1 = "k1";
  const char *v1 = "v1";
  const char *singleton = "Content-Length";

  hash_table *ht = ht_init(0);

  ok(insert_header(ht, k1, v1) == 1,
     "successfully inserts the header into the headers hash table");
  is(ht_get(ht, k1), v1, "gets the header value that was just inserted");

  ok(insert_header(ht, singleton, v1) == 1,
     "successfully inserts the header into the headers hash table");
  is(ht_get(ht, singleton), v1, "gets the header value that was just inserted");

  ok(insert_header(ht, singleton, v1) == 0,
     "fails to insert duplicate header of singleton type");
}

int main(int argc, char const *argv[]) {
  plan(21);

  test_token_table();
  test_to_canonical_MIME_header_key();

  test_req_header_get();
  test_req_header_values();

  done_testing();
}
