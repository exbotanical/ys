#include "cors.h"

#include <stdarg.h>
#include <stdlib.h>  // for malloc
#include <string.h>  // for strcmp

#include "libhash/libhash.h"
#include "libhttp.h"
#include "tap.c/tap.h"

// TODO: cleanup test helpers
static const char *all_headers[] = {"Vary",
                                    "Access-Control-Allow-Origin",
                                    "Access-Control-Allow-Methods",
                                    "Access-Control-Allow-Headers",
                                    "Access-Control-Allow-Credentials",
                                    "Access-Control-Allow-Private-Network",
                                    "Access-Control-Max-Age",
                                    "Access-Control-Expose-Headers"};

static array_t *toarr(void *s, ...) {
  array_t *arr = array_init();

  va_list args;
  va_start(args, s);

  array_push(arr, s);
  while ((s = va_arg(args, void *))) {
    array_push(arr, s);
  }

  va_end(args);

  return arr;
}

inline static array_t *toheaders(char *s, ...) {
  array_t *arr = array_init();
  va_list args;
  va_start(args, s);

  for (unsigned int i = 0; s != NULL; i++) {
    header_t *h = malloc(sizeof(header_t));
    h->key = s;
    h->value = va_arg(args, char *);

    array_push(arr, h);

    s = va_arg(args, char *);
  }

  va_end(args);
  return arr;
}

void insert_headers(req_t *req, array_t *headers) {
  for (unsigned int i = 0; i < array_size(headers); i++) {
    header_t *h = array_get(headers, i);

    // We're not using this helper for duplicate headers, so we can assume the
    // header hasn't been inserted before
    // TODO: use method that does this internally so we never have to worry
    // about whether the value array_t was initialized
    array_t *v = array_init();
    array_push(v, (void *)h->value);

    ht_insert(req->headers, h->key, v);
  }
}

void check_status_code(res_t *res, int expected_status) {
  if (expected_status != res->status) {
    fail("expected status code to be %d but got %d. ", expected_status,
         res->status);
  }
}

bool m(void *h, void *v) {
  return strcmp(((header_t *)h)->key, (char *)v) == 0;
}

void check_headers(char *test_name, array_t *actual, array_t *expected) {
  for (unsigned int i = 0; i < sizeof(all_headers) / sizeof(char *); i++) {
    const char *key = all_headers[i];

    int eidx = array_find(expected, m, key);
    int aidx = array_find(actual, m, key);

    // If *both* are NULL, they're equal, so continue
    if (eidx == -1 && aidx == -1) {
      continue;
    } else if (eidx == -1 || aidx == -1) {
      fail("headers do not match: %s was not found in %s\n", key,
           eidx == -1 ? "expected" : "actual");
    }

    header_t *expected_h = array_get(expected, eidx);
    header_t *actual_h = array_get(actual, aidx);
    is(actual_h->value, expected_h->value, "%s - headers for key '%s' match",
       test_name, key);
  }
}

cors_opts_t *make_opts(array_t *allowed_origins, array_t *allowed_methods,
                       array_t *allowed_headers, array_t *expose_headers,
                       bool allow_credentials) {
  cors_opts_t *o = malloc(sizeof(cors_opts_t));
  o->allowed_origins = allowed_origins;
  o->allowed_methods = allowed_methods;
  o->allowed_headers = allowed_headers;
  o->expose_headers = expose_headers;
  o->allow_credentials = allow_credentials;

  return o;
}

