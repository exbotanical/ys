#include "cookie.c"

#include <stdarg.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "libys.h"
#include "response.h"
#include "tap.c/tap.h"
#include "tests.h"

typedef struct {
  hash_table* headers;
  array_t* expected;
} test_case;

cookie_internal* tocookie(const char* domain, const char* value,
                          const char* name, const char* path, time_t expires,
                          int max_age, ys_same_site_mode same_site,
                          bool http_only, bool secure) {
  ys_cookie* c = ys_cookie_init(name, value);

  ys_cookie_set_domain(c, domain);
  ys_cookie_set_expires(c, expires);
  ys_cookie_set_http_only(c, http_only);
  if (max_age != 0) {
    ys_cookie_set_max_age(c, max_age < 0 ? 0 : max_age);
  }
  ys_cookie_set_path(c, path);
  ys_cookie_set_same_site(c, same_site);
  ys_cookie_set_secure(c, secure);

  return (cookie_internal*)c;
}

void test_read_cookies(void) {
  test_case tests[] = {
      {.headers = to_headers(
           to_header(
               COOKIE,
               array_collect(
                   "cookie-9=i3e01nf61b6t23bvfmplnanol3; Path=/restricted/; "
                   "Domain=example.com; Expires=Tue, 10 Nov 2009 23:00:00 GMT; "
                   "Max-Age=3600",
                   "test=value; Path=/internal/path; Domain=test.com; "
                   "Expires=Mon, 21 Jan 2109 03:17:48 GMT; Max-Age=28800; "
                   "HttpOnly; Secure; SameSite=Strict")),
           NULL),
       .expected = array_collect(
           tocookie("example.com", "i3e01nf61b6t23bvfmplnanol3", "cookie-9",
                    "/restricted/", 1257894000, 3600, SAME_SITE_DEFAULT_MODE,
                    false, false),
           tocookie("test.com", "value", "test", "/internal/path", 4388181468,
                    28800, SAME_SITE_STRICT_MODE, true, true))},

      {.headers = to_headers(
           to_header(COOKIE,
                     array_collect(
                         "TestCookie=bGliaHR0cAo=; Path=/some/path; "
                         "Domain=www.domain.com; Expires=Mon, 25 Apr 2078 "
                         "14:11:39 GMT; Max-Age=120; Secure; SameSite=Lax")),
           NULL),
       .expected = array_collect(tocookie(
           "www.domain.com", "bGliaHR0cAo=", "TestCookie", "/some/path",
           3418121499, 120, SAME_SITE_LAX_MODE, false, true))},

      {.headers = to_headers(
           to_header(
               COOKIE,
               array_collect("M=T; Path=/; Domain=.somesite.com; SameSite=Lax",
                             "M=T; Path=/; Max-Age=0; SameSite=Lax")),
           NULL),
       .expected = array_collect(tocookie(".somesite.com", "T", "M", "/", -1, 0,
                                          SAME_SITE_LAX_MODE, false, false),
                                 tocookie(NULL, "T", "M", "/", -1, -1,
                                          SAME_SITE_LAX_MODE, false, false))}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    array_t* cookies = read_cookies(test.headers);

    foreach (cookies, i) {
      cookie_internal* actual = (cookie_internal*)array_get(cookies, i);
      cookie_internal* expected = (cookie_internal*)array_get(test.expected, i);

      is(actual->domain, expected->domain, "domain is correctly deserialized");
      is(actual->name, expected->name, "name is correctly deserialized");
      is(actual->path, expected->path, "path is correctly deserialized");
      is(actual->value, expected->value, "value is correctly deserialized");
      ok(actual->expires == expected->expires,
         "expiry is correctly deserialized = actual is %lu and expected is %lu",
         actual->expires, expected->expires);
      ok(actual->http_only == expected->http_only,
         "httponly flag is correctly deserialized - actual is %s",
         actual->http_only ? "true" : "false");
      ok(actual->max_age == expected->max_age,
         "max age is correctly deserialized");
      ok(actual->secure == expected->secure,
         "secure flag is correctly deserialized - actual is %s",
         actual->secure ? "true" : "false");
      ok(actual->same_site == expected->same_site,
         "samesite flag is correctly deserialized - actual is %d",
         actual->same_site);

      ys_cookie_free(actual);
      ys_cookie_free(expected);
    }

    array_free(cookies);
    array_free(test.expected);
  }
}

void test_sanitize_cookie_value(void) {
  typedef struct {
    char* in;
    char* want;
  } test_case;

  test_case tests[] = {{"foo", "foo"},
                       {"foo;bar", "foobar"},
                       {"foo\\bar", "foobar"},
                       {"foo\"bar", "foobar"},
                       //  {"\x00\x7e\x7f\x80", "\x7e"},
                       {"\"withquotes\"", "withquotes"},
                       {"a z", "\"a z\""},
                       {" z", "\" z\""},
                       {"a ", "\"a \""},
                       {"a,z", "\"a,z\""},
                       {",z", "\",z\""},
                       {"a,", "\"a,\""}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    is(sanitize_cookie_value(test.in), test.want,
       "cookie value should be sanitized to %s", test.want);
  }
}

