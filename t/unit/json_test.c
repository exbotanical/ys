#include "json.h"

#include "tap.c/tap.h"

static const char *json =
    "{\"key\":\"value\",\"name\": {\"cookie\": {\"value\": "
    "\"someval\"},\"star\": 4,\"hits\": [1, 2, 3, 4]},\"fork\": \"spoon\"}";

void test_json_getstr_toplvl_happycase() {
  is(json_getstr(json, "fork"), "spoon",
     "retrieves a top-level JSON string value");

  is(json_getstr(json, "key"), "value",
     "retrieves a top-level JSON string value");
}

void test_json_getstr_toplvl_sadcase() {
  is(json_getstr(json, "keyd"), NULL,
     "non-existent top-level key returns NULL");
}

void test_json_getstr_nested_happycase() {
  is(json_getstr(json, "name.cookie.value"), "someval",
     "retrieves a nested JSON string value");
}

void test_json_getstr_nested_sadcase_nomatch() {
  is(json_getstr(json, "name.cookie.valsue"), NULL,
     "non-existent nested key returns NULL");

  is(json_getstr(json, "name.cookie.value.cdc"), NULL,
     "non-existent nested key returns NULL");

  is(json_getstr(json, "name.cookie"), NULL,
     "non-existent nested key returns NULL");
}

void test_json_getstr_nested_sadcase_nonterminal() {
  is(json_getstr(json, "name.cookie"), NULL,
     "non-terminal nested key returns NULL");
}

void test_json_getstr_nested_wrongtype() {
  is(json_getstr(json, "name.star"), NULL, "wrong type returns NULL");
}

void test_json_getint() {
  ok(json_getint(json, "name.star") == 4, "retrieves a nested JSON int value");
}

void test_json_getint_sadcase() {
  ok(json_getint(json, "name.star.v") == 0,
     "non-existent nested key returns 0");
}

int main(int argc, char const *argv[]) {
  plan(11);

  test_json_getstr_toplvl_happycase();
  test_json_getstr_toplvl_sadcase();
  test_json_getstr_nested_happycase();
  test_json_getstr_nested_sadcase_nomatch();
  test_json_getstr_nested_sadcase_nonterminal();
  test_json_getstr_nested_wrongtype();
  test_json_getint();
  test_json_getint_sadcase();

  done_testing();
}
