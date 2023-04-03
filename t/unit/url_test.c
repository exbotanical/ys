#include "url.h"

#include <stdlib.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "tap.c/tap.h"

typedef struct {
  bool ok;
  char* query;
  array_t* expected;
} parse_test;

typedef struct {
  char* key;
  array_t* value;
} pair;

pair* topair(char* k, array_t* v) {
  pair* p = malloc(sizeof(pair));
  p->key = k;
  p->value = v;

  return p;
}

void test_parse_query() {
  parse_test tests[] = {
      {.query = "a=1",
       .expected = array_collect(topair("a", array_collect("1"))),
       .ok = true},
      {.query = "a=1&b=2",
       .expected = array_collect(topair("a", array_collect("1")),
                                 topair("b", array_collect("2"))),
       .ok = true},
      {.query = "a=1&a=2&a=banana",
       .expected =
           array_collect(topair("a", array_collect("1", "2", "banana"))),
       .ok = true},

      {.query = "ascii=%3Ckey%3A+0x90%3E",
       .expected = array_collect(topair("ascii", array_collect("<key: 0x90>"))),
       .ok = true},
      {.query = "a=1;b=2", .expected = NULL, .ok = false},
      {.query = "a;b=1", .expected = NULL, .ok = false},
      {.query = "a=%3B",  // hex encoding for semicolon
       .expected = array_collect(topair("a", array_collect(";"))),
       .ok = true},
      {.query = "a%3Bb=1",
       .expected = array_collect(topair("a;b", array_collect("1"))),
       .ok = true},
      {.query = "a=1&a=2;a=banana",
       .expected = array_collect(topair("a", array_collect("1"))),
       .ok = true},
      {.query = "a;b&c=1",
       .expected = array_collect(topair("c", array_collect("1"))),
       .ok = true},
      {.query = "a=1&b=2;a=3&c=4",
       .expected = array_collect(topair("a", array_collect("1")),
                                 topair("c", array_collect("4"))),
       .ok = true},
      {.query = "a=1&b=2;c=3",
       .expected = array_collect(topair("a", array_collect("1"))),
       .ok = true},
      {.query = ";", .expected = NULL, .ok = false},
      {.query = "a=1;", .expected = NULL, .ok = false},
      {.query = "a=1&;",
       .expected = array_collect(topair("a", array_collect("1"))),
       .ok = true},
      {.query = ";a=1&b=2",
       .expected = array_collect(topair("b", array_collect("2"))),
       .ok = true},
      {.query = "a=1&b=2;",
       .expected = array_collect(topair("a", array_collect("1"))),
       .ok = true}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(parse_test); i++) {
    parse_test test = tests[i];
    hash_table* ht = parse_query(test.query);

    if (!test.ok) {
      ok(ht->count == 0, "hash table on error has zero elements");
      continue;
    }

    // for each pair in the expected values array
    for (unsigned int j = 0; j < array_size(test.expected); j++) {
      pair* p = array_get(test.expected, j);
      // get the hash table values
      array_t* vs = (array_t*)ht_get(ht, p->key);

      ok(array_size(vs) == array_size(p->value),
         "the hash table contains the correct number of values for key %s",
         p->key);

      // for each value in the pair
      for (unsigned int k = 0; k < array_size(p->value); k++) {
        // get the current value
        char* v = array_get(p->value, k);
        is(array_get(vs, k), v, "each value is in the hash table for key %s",
           p->key);
      }

      array_free(vs);
    }
    array_free(test.expected);
  }
}

void test_has_query_string() {
  typedef struct {
    bool has;
    char* name;
    char* str;
  } test;

  test tests[] = {
      {.name = "NormalQuery",
       .has = true,
       .str = "http://example.com?query=value"},
      {.name = "NormalQueryAfterSlash",
       .has = true,
       .str = "http://example.com/?query=value"},
      {.name = "NormalQueryNoProtocol",
       .has = true,
       .str = "example.com?query=value"},
      {.name = "InvalidQmarkLastChar", .has = false, .str = "example.com?"},
      {.name = "InvalidMultipleQmark",
       .has = false,
       .str = "example.com??query=value"},
      {.name = "InvalidQmarkAfterHash",
       .has = false,
       .str = "example.com#what?query=value"},
      {.name = "InvalidNoQmark",
       .has = false,
       .str = "example.com#what/query=value"},
      {.name = "InvalidQmarWithSlashkAfterHash",
       .has = false,
       .str = "example.com#what/?query=value"},
      {.name = "InvalidMultipleQmarkWithSlash",
       .has = false,
       .str = "example.com/??query=value"},
      {.name = "InvalidQmarkAfterSlashkLastChar",
       .has = false,
       .str = "example.com/?"}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    ok(has_query_string(t.str) == t.has, "%s - %s %s a query", t.name, t.str,
       t.has ? "has" : "does not have");
  }
}

int main() {
  plan(46);

  test_parse_query();
  test_has_query_string();

  done_testing();
}
