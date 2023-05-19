#include "cors.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "header.h"
#include "libutil/libutil.h"
#include "util.h"
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
static const char REQUEST_YS_METHOD_HEADER[] = "Access-Control-Request-Method";

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
 */
bool match(void *s1, void *s2) { return s_casecmp((char *)s1, (char *)s2); }

/**
 * is_origin_allowed determines whether the given origin is allowed per the
 * user-defined allow list
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
 */
static bool is_method_allowed(cors_config *c, char *method) {
  if (has_elements(c->allowed_methods)) {
    // Always allow preflight requests
    if (s_casecmp(method, ys_http_method_names[YS_METHOD_OPTIONS])) {
      return true;
    }

    foreach (c->allowed_methods, i) {
      char *allowed_method = array_get(c->allowed_methods, i);
      if (s_casecmp(method, allowed_method)) {
        return true;
      }
    }
  }

  return false;
}


  return false;
}

/**
 * are_headers_allowed determines whether the given headers are allowed per
 * the user-defined allow list
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
 */
static bool is_preflight_request(request_internal *req) {
  bool is_options_req =
      s_equals(req->method, ys_http_method_names[YS_METHOD_OPTIONS]);
  bool has_origin_header =
      !s_nullish(get_first_header(req->headers, ORIGIN_HEADER));
  bool has_request_method =
      !s_nullish(get_first_header(req->headers, REQUEST_YS_METHOD_HEADER));

  return is_options_req && has_origin_header && has_request_method;
}

/**
 * handleRequest handles actual HTTP requests subsequent to or standalone from
 * Preflight requests
 */
