#include "response.c"

#include <stdarg.h>
#include <stdlib.h>

#include "header.h"
#include "tap.c/tap.h"
#include "tests.h"

void test_is_2xx_connect(void) {
  request_internal* req = malloc(sizeof(req));
  response_internal* res = response_init();

  req->method = "CONNECT";
  ys_set_status((ys_response*)res, YS_STATUS_NO_CONTENT);
  ok(is_2xx_connect(req, res) == true,
     "a 204 CONNECT request is a 2xx connect");

  req->method = "CONNECT";
  ys_set_status((ys_response*)res, YS_STATUS_NOT_FOUND);
  ok(is_2xx_connect(req, res) == false,
     "a 404 CONNECT request is not a 2xx connect");

  req->method = "OPTIONS";
  ys_set_status((ys_response*)res, YS_STATUS_NO_CONTENT);
  ok(is_2xx_connect(req, res) == false,
     "a 204 OPTIONS request is not a 2xx connect");
}

void test_is_informational(void) {
  response_internal* res = response_init();

  ys_set_status((ys_response*)res, YS_STATUS_EARLY_HINTS);
  ok(is_informational(res) == true, "a 1xx response is informational");

  ys_set_status((ys_response*)res, YS_STATUS_OK);
  ok(is_informational(res) == false, "a 2xx response is not informational");
}

void test_is_nocontent(void) {
  response_internal* res = response_init();

  ys_set_status((ys_response*)res, YS_STATUS_NO_CONTENT);
  ok(is_nocontent(res) == true, "a 204 response is a no content response");

  ys_set_status((ys_response*)res, YS_STATUS_OK);
  ok(is_nocontent(res) == false, "a 200 response is not a no content response");
}

void test_response_serialize(void) {
  typedef struct {
    char* name;
    char* expected;
    char* method;
    char* body;
    int status;
    hash_table* headers;
  } test_case;

  test_case tests[] = {
      {.name = "NormalRequest",
       .body = "{\"test\":\"body\"}",
       .headers = to_headers(
           to_header("Vary", array_collect("Origin")),
           to_header("X-Powered-By", array_collect("Unit-Test")), NULL),
       .method = "GET",
       .status = YS_STATUS_OK,
       .expected = "HTTP/1.1 200 OK\r\nVary: Origin\r\nX-Powered-By: "
                   "Unit-Test\r\nContent-Type: text/plain\r\nContent-Length: "
                   "15\r\n\r\n{\"test\":\"body\"}"},

      {.name = "MultipleValuesSameHeaderKey",
       .body = "body",
       .status = YS_STATUS_ACCEPTED,
       .method = "POST",
       .headers = to_headers(
           to_header("Vary", array_collect("Origin", "Request-Method", "Etc")),
           NULL),
       .expected = "HTTP/1.1 202 Accepted\r\nVary: "
                   "Origin, Request-Method, Etc\r\nContent-Type: "
                   "text/plain\r\nContent-Length: 4\r\n\r\nbody"},

      {.name = "OmitContentLength_For204",
       .body = "body",
       .status = YS_STATUS_NO_CONTENT,
       .method = "OPTIONS",
       .headers = ht_init(0),
       .expected = "HTTP/1.1 204 No Content\r\n\r\n"},

      {.name = "OmitContentLength_ForInformational",
       .body = "test",
       .status = YS_STATUS_SWITCHING_PROTOCOLS,
       .method = "HEAD",
       .headers = ht_init(0),
       .expected = "HTTP/1.1 101 Switching Protocols\r\n\r\n"},

      {.name = "ContentLengthZero_WhenNoBody",
       .body = NULL,
       .headers = to_headers(
           to_header("Vary", array_collect("Origin")),
           to_header("X-Powered-By", array_collect("Unit-Test")), NULL),
       .method = "GET",
       .status = YS_STATUS_OK,
       .expected = "HTTP/1.1 200 OK\r\nVary: Origin\r\nX-Powered-By: "
                   "Unit-Test\r\nContent-Length: 0\r\n\r\n"},

      {.name = "NoContentLength_When2xxConnect",
       .body = "body",
       .status = YS_STATUS_NO_CONTENT,
       .method = "CONNECT",
       .headers = ht_init(0),
       .expected = "HTTP/1.1 204 No Content\r\n\r\n"},
      {.name = "AddsTextPlainContentType_WhenGivenBodyAndNotSet",
       .body = "body",
       .status = YS_STATUS_OK,
       .method = "CONNECT",
       .headers = ht_init(0),
       .expected = "HTTP/1.1 200 OK\r\n\r\n"}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    request_internal* req = malloc(sizeof(request_internal));
    req->method = test.method;

    response_internal* res = response_init();
    res->headers = test.headers;
    ys_set_body((ys_response*)res, test.body);
    ys_set_status((ys_response*)res, test.status);

    char* response = buffer_state(response_serialize(req, res));

    is(response, test.expected,
       "the response string is serialized properly and compliant to RFC 7230");
  }
}

void ys_set_body_test(void) {
  response_internal* res = response_init();

  ys_set_body((ys_response*)res, "%s - %s\n%s - %s \t\t%d\n", "test", "xtestx",
              "cookie", "x\t cookie\t x", 20);
  is(res->body, "test - xtestx\ncookie - x\t cookie\t x \t\t20\n");
}

void run_response_tests(void) {
  test_is_2xx_connect();
  test_is_informational();
  test_is_nocontent();

  test_response_serialize();

  ys_set_body_test();
}
