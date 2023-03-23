#include "cors.c"

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

req_t *toreq(http_method_t method, array_t *headers) {
  hash_table *ht = ht_init(0);
  foreach (headers, i) {
    header_t *h = array_get(headers, i);

    ht_insert(ht, h->key, array_collect(h->value));
  }

  req_t *req = malloc(sizeof(req_t));
  req->method = http_method_names[method];
  req->headers = ht;

  return req;
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
       .res_headers = toheaders(
           "Vary",
           "Origin,Access-Control-Request-Method,"
           "Access-Control-Request-Headers",
           "Access-Control-Allow-Origin", "*", "Access-Control-Allow-Headers",
           "X-Testing, Origin", "Access-Control-Allow-Methods",
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
           "X-Testing, X-Testing-2, X-Testing-3, Origin",
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

void test_is_preflight_request() {
  typedef struct {
    const char *name;
    req_t *req;
    bool is_preflight;
  } test_case;

  test_case tests[] = {
      {.name = "ValidCORSRequest",
       .is_preflight = true,
       .req = toreq(METHOD_OPTIONS,
                    toheaders("Origin", "https://test.com",
                              "Access-Control-Request-Method", "POST", NULL))},
      {.name = "MissingOriginHeader",
       .is_preflight = false,
       .req = toreq(METHOD_OPTIONS,
                    toheaders("Access-Control-Request-Method", "POST", NULL))},
      {.name = "MissingRequestMethodHeader",
       .is_preflight = false,
       .req = toreq(METHOD_OPTIONS,
                    toheaders("Origin", "https://test.com", NULL))},
      {.name = "NotOPTIONSRequest",
       .is_preflight = false,
       .req = toreq(METHOD_GET,
                    toheaders("Origin", "https://test.com",
                              "Access-Control-Request-Method", "POST", NULL))}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case t = tests[i];
    ok(is_preflight_request(t.req) == t.is_preflight,
       "%s - is%sa preflight request", t.name, t.is_preflight ? " " : " not ");
  }
}

void test_cors_allow_all() {
  cors_opts_t *o = cors_allow_all();
  cors_t *c = cors_init(o);

  ok(c->allow_all_origins == true, "cors_allow_all allows all origins");
  ok(c->allow_all_headers == true, "cors_allow_all allows all headers");
  ok(c->allow_credentials == false,
     "cors_allow_all does not allow credentials");
  ok(array_size(c->allowed_methods) == 6,
     "cors_allow_all allows all common methods");
}

void test_set_helpers() {
  cors_opts_t *o = cors_opts_init();

  const char *o1 = "o1";
  const char *o2 = "o2";
  const char *o3 = "o3";
  set_allowed_origins(o, o1, o2, o3);  // TODO: internal to omit NULL

  set_allowed_methods(o, "GET", "PUT", "DELETE");

  const char *h1 = "h1";
  const char *h2 = "h2";
  const char *h3 = "h3";
  set_allowed_headers(o, h1, h2, h3, NULL);

  const int m = 3600;
  set_max_age(o, m);
  ok(o->max_age == 3600, "max_age is 0 by default");

  cors_t *c = cors_init(o);

  ok(c->allow_credentials == false, "allow_credentials is false by default");

  set_allow_credentials(o, true);
  ok(o->allow_credentials == true,
     "set_allow_credentials(o, true) sets allow_credentials to true");

  set_allow_credentials(o, false);
  ok(o->allow_credentials == false,
     "set_allow_credentials(o, false) sets allow_credentials to false");

  ok(o->use_options_passthrough == false,
     "use_options_passthrough is false by default");

  set_use_options_passthrough(o, true);
  ok(o->use_options_passthrough == true,
     "set_use_options_passthrough(o, true) sets use_options_passthrough to "
     "true");

  set_use_options_passthrough(o, false);
  ok(o->use_options_passthrough == false,
     "set_use_options_passthrough(o, false) sets use_options_passthrough to "
     "false");

  ok(c->max_age == m, "set_max_age(o, n) sets max_age to n");

  ok(is_origin_allowed(c, "o4") == false,
     "set_allowed_origins functions properly");
  ok(is_origin_allowed(c, o1) == true, "set_allowed_origins allows the origin");
  ok(is_origin_allowed(c, o2) == true, "set_allowed_origins allows the origin");
  ok(is_origin_allowed(c, o3) == true, "set_allowed_origins allows the origin");

  ok(is_method_allowed(c, "PATCH") == false,
     "set_allowed_methods functions properly");
  ok(is_method_allowed(c, "GET") == true,
     "set_allowed_methods allows the method");
  ok(is_method_allowed(c, "PUT") == true,
     "set_allowed_methods allows the method");
  ok(is_method_allowed(c, "DELETE") == true,
     "set_allowed_methods allows the method");

  ok(are_headers_allowed(c, array_collect("h4")) == false,
     "set_allowed_headers functions properly");
  ok(are_headers_allowed(c, array_collect(h1, h2, h3)) == true,
     "set_allowed_headers allows the headers");
}

int main(int argc, char const *argv[]) {
  plan(85);

  test_cors_middleware();
  test_are_headers_allowed();
  test_is_method_allowed();
  test_origin_is_allowed();
  test_is_preflight_request();
  test_cors_allow_all();
  test_set_helpers();

  done_testing();
}
