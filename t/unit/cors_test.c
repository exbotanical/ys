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
  foreach (headers, i) {
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
          .options = make_opts(array_collect("*"), NULL, NULL, NULL, false),
          .method = METHOD_GET,
          .req_headers = toheaders("Origin", "http://foo.com", NULL),
          .res_headers = toheaders("Vary", "Origin",
                                   "Access-Control-Allow-Origin", "*", NULL),
          .code = STATUS_OK,
      },
      {
          .name = "OriginAllowed",
          .options = make_opts(array_collect("http://foo.com"), NULL, NULL,
                               NULL, false),
          .method = METHOD_GET,
          .req_headers = toheaders("Origin", "http://foo.com", NULL),
          .res_headers =
              toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                        "http://foo.com", NULL),
          .code = STATUS_OK,
      },
      {.name = "OriginAllowedMultipleProvided",
       .options = make_opts(array_collect("http://foo.com", "http://bar.com"),
                            NULL, NULL, NULL, false),
       .method = METHOD_GET,
       .req_headers = toheaders("Origin", "http://bar.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://bar.com", NULL),
       .code = STATUS_OK},
      {.name = "GETMethodAllowedDefault",
       .options =
           make_opts(array_collect("http://foo.com"), NULL, NULL, NULL, false),
       .method = METHOD_GET,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = STATUS_OK},
      {.name = "POSTMethodAllowedDefault",
       .options =
           make_opts(array_collect("http://foo.com"), NULL, NULL, NULL, false),
       .method = METHOD_POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = STATUS_OK},
      {.name = "HEADMethodAllowedDefault",
       .options =
           make_opts(array_collect("http://foo.com"), NULL, NULL, NULL, false),
       .method = METHOD_HEAD,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders("Vary", "Origin", "Access-Control-Allow-Origin",
                                "http://foo.com", NULL),
       .code = STATUS_OK},
      {.name = "MethodAllowed",
       // TODO: allow just enum
       .options = make_opts(array_collect("*"),
                            array_collect(http_method_names[METHOD_DELETE]),
                            NULL, NULL, false),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[METHOD_DELETE], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,Access-"
                                "Control-Request-Headers",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Methods",
                                http_method_names[METHOD_DELETE], NULL),
       .code = STATUS_NO_CONTENT},
      {.name = "HeadersAllowed",
       .options = make_opts(array_collect("*"), NULL,
                            array_collect("X-Testing"), NULL, false),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders(
           "Origin", "http://foo.com", "Access-Control-Request-Method",
           http_method_names[METHOD_GET], "Access-Control-Request-Headers",
           "X-Testing", NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Headers", "X-Testing",
                                "Access-Control-Allow-Methods",
                                http_method_names[METHOD_GET], NULL),
       .code = STATUS_NO_CONTENT},
      {.name = "HeadersAllowedMultiple",
       .options =
           make_opts(array_collect("*"), NULL,
                     array_collect("X-Testing", "X-Testing-2", "X-Testing-3"),
                     NULL, false),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders(
           "Origin", "http://foo.com", "Access-Control-Request-Method",
           http_method_names[METHOD_GET], "Access-Control-Request-Headers",
           "X-Testing, X-Testing-2, X-Testing-3", NULL),
       .res_headers = toheaders(
           "Vary",
           "Origin,Access-Control-Request-Method,"
           "Access-Control-Request-Headers",
           "Access-Control-Allow-Origin", "*", "Access-Control-Allow-Headers",
           "X-Testing, X-Testing-2, X-Testing-3",
           "Access-Control-Allow-Methods", http_method_names[METHOD_GET], NULL),
       .code = STATUS_NO_CONTENT},
      {.name = "CredentialsAllowed",
       .options = make_opts(array_collect("*"), NULL, NULL, NULL, true),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[METHOD_GET], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                "Access-Control-Allow-Credentials", "true",
                                "Access-Control-Allow-Origin", "*",
                                "Access-Control-Allow-Methods",
                                http_method_names[METHOD_GET], NULL),
       .code = STATUS_NO_CONTENT},
      {.name = "ExposeHeaders",
       .options = make_opts(array_collect("http://foo.com"), NULL, NULL,
                            array_collect("x-test"), false),
       .method = METHOD_POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders(
           "Vary", "Origin", "Access-Control-Allow-Origin", "http://foo.com",
           "Access-Control-Expose-Headers", "x-test", NULL),
       .code = STATUS_OK},
      {.name = "ExposeHeadersMultiple",
       .options = make_opts(array_collect("http://foo.com"), NULL, NULL,
                            array_collect("x-test-1", "x-test-2"), false),
       .method = METHOD_POST,
       .req_headers = toheaders("Origin", "http://foo.com", NULL),
       .res_headers = toheaders(
           "Vary", "Origin", "Access-Control-Allow-Origin", "http://foo.com",
           "Access-Control-Expose-Headers", "x-test-1, x-test-2", NULL),
       .code = STATUS_OK},

      // CORS Rejections
      {.name = "OriginNotAllowed",
       .options =
           make_opts(array_collect("http://foo.com"), NULL, NULL, NULL, false),
       .method = METHOD_GET,
       .req_headers = toheaders("Origin", "http://bar.com", NULL),
       .res_headers = toheaders("Vary", "Origin", NULL),
       .code = STATUS_OK},
      {.name = "OriginNotAllowedPortMismatch",
       .options = make_opts(array_collect("http://foo.com:443"), NULL, NULL,
                            NULL, false),
       .method = METHOD_GET,
       .req_headers = toheaders("Origin", "http://foo.com:444", NULL),
       .res_headers = toheaders("Vary", "Origin", NULL),
       .code = STATUS_OK},
      {.name = "MethodNotAllowed",
       .options = make_opts(array_collect("*"), NULL, NULL, NULL, false),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders("Origin", "http://foo.com",
                                "Access-Control-Request-Method",
                                http_method_names[METHOD_DELETE], NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                NULL),
       .code = STATUS_NO_CONTENT},
      {.name = "HeadersNotAllowed",
       .options = make_opts(array_collect("*"), NULL, NULL, NULL, false),
       .method = METHOD_OPTIONS,
       .req_headers = toheaders(
           "Origin", "http://foo.com", "Access-Control-Request-Method",
           http_method_names[METHOD_GET], "Access-Control-Request-Headers",
           "X-Testing", NULL),
       .res_headers = toheaders("Vary",
                                "Origin,Access-Control-Request-Method,"
                                "Access-Control-Request-Headers",
                                NULL),
       .code = STATUS_NO_CONTENT}};

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
    cors_init(test.options);
    cors_handler(req, res);

    // Evaluate resulting headers and status
    check_headers(test.name, res->headers, test.res_headers);

    // check_status_code(res, (int)test.code); // TODO:
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
       .cors = cors_init(make_opts(array_collect("*"), NULL,
                                   array_collect("x-test-1", "x-test-2"), NULL,
                                   false)),
       .cases = array_collect(
           &(test_case){.test_headers = array_collect("x-test-1", "x-test-2"),
                        .is_allowed = true},
           &(test_case){
               .test_headers = array_collect("x-test", "x-test-1", "x-test-2"),
               .is_allowed = false},
           &(test_case){.test_headers = array_collect(""),
                        .is_allowed = false})},
      {.name = "WildcardHeaders",
       .cors = cors_init(make_opts(array_collect("*"), NULL, array_collect("*"),
                                   NULL, false)),
       .cases = array_collect(
           &(test_case){.test_headers = array_collect("x-test-1", "x-test-2"),
                        .is_allowed = true},
           &(test_case){
               .test_headers = array_collect("x-test", "x-test-1", "x-test-2"),
               .is_allowed = true},
           &(test_case){.test_headers = array_collect(""),
                        .is_allowed = true})}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    foreach (t.cases, j) {
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

  test tests[] = {{.name = "ExplicitMethod",
                   .cors = cors_init(
                       make_opts(array_collect("*"),
                                 array_collect(http_method_names[METHOD_DELETE],
                                               http_method_names[METHOD_PUT]),
                                 NULL, NULL, false)),
                   .cases = array_collect(
                       &(test_case){.method = http_method_names[METHOD_DELETE],
                                    .is_allowed = true},
                       &(test_case){.method = http_method_names[METHOD_PUT],
                                    .is_allowed = true},
                       &(test_case){.method = http_method_names[METHOD_PATCH],
                                    .is_allowed = false})}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    foreach (t.cases, j) {
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
       .cors = cors_init(make_opts(
           array_collect("http://foo.com", "http://bar.com", "baz.com"), NULL,
           NULL, NULL, false)),
       .cases = array_collect(
           &(test_case){.origin = "http://foo.com", .is_allowed = true},
           &(test_case){.origin = "http://bar.com", .is_allowed = true},
           &(test_case){.origin = "baz.com", .is_allowed = true},
           &(test_case){.origin = "http://foo.com/", .is_allowed = false},
           &(test_case){.origin = "https://bar.com", .is_allowed = false},
           &(test_case){.origin = "http://baz.com", .is_allowed = false},
           &(test_case){.origin = "null",  // file redirect
                        .is_allowed = true})},
      {.name = "WildcardOrigin",
       .cors =
           cors_init(make_opts(array_collect("*"), NULL, NULL, NULL, false)),
       .cases = array_collect(
           &(test_case){.origin = "http://foo.com", .is_allowed = true},
           &(test_case){.origin = "http://bar.com", .is_allowed = true},
           &(test_case){.origin = "baz.com", .is_allowed = true},
           &(test_case){.origin = "http://foo.com/", .is_allowed = true},
           &(test_case){.origin = "https://bar.com", .is_allowed = true},
           &(test_case){.origin = "http://baz.com", .is_allowed = true})}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test); i++) {
    test t = tests[i];
    foreach (t.cases, j) {
      test_case *c = (test_case *)array_get(t.cases, j);

      bool actual = is_origin_allowed(t.cors, c->origin);
      ok(actual == c->is_allowed, "%s - origin %s is %s", t.name, c->origin,
         c->is_allowed ? "allowed" : "not allowed");
    }
  }
}

int main(int argc, char const *argv[]) {
  plan(59);

  test_cors_middleware();
  test_are_headers_allowed();
  test_is_method_allowed();
  test_origin_is_allowed();

  done_testing();
}