void test_sanitize_cookie_path(void) {
  typedef struct {
    char* in;
    char* want;
  } test_case;

  test_case tests[] = {{"/path", "/path"},
                       {"/path with space/", "/path with space/"},
                       {"/just;no;semicolon\x00orstuff/", "/justnosemicolon"}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    is(sanitize_cookie_path(test.in), test.want,
       "cookie path should be sanitized to %s", test.want);
  }
}

void test_cookie_serialize(void) {
  const char* expected =
      "cookie-9=i3e01nf61b6t23bvfmplnanol3; Path=/restricted/; "
      "Domain=example.com; Expires=Tue, 10 Nov 2009 23:00:00 GMT; Max-Age=3600";

  cookie_internal* c = tocookie(".example.com", "i3e01nf61b6t23bvfmplnanol3",
                                "cookie-9", "/restricted/", 1257894000, 3600,
                                SAME_SITE_DEFAULT_MODE, false, false);

  const char* actual = cookie_serialize(c);

  is(actual, expected, "serializes cookie as expected");
  ys_cookie_free(c);
}

void test_ys_get_cookie(void) {
  cookie_internal* c =
      tocookie(".somesite.gov", "value", "NotThisCookie", "/path", 1257894000,
               3600, SAME_SITE_STRICT_MODE, true, true);
  cookie_internal* c2 =
      tocookie("hello.com", "someval", "NotThisCookieEither", "/", -1, -1,
               SAME_SITE_NONE_MODE, false, false);
  cookie_internal* expected =
      tocookie(".testsite.com", "test", "ThisCookie", "/", 2257894000, 86400,
               SAME_SITE_LAX_MODE, false, true);

  request_internal* req =
      malloc(sizeof(request_internal));  // TODO: request_init fn
  req->headers = ht_init(0);
  insert_header(req->headers, COOKIE, cookie_serialize(c), true);
  insert_header(req->headers, COOKIE, cookie_serialize(expected), true);
  insert_header(req->headers, COOKIE, cookie_serialize(c2), true);

  cookie_internal* actual = (cookie_internal*)ys_get_cookie(req, "ThisCookie");

  is(actual->domain, "testsite.com",  // . is stripped
     "retrieved cookie domain matches what was inserted");
  is(actual->name, expected->name,
     "retrieved cookie name matches what was inserted");
  is(actual->path, expected->path,
     "retrieved cookie path matches what was inserted");
  is(actual->value, expected->value,
     "retrieved cookie value matches what was inserted");
  ok(actual->expires == expected->expires,
     "retrieved cookie expiry (%lu) matches what was inserted (%lu)",
     actual->expires, expected->expires);
  ok(actual->http_only == expected->http_only,
     "retrieved cookie httponly flag (%s) matches what was inserted (%s)",
     actual->http_only ? "true" : "false");
  ok(actual->max_age == expected->max_age,
     "retrieved cookie max age matches what was inserted");
  ok(actual->secure == expected->secure,
     "retrieved cookie secure flag (%s) matches what was inserted (%s)",
     actual->secure ? "true" : "false");
  ok(actual->same_site == expected->same_site,
     "retrieved cookie samesite flag (%d) matches what was inserted",
     actual->same_site);

  ys_cookie_free(c);
  ys_cookie_free(c2);
  ys_cookie_free(actual);
  ys_cookie_free(expected);
}

void test_ys_set_cookie(void) {
  ys_cookie* c = tocookie(".testsite.com", "test", "ThisCookie", "/",
                          2257894000, 86400, SAME_SITE_LAX_MODE, false, true);

  response_internal* res = response_init();
  ys_set_cookie(res, c);

  const char* actual = get_first_header(res->headers, SET_COOKIE);

  const char* expected =
      "ThisCookie=test; Path=/; Domain=testsite.com; Expires=Sat, 20 Jul 2041 "
      "00:46:40 GMT; Max-Age=86400; Secure; SameSite=Lax";

  ys_cookie_free(c);
}

void test_ys_cookie_free(void) {
  ys_cookie* c = tocookie(".testsite.com", "test", "ThisCookie", "/",
                          2257894000, 86400, SAME_SITE_LAX_MODE, false, true);

  lives_ok({ ys_cookie_free(c); }, "ys_cookie_free does not segfault");
}

void run_cookie_tests(void) {
  test_ys_cookie_free();

  test_read_cookies();
  test_sanitize_cookie_value();
  test_sanitize_cookie_path();
  test_cookie_serialize();
  test_ys_get_cookie();
  test_ys_set_cookie();
}