void test_cors_middleware() {
  typedef struct {
    char *name;
    cors_opts_t *options;
    http_method_t method;
    array_t *req_headers;
    array_t *res_headers;
    http_status_t code;
  } test_case;

  test_case tests[] = {
      {
          .name = "AllOriginAllowed",
          .options = make_opts(toarr("*"), NULL, NULL, NULL, false),
          .method = GET,
          .req_headers = toheaders("Origin", "http://foo.com", NULL),
          .res_headers = toheaders("Vary", "Origin",
                                   "Access-Control-Allow-Origin", "*", NULL),
          .code = OK,
      },
      {
          .name = "OriginAllowed",
          .options =
              make_opts(toarr("http://foo.com"), NULL, NULL, NULL, false),
          .method = GET,
          .req_headers = toheaders("Origin", "http://foo.com", NULL),
          .res_headers =
              toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                        "http://foo.com", NULL),
          .code = OK,
      },
      {.name = "OriginAllowedMultipleProvided",
       .options = make_opts(toarr("http://foo.com", "http://bar.com"), NULL,
                            NULL, NULL, false),
       .method = GET,
       .req_headers = toheaders("Origin", "http://bar.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://bar.com", NULL),
       .code = OK},
      {.name = "GETMethodAllowedDefault",
       .options = make_opts(toarr("http://foo.com"), NULL, NULL, NULL, false),
       .method = GET,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = OK},
      {.name = "POSTMethodAllowedDefault",
       .options = make_opts(toarr("http://foo.com"), NULL, NULL, NULL, false),
       .method = POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = OK},
      {.name = "HEADMethodAllowedDefault",
       .options = make_opts(toarr("http://foo.com"), NULL, NULL, NULL, false),
       .method = HEAD,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = OK},
      {.name = "MethodAllowed",
       // TODO: allow just enum
       .options = make_opts(toarr("*"), toarr(http_method_names[DELETE]), NULL,
                            NULL, false),
       .method = OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[DELETE], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,Access-"
                                "Control-Request-Headers",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Methods",
                                http_method_names[DELETE], NULL),
       .code = NO_CONTENT},
      {.name = "HeadersAllowed",
       .options = make_opts(toarr("*", NULL), NULL, toarr("X-Testing", NULL),
                            NULL, false),
       .method = OPTIONS,
       .req_headers =
           toheaders("Origin", "http://foo.com",
                     "Access-Control-Request-Method", http_method_names[GET],
                     "Access-Control-Request-Headers", "X-Testing", NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Headers", "X-Testing",
                                "Access-Control-Allow-Methods",
                                http_method_names[GET], NULL),
       .code = NO_CONTENT},
      {.name = "HeadersAllowedMultiple",
       .options = make_opts(
           toarr("*", NULL), NULL,
           toarr("X-Testing", "X-Testing-2", "X-Testing-3", NULL), NULL, false),
       .method = OPTIONS,
       .req_headers = toheaders(
           "Origin", "http://foo.com", "Access-Control-Request-Method",
           http_method_names[GET], "Access-Control-Request-Headers",
           "X-Testing, X-Testing-2, X-Testing-3", NULL),
       .res_headers = toheaders(
           "Vary",
           "Origin,Access-Control-Request-Method,"
           "Access-Control-Request-Headers",
           "Access-Control-Allow-Origin", "*", "Access-Control-Allow-Headers",
           "X-Testing, X-Testing-2, X-Testing-3",
           "Access-Control-Allow-Methods", http_method_names[GET], NULL),
       .code = NO_CONTENT},
      {.name = "CredentialsAllowed",
       .options = make_opts(toarr("*", NULL), NULL, NULL, NULL, true),
       .method = OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[GET], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                "Access-Control-Allow-Credentials", "true",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Methods",
                                http_method_names[GET], NULL),
       .code = NO_CONTENT},
      {.name = "ExposeHeaders",
       .options = make_opts(toarr("http://foo.com", NULL), NULL, NULL,
                            toarr("x-test", NULL), false),
       .method = POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders(
           "Vary", "Origin", "Access-Control-Allow-Origin", "http://foo.com",
           "Access-Control-Expose-Headers", "x-test", NULL),
       .code = OK},
      {.name = "ExposeHeadersMultiple",
       .options = make_opts(toarr("http://foo.com", NULL), NULL, NULL,
                            toarr("x-test-1", "x-test-2", NULL), false),
       .method = POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders(
           "Vary", "Origin", "Access-Control-Allow-Origin", "http://foo.com",
           "Access-Control-Expose-Headers", "x-test-1, x-test-2", NULL),
       .code = OK},

      // CORS Rejections
      {.name = "OriginNotAllowed",
       .options =
           make_opts(toarr("http://foo.com", NULL), NULL, NULL, NULL, false),
       .method = GET,
       .req_headers = toheaders("Origin", "http://bar.com", NULL),
       .res_headers = toheaders("Vary", "Origin", NULL),
       .code = OK},
      {.name = "OriginNotAllowedPortMismatch",
       .options = make_opts(toarr("http://foo.com:443", NULL), NULL, NULL, NULL,
                            false),
       .method = GET,
       .req_headers = toheaders("Origin", "http://foo.com:444", NULL),
       .res_headers = toheaders("Vary", "Origin", NULL),
       .code = OK},
      {.name = "MethodNotAllowed",
       .options = make_opts(toarr("*", NULL), NULL, NULL, NULL, false),
       .method = OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[DELETE], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                NULL),
       .code = NO_CONTENT},
      {.name = "HeadersNotAllowed",
       .options = make_opts(toarr("*", NULL), NULL, NULL, NULL, false),
       .method = OPTIONS,
       .req_headers =
           toheaders("Origin", "http://foo.com",
                     "Access-Control-Request-Method", http_method_names[GET],
                     "Access-Control-Request-Headers", "X-Testing", NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                NULL),
       .code = NO_CONTENT}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    // Build mock request
    req_t *req = &(req_t){
        .method = http_method_names[test.method],
    };
    req->headers = ht_init(0);

    // Add headers to mock request
    insert_headers(req, test.req_headers);

    res_t *res = malloc(sizeof(res_t));
    res->headers = array_init();

    // Run CORS handler
    cors_handler(cors_init(test.options), req, res);

    // Evaluate resulting headers and status
    check_headers(test.name, res->headers, test.res_headers);

    // check_status_code(res, (int)test.code); // TODO:
  }
}