static void handle_request(request_internal *req, ys_response *res) {
  char *origin = get_first_header(req->headers, ORIGIN_HEADER);
  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  ys_set_header(res, VARY_HEADER, ORIGIN_HEADER);

  // If no origin was specified, this is not a valid CORS request
  if (s_nullish(origin)) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_conf, origin)) {
    ys_set_status(res, YS_STATUS_FORBIDDEN);
    return;
  }

  if (cors_conf->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    ys_set_header(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    ys_set_header(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // If we've exposed headers, set them
  // If the consumer specified headers that are exposed by default, we'll still
  // include them - this is spec compliant
  if (has_elements(cors_conf->exposed_headers)) {
    ys_set_header(res, EXPOSE_HEADERS_HEADER,
                  str_join(cors_conf->exposed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `with_credentials` to `true`
  if (cors_conf->allow_credentials) {
    ys_set_header(res, ALLOW_CREDENTIALS_HEADER, "true");
  }
}

/**
 * handle_preflight_request handles preflight requests
 */
static void handle_preflight_request(request_internal *req, ys_response *res) {
  char *origin = get_first_header(req->headers, ORIGIN_HEADER);

  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  ys_set_header(res, VARY_HEADER,
                fmt_str("%s, %s, %s", ORIGIN_HEADER, REQUEST_YS_METHOD_HEADER,
                        REQUEST_HEADERS_HEADER));

  // If no origin was specified, this is not a valid CORS request
  if (s_nullish(origin)) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_conf, origin)) {
    ys_set_status(res, YS_STATUS_FORBIDDEN);
    return;
  }

  // Validate the method; this is the crux of the Preflight
  char *reqd_method = get_first_header(req->headers, REQUEST_YS_METHOD_HEADER);

  if (s_nullish(reqd_method) || !is_method_allowed(cors_conf, reqd_method)) {
    return;
  }

  // Validate request headers. Preflights are also used when
  // requests include additional headers from the client
  char *header_str = get_first_header(req->headers, REQUEST_HEADERS_HEADER);

  array_t *reqd_headers = derive_headers(header_str);
  if (!are_headers_allowed(cors_conf, reqd_headers)) {
    array_free(reqd_headers);
    return;
  }

  if (cors_conf->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    ys_set_header(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    ys_set_header(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // Set the allowed methods, as a Preflight may have been sent if the client
  // included non-simple methods
  ys_set_header(res, ALLOW_METHODS_HEADER, reqd_method);

  // Set the allowed headers, as a Preflight may have been sent if the client
  // included non-simple headers.
  if (has_elements(reqd_headers)) {
    ys_set_header(res, ALLOW_HEADERS_HEADER,
                  str_join(cors_conf->allowed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `with_credentials` to `true`.
  if (cors_conf->allow_credentials) {
    ys_set_header(res, ALLOW_CREDENTIALS_HEADER, "true");
  }

  // Set the Max Age. This is only necessary for Preflights given the Max Age
  // refers to server-suggested duration, in seconds, a response should stay in
  // the browser's cache before another Preflight is made
  if (cors_conf->max_age > 0) {
    ys_set_header(res, MAX_AGE_HEADER, fmt_str("%d", cors_conf->max_age));
  }

  array_free(reqd_headers);
}

ys_cors_opts *ys_cors_opts_init(void) {
  cors_opts_internal *opts = xmalloc(sizeof(cors_opts_internal));
  opts->allow_credentials = false;
  opts->max_age = 0;
  opts->use_options_passthrough = false;
  // TODO: why does this need to be pre-initialized but not other arrays (when
  // using allow_x macros)
  opts->allowed_methods = array_init();

  return (ys_cors_opts *)opts;
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
        array_collect(s_copy(ys_http_method_names[YS_METHOD_GET]),
                      s_copy(ys_http_method_names[YS_METHOD_POST]),
                      s_copy(ys_http_method_names[YS_METHOD_HEAD]));

    cors_conf->allowed_methods = default_allowed_methods;
  } else {
    foreach (opts->allowed_methods, i) {
      char *method = array_get(opts->allowed_methods, i);
      array_push(cors_conf->allowed_methods, s_upper(method));
    }
  }

  return cors_conf;
}

ys_cors_opts *ys_cors_allow_all(void) {
  cors_opts_internal *opts = xmalloc(sizeof(cors_opts_internal));
  opts->allowed_origins = array_collect("*");
  opts->allowed_methods =
      array_collect(s_copy(ys_http_method_names[YS_METHOD_HEAD]),
                    s_copy(ys_http_method_names[YS_METHOD_GET]),
                    s_copy(ys_http_method_names[YS_METHOD_POST]),
                    s_copy(ys_http_method_names[YS_METHOD_PATCH]),
                    s_copy(ys_http_method_names[YS_METHOD_PUT]),
                    s_copy(ys_http_method_names[YS_METHOD_DELETE]));
  opts->allowed_headers = array_collect("*");
  opts->allow_credentials = false;

  return (ys_cors_opts *)opts;
}

ys_response *cors_handler(ys_request *req, ys_response *res) {
  request_internal *reqi = (request_internal *)req;

  if (is_preflight_request(reqi)) {
    handle_preflight_request(reqi, res);

    if (cors_conf->use_options_passthrough) {
      // Do nothing i.e. allow next handler to run
    } else {
      ys_set_status(res, YS_STATUS_NO_CONTENT);
      // We're done
      ys_set_done(res);
    }
  } else {
    handle_request(reqi, res);
    // Do nothing i.e. allow next handler to run
  }

  return res;
}

void ys_cors_allow_credentials(ys_cors_opts *opts, bool allow) {
  ((cors_opts_internal *)opts)->allow_credentials = allow;
}

void ys_cors_use_options_passthrough(ys_cors_opts *opts, bool use) {
  ((cors_opts_internal *)opts)->use_options_passthrough = use;
}

void ys_cors_set_max_age(ys_cors_opts *opts, int max_age) {
  ((cors_opts_internal *)opts)->max_age = max_age;
}

void __set_allowed_origins(ys_cors_opts *opts, array_t *origins) {
  ((cors_opts_internal *)opts)->allowed_origins = origins;
}

void __set_allowed_methods(ys_cors_opts *opts, array_t *methods) {
  array_t *allowed_methods = ((cors_opts_internal *)opts)->allowed_methods;

  foreach (methods, i) {
    array_push(
        allowed_methods,
        s_copy(ys_http_method_names[(ys_http_method)array_get(methods, i)]));
  }
}

void __set_allowed_headers(ys_cors_opts *opts, array_t *headers) {
  ((cors_opts_internal *)opts)->allowed_headers = headers;
}

void __set_expose_headers(ys_cors_opts *opts, array_t *headers) {
  ((cors_opts_internal *)opts)->expose_headers = headers;
}
