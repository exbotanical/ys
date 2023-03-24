#include "request.c"

#include <stdio.h>

#include "libhttp.h"
#include "tap.c/tap.h"

static parameter_t *parameter_init(const char *k, const char *v) {
  parameter_t *p = xmalloc(sizeof(parameter_t));
  p->key = k;
  p->value = v;
  return p;
}

static req_t *make_req() {
  req_t *req = malloc(sizeof(req_t));
  req->parameters = array_init();

  array_push(req->parameters, parameter_init("k1", "v1"));
  array_push(req->parameters, parameter_init("k2", "v2"));
  return req;
}

void test_fix_pragma_cache_control() {
  hash_table *headers = ht_init(0);

  insert_header(headers, "Pragma", "no-cache");
  fix_pragma_cache_control(headers);

  is(req_header_get(headers, "Cache-Control"), "no-cache",
     "sets the Cache-Control header to no-cache if Pragma: no-cache and no "
     "Cache-Control header present");

  is(req_header_get(headers, "Pragma"), "no-cache",
     "keeps the Pragma: no-cache header");

  ok(headers->count == 2, "has two header keys");

  // TODO: ADD CACHE_CONTROL AND TEST
}

void test_fix_pragma_cache_control_has_cache_control() {
  hash_table *headers = ht_init(0);

  insert_header(headers, "Pragma", "no-cache");
  insert_header(headers, "Cache-Control", "whatever");

  fix_pragma_cache_control(headers);

  is(req_header_get(headers, "Cache-Control"), "whatever",
     "leaves the Cache-Control header as-is");

  is(req_header_get(headers, "Pragma"), "no-cache",
     "keeps the Pragma: no-cache header");

  ok(headers->count == 2, "has two header keys");
}

void test_fix_pragma_cache_control_no_pragma() {
  hash_table *headers = ht_init(0);

  fix_pragma_cache_control(headers);

  is(req_header_get(headers, "Pragma"), NULL, "does not modify the headers");

  ok(headers->count == 0, "has no header keys");
}

void test_req_get_parameter_at() {
  req_t *req = make_req();

  is(req_get_parameter_at(req, 0)->key, "k1", "returns the correct parameter");
  is(req_get_parameter_at(req, 0)->value, "v1",
     "returns the correct parameter");
  is(req_get_parameter_at(req, 1)->key, "k2", "returns the correct parameter");
  is(req_get_parameter_at(req, 1)->value, "v2",
     "returns the correct parameter");
}

void test_req_get_parameter_at_no_param() {
  req_t *req = malloc(sizeof(req_t));

  is(req_get_parameter_at(req, 0), NULL, "returns NULL if no parameters");
  is(req_get_parameter_at(req, 100), NULL,
     "returns NULL if no parameters no matter the index");
}

void test_req_get_parameter() {
  req_t *req = make_req();

  is(req_get_parameter(req, "k1"), "v1", "returns the correct parameter value");
  is(req_get_parameter(req, "k2"), "v2", "returns the correct parameter value");
  is(req_get_parameter(req, "k3"), NULL,
     "returns the NULL if parameter value not found");
}

void test_req_get_parameter_no_param() {
  req_t *req = malloc(sizeof(req_t));

  is(req_get_parameter(req, "k1"), NULL, "returns NULL if no parameters");
}

void test_req_num_parameters() {
  req_t *req = make_req();

  ok(req_num_parameters(req) == 2, "returns the correct number of parameters");
}

void test_req_num_parameters_no_param() {
  req_t *req = malloc(sizeof(req_t));

  ok(req_num_parameters(req) == 0, "returns the correct number of parameters");
}

void test_req_has_parameters() {
  req_t *req = make_req();

  ok(req_has_parameters(req) == true,
     "returns true if the request has parameters");

  req_t *req2 = malloc(sizeof(req_t));

  ok(req_has_parameters(req2) == false,
     "returns false if the request has no parameters");
}

int main() {
  plan(22);

  test_fix_pragma_cache_control();
  test_fix_pragma_cache_control_has_cache_control();
  test_fix_pragma_cache_control_no_pragma();

  test_req_get_parameter_at();
  test_req_get_parameter_at_no_param();

  test_req_get_parameter();
  test_req_get_parameter_no_param();

  test_req_num_parameters();
  test_req_num_parameters_no_param();

  test_req_has_parameters();

  done_testing();
}