void test_derive_headers() {
  req_t *req = malloc(sizeof(req_t));
  req->headers = ht_init(0);

  array_t *values = array_init();
  array_push(values, "x-test-1, x-test-2, x-test-3");

  ht_insert(req->headers, REQUEST_HEADERS_HEADER, values);

  array_t *expected = toarr("x-test-1", "x-test-2", "x-test-3", NULL);
  array_t *actual = derive_headers(req);

  for (unsigned int i = 0; i < array_size(expected); i++) {
    char *e = array_get(expected, i);
    char *a = array_get(actual, i);

    is(e, a, "derive_headers - expected %s to equal %s", e, a);
  }
}

void test_are_headers_allowed() {
  typedef struct {
    array_t *test_headers;
    bool is_allowed
  } test_case;

  typedef struct {
    char *name;
    cors_t *cors;
    array_t *cases;
  } test;

  test tests[] = {
      {.name = "ExplicitHeaders",
       .cors = cors_init(make_opts(toarr("*", NULL), NULL,
                                   toarr("x-test-1", "x-test-2", NULL), NULL,
                                   false)),
       .cases = toarr(
           &(test_case){.test_headers = toarr("x-test-1", "x-test-2", NULL),
                        .is_allowed = true},
           &(test_case){
               .test_headers = toarr("x-test", "x-test-1", "x-test-2", NULL),
               .is_allowed = false},
           &(test_case){.test_headers = toarr("", NULL), .is_allowed = false},
           NULL)},
      {.name = "WildcardHeaders",
       .cors = cors_init(
           make_opts(toarr("*", NULL), NULL, toarr("*", NULL), NULL, false)),
       .cases = toarr(
           &(test_case){.test_headers = toarr("x-test-1", "x-test-2", NULL),
                        .is_allowed = true},
           &(test_case){
               .test_headers = toarr("x-test", "x-test-1", "x-test-2", NULL),
               .is_allowed = true},
           &(test_case){.test_headers = toarr("", NULL), .is_allowed = true},
           NULL)}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    for (unsigned int j = 0; j < array_size(t.cases); j++) {
      test_case *c = (test_case *)array_get(t.cases, j);

      bool actual = are_headers_allowed(t.cors, c->test_headers);
      ok(actual == c->is_allowed, "%s - headers are %s", t.name,
         c->is_allowed ? "allowed" : "not allowed");
    }
  }
}

