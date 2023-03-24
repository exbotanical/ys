#include "cors.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>  // for strlen

#include "header.h"  // for req_header_get, res_setheader, derive_headers
#include "libutil/libutil.h"  // for s_*
#include "util.h"             // for str_join
#include "xmalloc.h"

static cors_t *cors_config;

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
static bool is_origin_allowed(cors_t *c, char *origin) {
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
static bool is_method_allowed(cors_t *c, char *method) {
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
static bool are_headers_allowed(cors_t *c, array_t *headers) {
  if (c->allow_all_headers || !has_elements(headers)) {
    return true;
  }

  foreach (headers, i) {
    char *header = to_canonical_MIME_header_key(array_get(headers, i));
    bool allows_header = false;

    if (array_includes(c->allowed_headers, match, header)) {
      allows_header = true;
    }

    if (!allows_header) {
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
static bool is_preflight_request(req_t *req) {
  bool is_options_req =
      s_equals(req->method, http_method_names[METHOD_OPTIONS]);
  bool has_origin_header =
      !s_nullish(req_header_get(req->headers, ORIGIN_HEADER));
  bool has_request_method =
      !s_nullish(req_header_get(req->headers, REQUEST_METHOD_HEADER));

  return is_options_req && has_origin_header && has_request_method;
}

/**
 * handleRequest handles actual HTTP requests subsequent to or standalone from
 * Preflight requests
 * TODO: test
 * @param req
 * @param res
 */
static void handle_request(req_t *req, res_t *res) {
  char *origin = req_header_get(req->headers, ORIGIN_HEADER);
  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  res_setheader(res, VARY_HEADER, ORIGIN_HEADER);

  // If no origin was specified, this is not a valid CORS request
  if (!origin || s_equals(origin, "")) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_config, origin)) {
    res_setstatus(res, STATUS_FORBIDDEN);
    return;
  }

  if (cors_config->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    res_setheader(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    res_setheader(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // If we've exposed headers, set them
  // If the consumer specified headers that are exposed by default, we'll still
  // include them - this is spec compliant
  if (has_elements(cors_config->exposed_headers)) {
    res_setheader(res, EXPOSE_HEADERS_HEADER,
                  str_join(cors_config->exposed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `withCredentials` to `true`
  if (cors_config->allow_credentials) {
    res_setheader(res, ALLOW_CREDENTIALS_HEADER, "true");
  }
}

/**
 * handle_preflight_request handles preflight requests
 * TODO: test
 * @param req
 * @param res
 */
static void handle_preflight_request(req_t *req, res_t *res) {
  char *origin = req_header_get(req->headers, ORIGIN_HEADER);

  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  res_setheader(res, VARY_HEADER,
                fmt_str("%s,%s,%s", ORIGIN_HEADER, REQUEST_METHOD_HEADER,
                        REQUEST_HEADERS_HEADER));

  // If no origin was specified, this is not a valid CORS request
  if (s_nullish(origin)) {
    return;
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(cors_config, origin)) {
    res_setstatus(res, STATUS_FORBIDDEN);

    return;
  }

  // Validate the method; this is the crux of the Preflight
  char *req_method_header = req_header_get(req->headers, REQUEST_METHOD_HEADER);

  if (!is_method_allowed(cors_config, req_method_header)) {  // TODO: nullcheck
    return;
  }

  // Validate request headers. Preflights are also used when
  // requests include additional headers from the client
  char *header_str = req_header_get(req->headers, REQUEST_HEADERS_HEADER);

  array_t *reqd_headers = derive_headers(header_str);
  if (!are_headers_allowed(cors_config, reqd_headers)) {
    return;
  }

  if (cors_config->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    res_setheader(res, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    res_setheader(res, ALLOW_ORIGINS_HEADER, origin);
  }

  // Set the allowed methods, as a Preflight may have been sent if the client
  // included non-simple methods
  res_setheader(res, ALLOW_METHODS_HEADER, req_method_header);

  // Set the allowed headers, as a Preflight may have been sent if the client
  // included non-simple headers.
  if (has_elements(reqd_headers)) {
    res_setheader(res, ALLOW_HEADERS_HEADER,
                  str_join(cors_config->allowed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `withCredentials` to `true`.
  if (cors_config->allow_credentials) {
    res_setheader(res, ALLOW_CREDENTIALS_HEADER, "true");
  }

  // Set the Max Age. This is only necessary for Preflights given the Max Age
  // refers to server-suggested duration, in seconds, a response should stay in
  // the browser's cache before another Preflight is made
  if (cors_config->max_age > 0) {
    res_setheader(res, MAX_AGE_HEADER, fmt_str("%d", cors_config->max_age));
  }
}

cors_opts_t *cors_opts_init() {
  cors_opts_t *o = xmalloc(sizeof(cors_opts_t));
  o->allow_credentials = false;
  o->max_age = 0;
  o->use_options_passthrough = false;
}

cors_t *cors_init(cors_opts_t *opts) {
  cors_config = xmalloc(sizeof(cors_t));
  cors_config->allow_credentials = opts->allow_credentials;
  cors_config->max_age = opts->max_age;
  cors_config->exposed_headers = opts->expose_headers;
  cors_config->use_options_passthrough = opts->use_options_passthrough;
  cors_config->allowed_origins = array_init();
  cors_config->allowed_methods = array_init();

  // Register origins - if no given origins, default to allow all i.e. "*"
  if (!has_elements(opts->allowed_origins)) {
    cors_config->allow_all_origins = true;
  } else {
    foreach (opts->allowed_origins, i) {
      char *origin = array_get(opts->allowed_origins, i);
      if (s_equals("*", origin)) {
        cors_config->allow_all_origins = true;
        break;
      }
      array_push(cors_config->allowed_origins, origin);
    }

    // Append "null" to allow list to support testing / requests from files,
    // redirects, etc. Note: Used for redirects because the browser should not
    // expose the origin of the new server; redirects are followed
    // automatically
    array_push(cors_config->allowed_origins, "null");
  }

  // Register headers - if no given headers, default to those allowed per the
  // spec. Although these headers are allowed by default, we add them anyway for
  // the sake of consistency
  cors_config->allowed_headers = array_init();
  if (has_elements(opts->allowed_headers)) {
    foreach (opts->allowed_headers, i) {
      char *header = array_get(opts->allowed_headers, i);

      if (s_equals("*", header)) {
        cors_config->allow_all_headers = true;
        break;
      }

      array_push(cors_config->allowed_headers,
                 to_canonical_MIME_header_key(header));
    }
  }

  array_push(cors_config->allowed_headers, (void *)ORIGIN_HEADER);

  if (!has_elements(opts->allowed_methods)) {
    // Default allowed methods. Defaults to simple methods (those that do not
    // trigger a Preflight)
    array_t *default_allowed_methods =
        array_collect(s_copy(http_method_names[METHOD_GET]),
                      s_copy(http_method_names[METHOD_POST]),
                      s_copy(http_method_names[METHOD_HEAD]));

    cors_config->allowed_methods = default_allowed_methods;
  } else {
    foreach (opts->allowed_methods, i) {
      char *method = array_get(opts->allowed_methods, i);
      array_push(cors_config->allowed_methods, s_upper(method));
    }
  }

  return cors_config;
}

cors_opts_t *cors_allow_all() {
  cors_opts_t *c = xmalloc(sizeof(cors_opts_t));
  c->allowed_origins = array_collect("*");
  c->allowed_methods = array_collect(s_copy(http_method_names[METHOD_HEAD]),
                                     s_copy(http_method_names[METHOD_GET]),
                                     s_copy(http_method_names[METHOD_POST]),
                                     s_copy(http_method_names[METHOD_PATCH]),
                                     s_copy(http_method_names[METHOD_PUT]),
                                     s_copy(http_method_names[METHOD_DELETE]));
  c->allowed_headers = array_collect("*");
  c->allow_credentials = false;

  return c;
}

res_t *cors_handler(req_t *req, res_t *res) {
  if (is_preflight_request(req)) {
    handle_preflight_request(req, res);

    if (cors_config->use_options_passthrough) {
      // Do nothing i.e. allow next handler to run
    } else {
      res_setstatus(res, STATUS_NO_CONTENT);
      // We're done
      res->done = true;
    }
  } else {
    handle_request(req, res);
    // Do nothing i.e. allow next handler to run
  }

  return res;
}

void set_allow_credentials(cors_opts_t *c, bool allow) {
  c->allow_credentials = allow;
}

void set_use_options_passthrough(cors_opts_t *c, bool use) {
  c->use_options_passthrough = use;
}

void set_max_age(cors_opts_t *c, int max_age) { c->max_age = max_age; }
