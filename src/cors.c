#include "cors.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>  // for strlen

#include "header.h"           // for get_header, set_header, derive_headers
#include "libutil/libutil.h"  // for s_*
#include "util.h"             // for str_join
#include "xmalloc.h"

// Set by server and specifies the allowed origin. Must be a single value, or a
// wildcard for allow all origins.
static const char ALLOW_ORIGINS_HEADER[] = "Access-Control-Allow-Origin";

// Set by server and specifies the allowed methods. May be multiple values.
static const char ALLOW_METHODS_HEADER[] = "Access-Control-Allow-Methods";

// Set by server and specifies the allowed headers. May be multiple values.
static const char ALLOW_HEADERS_HEADER[] = "Access-Control-Allow-Headers";

// Set by server and specifies whether the client may send credentials. The
// client may still send credentials if the request was not preceded by a
// Preflight and the client specified `withCredentials`.
static const char ALLOW_CREDENTIALS_HEADER[] =
    "Access-Control-Allow-Credentials";

// Set by server and specifies which non-simple response headers may be
// visible to the client.
static const char EXPOSE_HEADERS_HEADER[] = "Access-Control-Expose-Headers";

// Set by server and specifies how long, in seconds, a response can stay in
// the browser's cache before another Preflight is made.
static const char MAX_AGE_HEADER[] = "Access-Control-Max-Age";

// Sent via Preflight when the client is using a non-simple HTTP method.
static const char REQUEST_METHOD_HEADER[] = "Access-Control-Request-Method";

// Sent via Preflight when the client has set additional headers. May be
// multiple values.
static const char REQUEST_HEADERS_HEADER[] = "Access-Control-Request-Headers";

// Specifies the origin of the request or response.
static const char ORIGIN_HEADER[] = "Origin";

// Set by server and tells proxy servers to take into account the Origin
// header when deciding whether to send cached content.
static const char VARY_HEADER[] = "Vary";

static cors_config *cors_conf;

/**
 * match is a matcher function for comparing array values against a given value
 * `s2`
 *
 * @param s1
 * @param s2
 * @return bool
 */
bool match(void *s1, void *s2) { return s_casecmp((char *)s1, (char *)s2); }

/**
 * is_origin_allowed determines whether the given origin is allowed per the
 * user-defined allow list
 *
 * @param c
 * @param origin
 * @return bool
 */
static bool is_origin_allowed(cors_config *c, char *origin) {
  if (c->allow_all_origins) {
    return true;
  }

  foreach (c->allowed_origins, i) {
    char *allowed_origin = array_get(c->allowed_origins, i);
    if (s_casecmp(origin, allowed_origin)) {
      return true;
    }
  }

  return false;
}

/**
 * is_method_allowed determines whether the given method is allowed per the
 * user-defined allow list
 *
 * @param c
 * @param method
 * @return bool
 */
static bool is_method_allowed(cors_config *c, char *method) {
  if (!has_elements(c->allowed_methods)) {
    return false;
  }

  // Always allow preflight requests
  if (s_casecmp(method, http_method_names[METHOD_OPTIONS])) {
    return true;
  }

  foreach (c->allowed_methods, i) {
    char *allowed_method = array_get(c->allowed_methods, i);
    if (s_casecmp(method, allowed_method)) {
      return true;
    }
  }

  return false;
}

/**
 * are_headers_allowed determines whether the given headers are allowed per
 * the user-defined allow list
 *
 * @param c
 * @param headers
 * @return bool
 */
static bool are_headers_allowed(cors_config *c, array_t *headers) {
  if (c->allow_all_headers || !has_elements(headers)) {
    return true;
  }

  foreach (headers, i) {
    char *header = to_canonical_mime_header_key(array_get(headers, i));

    if (!array_includes(c->allowed_headers, match, header)) {
      return false;
    }
  }

  return true;
}

/**
 * isPreflightRequest determines whether the given request is a Preflight.
 * A Preflight must:
 * 1) use the OPTIONS method
 * 2) include an Origin request header
 * 3) Include an Access-Control-Request-Method header
 *
 * @param req
 * @return bool
 */