void test_is_method_allowed() {
  typedef struct {
    char *method;
    bool is_allowed;
  } test_case;

  typedef struct {
    char *name;
    cors_t *cors;
    array_t *cases;
  } test;

  test tests[] = {
      {.name = "ExplicitMethod",
       .cors = cors_init(make_opts(
           toarr("*", NULL),
           toarr(http_method_names[DELETE], http_method_names[PUT], NULL), NULL,
           NULL, false)),
       .cases = toarr(
           &(test_case){.method = http_method_names[DELETE],
                        .is_allowed = true},
           &(test_case){.method = http_method_names[PUT], .is_allowed = true},
           &(test_case){.method = http_method_names[PATCH],
                        .is_allowed = false},
           NULL)}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    for (unsigned int j = 0; j < array_size(t.cases); j++) {
      test_case *c = (test_case *)array_get(t.cases, j);

      bool actual = is_method_allowed(t.cors, c->method);
      ok(actual == c->is_allowed, "%s - method %s is %s", t.name, c->method,
         c->is_allowed ? "allowed" : "not allowed");
    }
  }
}

void test_origin_is_allowed() {
  typedef struct {
    char *origin;
    bool is_allowed;
  } test_case;

  typedef struct {
    char *name;
    cors_t *cors;
    array_t *cases;
  } test;

  test tests[] = {
      {.name = "ExplicitOrigin",
       .cors = cors_init(
           make_opts(toarr("http://foo.com", "http://bar.com", "baz.com", NULL),
                     NULL, NULL, NULL, false)),
       .cases =
           toarr(&(test_case){.origin = "http://foo.com", .is_allowed = true},
                 &(test_case){.origin = "http://bar.com", .is_allowed = true},
                 &(test_case){.origin = "baz.com", .is_allowed = true},
                 &(test_case){.origin = "http://foo.com/", .is_allowed = false},
                 &(test_case){.origin = "https://bar.com", .is_allowed = false},
                 &(test_case){.origin = "http://baz.com", .is_allowed = false},
                 &(test_case){.origin = "null",  // file redirect
                              .is_allowed = true},
                 NULL)},
      {.name = "WildcardOrigin",
       .cors = cors_init(make_opts(toarr("*", NULL), NULL, NULL, NULL, false)),
       .cases =
           toarr(&(test_case){.origin = "http://foo.com", .is_allowed = true},
                 &(test_case){.origin = "http://bar.com", .is_allowed = true},
                 &(test_case){.origin = "baz.com", .is_allowed = true},
                 &(test_case){.origin = "http://foo.com/", .is_allowed = true},
                 &(test_case){.origin = "https://bar.com", .is_allowed = true},
                 &(test_case){.origin = "http://baz.com", .is_allowed = true},
                 NULL)}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    for (unsigned int j = 0; j < array_size(t.cases); j++) {
      test_case *c = (test_case *)array_get(t.cases, j);

      bool actual = is_origin_allowed(t.cors, c->origin);
      ok(actual == c->is_allowed, "%s - origin %s is %s", t.name, c->origin,
         c->is_allowed ? "allowed" : "not allowed");
    }
  }
}

int main(int argc, char const *argv[]) {
  plan(62);

  test_cors_middleware();
  test_derive_headers();
  test_are_headers_allowed();
  test_is_method_allowed();
  test_origin_is_allowed();

  done_testing();
}
