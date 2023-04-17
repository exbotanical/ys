#include "request.c"

#include <stdio.h>

#include "libys.h"
#include "tap.c/tap.h"
#include "tests.h"

static request *make_req(void) {
  request_internal *req = malloc(sizeof(request));
  req->parameters = ht_init(0);

  ht_insert(req->parameters, "k1", "v1");
  ht_insert(req->parameters, "k2", "v2");

  return (request *)req;
}

void test_fix_pragma_cache_control(void) {
  hash_table *headers = ht_init(0);

  insert_header(headers, PRAGMA, NO_CACHE, true);
  fix_pragma_cache_control(headers);

  is(get_first_header(headers, CACHE_CONTROL), NO_CACHE,
     "sets the Cache-Control header to no-cache if Pragma: no-cache and no "
     "Cache-Control header present");

  is(get_first_header(headers, PRAGMA), NO_CACHE,
     "keeps the Pragma: no-cache header");

  ok(headers->count == 2, "has two header keys");
}

void test_fix_pragma_cache_control_has_cache_control(void) {
  hash_table *headers = ht_init(0);

  insert_header(headers, PRAGMA, NO_CACHE, true);
  insert_header(headers, CACHE_CONTROL, "whatever", true);

  fix_pragma_cache_control(headers);

  is(get_first_header(headers, CACHE_CONTROL), "whatever",
     "leaves the Cache-Control header as-is");

  is(get_first_header(headers, PRAGMA), NO_CACHE,
     "keeps the Pragma: no-cache header");

  ok(headers->count == 2, "has two header keys");
}

void test_fix_pragma_cache_control_no_pragma(void) {
  hash_table *headers = ht_init(0);

  fix_pragma_cache_control(headers);

  is(get_first_header(headers, PRAGMA), NULL, "does not modify the headers");

  ok(headers->count == 0, "has no header keys");
}

void test_req_get_parameter(void) {
  request *req = make_req();

  is(req_get_parameter(req, "k1"), "v1", "returns the correct parameter value");
  is(req_get_parameter(req, "k2"), "v2", "returns the correct parameter value");
  is(req_get_parameter(req, "k3"), NULL,
     "returns the NULL if parameter value not found");
}

void test_req_get_parameter_no_param(void) {
  request *req = malloc(sizeof(request));

  is(req_get_parameter(req, "k1"), NULL, "returns NULL if no parameters");
}

void test_req_num_parameters(void) {
  request *req = make_req();

  ok(req_num_parameters(req) == 2, "returns the correct number of parameters");
}

void test_req_num_parameters_no_param(void) {
  request *req = malloc(sizeof(request));

  ok(req_num_parameters(req) == 0, "returns the correct number of parameters");
}

void test_req_has_parameters(void) {
  request *req = make_req();

  ok(req_has_parameters(req) == true,
     "returns true if the request has parameters");

  request *req2 = malloc(sizeof(request));

  ok(req_has_parameters(req2) == false,
     "returns false if the request has no parameters");
}

void run_request_tests(void) {
  test_fix_pragma_cache_control();
  test_fix_pragma_cache_control_has_cache_control();
  test_fix_pragma_cache_control_no_pragma();

  test_req_get_parameter();
  test_req_get_parameter_no_param();

  test_req_num_parameters();
  test_req_num_parameters_no_param();

  test_req_has_parameters();
}