static bool is_preflight_request(request_internal *req) {
  bool is_options_req =
      s_equals(req->method, http_method_names[METHOD_OPTIONS]);
  bool has_origin_header = !s_nullish(get_header(req->headers, ORIGIN_HEADER));
  bool has_request_method =
      !s_nullish(get_header(req->headers, REQUEST_METHOD_HEADER));

  return is_options_req && has_origin_header && has_request_method;
}

/**
 * handleRequest handles actual HTTP requests subsequent to or standalone from
 * Preflight requests
 *
 * @param req
 * @param res
 *
 * TODO: test
 */
static void handle_request(request_internal *req, response *res) {
  char *origin = get_header(req->headers, ORIGIN_HEADER);
  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  set_header(res, VARY_HEADER, ORIGIN_HEADER);

  // If no origin was specified, this is not a valid CORS request
  if (s_nullish(origin)) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_conf, origin)) {
    set_status(res, STATUS_FORBIDDEN);
    return;
  }

  if (cors_conf->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    set_header(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    set_header(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // If we've exposed headers, set them
  // If the consumer specified headers that are exposed by default, we'll still
  // include them - this is spec compliant
  if (has_elements(cors_conf->exposed_headers)) {
    set_header(res, EXPOSE_HEADERS_HEADER,
               str_join(cors_conf->exposed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `with_credentials` to `true`
  if (cors_conf->allow_credentials) {
    set_header(res, ALLOW_CREDENTIALS_HEADER, "true");
  }
}

/**
 * handle_preflight_request handles preflight requests
 *
 * @param req
 * @param res
 *
 * TODO: test
 */
static void handle_preflight_request(request_internal *req, response *res) {
  char *origin = get_header(req->headers, ORIGIN_HEADER);

  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  set_header(res, VARY_HEADER,
             fmt_str("%s,%s,%s", ORIGIN_HEADER, REQUEST_METHOD_HEADER,
                     REQUEST_HEADERS_HEADER));

  // If no origin was specified, this is not a valid CORS request
  if (s_nullish(origin)) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_conf, origin)) {
    set_status(res, STATUS_FORBIDDEN);

    return;
  }

  // Validate the method; this is the crux of the Preflight
  char *reqd_method = get_header(req->headers, REQUEST_METHOD_HEADER);

  if (s_nullish(reqd_method) || !is_method_allowed(cors_conf, reqd_method)) {
    return;
  }

  // Validate request headers. Preflights are also used when
  // requests include additional headers from the client
  char *header_str = get_header(req->headers, REQUEST_HEADERS_HEADER);

  array_t *reqd_headers = derive_headers(header_str);
  if (!are_headers_allowed(cors_conf, reqd_headers)) {
    return;
  }

  if (cors_conf->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    set_header(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    set_header(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // Set the allowed methods, as a Preflight may have been sent if the client
  // included non-simple methods
  set_header(res, ALLOW_METHODS_HEADER, reqd_method);

  // Set the allowed headers, as a Preflight may have been sent if the client
  // included non-simple headers.
  if (has_elements(reqd_headers)) {
    set_header(res, ALLOW_HEADERS_HEADER,
               str_join(cors_conf->allowed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `with_credentials` to `true`.
  if (cors_conf->allow_credentials) {
    set_header(res, ALLOW_CREDENTIALS_HEADER, "true");
  }

  // Set the Max Age. This is only necessary for Preflights given the Max Age
  // refers to server-suggested duration, in seconds, a response should stay in
  // the browser's cache before another Preflight is made
  if (cors_conf->max_age > 0) {
    set_header(res, MAX_AGE_HEADER, fmt_str("%d", cors_conf->max_age));
  }
}

cors_opts *cors_opts_init() {
  cors_opts_internal *opts = xmalloc(sizeof(cors_opts_internal));
  opts->allow_credentials = false;
  opts->max_age = 0;
  opts->use_options_passthrough = false;

  return (cors_opts *)opts;
}

cors_config *cors_init(cors_opts_internal *opts) {
  cors_conf = xmalloc(sizeof(cors_config));
  cors_conf->allow_credentials = opts->allow_credentials;
  cors_conf->max_age = opts->max_age;
  cors_conf->exposed_headers = opts->expose_headers;
  cors_conf->use_options_passthrough = opts->use_options_passthrough;
  cors_conf->allowed_origins = array_init();
  cors_conf->allowed_methods = array_init();

  // Register origins - if no given origins, default to allow all i.e. "*"
  if (!has_elements(opts->allowed_origins)) {
    cors_conf->allow_all_origins = true;
  } else {
    foreach (opts->allowed_origins, i) {
      char *origin = array_get(opts->allowed_origins, i);
      if (s_equals("*", origin)) {
        cors_conf->allow_all_origins = true;
        break;
      }

      array_push(cors_conf->allowed_origins, origin);
    }

    // Append "null" to allow list to support testing / requests from files,
    // redirects, etc. Note: Used for redirects because the browser should not
    // expose the origin of the new server; redirects are followed
    // automatically
    array_push(cors_conf->allowed_origins, "null");
  }

  // Register headers - if no given headers, default to those allowed per the
  // spec. Although these headers are allowed by default, we add them anyway for
  // the sake of consistency
  cors_conf->allowed_headers = array_init();
  if (has_elements(opts->allowed_headers)) {
    foreach (opts->allowed_headers, i) {
      char *header = array_get(opts->allowed_headers, i);

      if (s_equals("*", header)) {
        cors_conf->allow_all_headers = true;
        break;
      }

      array_push(cors_conf->allowed_headers,
                 to_canonical_mime_header_key(header));
    }
  }

  array_push(cors_conf->allowed_headers, (void *)ORIGIN_HEADER);

  if (!has_elements(opts->allowed_methods)) {
    // Default allowed methods. Defaults to simple methods (those that do not
    // trigger a Preflight)
    array_t *default_allowed_methods =
        array_collect(s_copy(http_method_names[METHOD_GET]),
                      s_copy(http_method_names[METHOD_POST]),
                      s_copy(http_method_names[METHOD_HEAD]));

    cors_conf->allowed_methods = default_allowed_methods;
  } else {
    foreach (opts->allowed_methods, i) {
      char *method = array_get(opts->allowed_methods, i);
      array_push(cors_conf->allowed_methods, s_upper(method));
    }
  }

  return cors_conf;
}

cors_opts *cors_allow_all() {
  cors_opts_internal *opts = xmalloc(sizeof(cors_opts_internal));
  opts->allowed_origins = array_collect("*");
  opts->allowed_methods =
      array_collect(s_copy(http_method_names[METHOD_HEAD]),
                    s_copy(http_method_names[METHOD_GET]),
                    s_copy(http_method_names[METHOD_POST]),
                    s_copy(http_method_names[METHOD_PATCH]),
                    s_copy(http_method_names[METHOD_PUT]),
                    s_copy(http_method_names[METHOD_DELETE]));
  opts->allowed_headers = array_collect("*");
  opts->allow_credentials = false;

  return (cors_opts *)opts;
}

response *cors_handler(request_internal *req, response *res) {
  if (is_preflight_request(req)) {
    handle_preflight_request(req, res);

    if (cors_conf->use_options_passthrough) {
      // Do nothing i.e. allow next handler to run
    } else {
      set_status(res, STATUS_NO_CONTENT);
      // We're done
      set_done(res, true);
    }
  } else {
    handle_request(req, res);
    // Do nothing i.e. allow next handler to run
  }

  return res;
}

void set_allow_credentials(cors_opts *opts, bool allow) {
  ((cors_opts_internal *)opts)->allow_credentials = allow;
}

void set_use_options_passthrough(cors_opts *opts, bool use) {
  ((cors_opts_internal *)opts)->use_options_passthrough = use;
}

void set_max_age(cors_opts *opts, int max_age) {
  ((cors_opts_internal *)opts)->max_age = max_age;
}

void __set_allowed_origins(cors_opts *opts, array_t *origins) {
  ((cors_opts_internal *)opts)->allowed_origins = origins;
}

void __set_allowed_methods(cors_opts *opts, array_t *methods) {
  ((cors_opts_internal *)opts)->allowed_methods = methods;
}

void __set_allowed_headers(cors_opts *opts, array_t *headers) {
  ((cors_opts_internal *)opts)->allowed_headers = headers;
}

void __set_expose_headers(cors_opts *opts, array_t *headers) {
  ((cors_opts_internal *)opts)->expose_headers = headers;
}
