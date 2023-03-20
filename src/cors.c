#include "cors.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>  // for strlen

#include "header.h"  // for req_header_get, res_header_append
#include "util.h"
#include "xmalloc.h"

/**
 * match is a matcher function for comparing array values against a given value
 * `s2`
 *
 * @param s1
 * @param s2
 * @return true
 * @return false
 */
bool match(void *s1, void *s2) {
  return safe_strcasecmp((char *)s1, (char *)s2);
}

/**
 * isPreflightRequest determines whether the given request is a Preflight.
 * A Preflight must:
 * 1) use the OPTIONS method
 * 2) include an Origin request header
 * 3) Include an Access-Control-Request-Method header
 *
 * @param req
 * @return true
 * @return false
 */
static bool is_preflight_request(req_t *req) {
  bool is_options_req = str_equals(req->method, "OPTIONS");
  bool has_origin_header = true;   // TODO:
  bool has_request_method = true;  // TODO:

  return is_options_req && has_origin_header && has_request_method;
}

// TODO: Use hash table for headers and parameters
/**
 * handleRequest handles actual HTTP requests subsequent to or standalone from
 * Preflight requests
 *
 * @param request
 * @param response
 * @return res_t*
 */
static res_t *handle_request(cors_t *c, req_t *req, res_t *res) {
  char *origin = req_header_get(req->headers, ORIGIN_HEADER);
  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  res_header_append(res->headers, VARY_HEADER, ORIGIN_HEADER);

  // If no origin was specified, this is not a valid CORS request
  if (!origin || str_equals(origin, "")) {
    // TODO: nope

    return res;
  }

  // If the origin is not in the allow list, deny
  // TODO: case insensitive
  if (!is_origin_allowed(c, origin)) {
    // TODO: 403
    return res;  // TODO: nope
  }

  if (c->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    res_header_append(res->headers, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    res_header_append(res->headers, ALLOW_ORIGINS_HEADER, origin);
  }

  // If we've exposed headers, set them
  // If the consumer specified headers that are exposed by default, we'll still
  // include them - this is spec compliant
  if (c->exposed_headers && array_size(c->exposed_headers) > 0) {
    res_header_append(res->headers, EXPOSE_HEADERS_HEADER,
                      str_join(c->exposed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `withCredentials` to `true`
  if (c->allow_credentials) {
    res_header_append(res->headers, ALLOW_CREDENTIALS_HEADER, "true");
  }

  return res;
}

/**
 * handle_preflight_request handles preflight requests
 *
 * @param c
 * @param req
 * @param res
 * @return res_t*
 */
static res_t *handle_preflight_request(cors_t *c, req_t *req, res_t *res) {
  char *origin = req_header_get(req->headers, ORIGIN_HEADER);

  // Set the "vary" header to prevent proxy servers from sending cached
  // responses for one client to another
  res_header_append(res->headers, VARY_HEADER,
                    fmt_str("%s,%s,%s", ORIGIN_HEADER, REQUEST_METHOD_HEADER,
                            REQUEST_HEADERS_HEADER));

  // If no origin was specified, this is not a valid CORS request
  if (str_equals(origin, "")) {
    return NULL;  // TODO: NOPE
  }

  // If the origin is not in the allow list, deny
  if (!is_origin_allowed(c, origin)) {
    return NULL;  // TODO: NOPE
  }

  // Validate the method; this is the crux of the Preflight
  char *req_method_header = req_header_get(req->headers, REQUEST_METHOD_HEADER);

  if (!is_method_allowed(c, req_method_header)) {
    return NULL;  // TODO: NOPE
  }

  // Validate request headers. Preflights are also used when
  // requests include additional headers from the client
  array_t *reqd_headers = derive_headers(req);
  if (!are_headers_allowed(c, reqd_headers)) {
    return NULL;  // TODO: NOPE
  }

  if (c->allow_all_origins) {
    // If all origins are allowed, use the wildcard value
    res_header_append(res->headers, ALLOW_ORIGINS_HEADER, "*");
  } else {
    // Otherwise, set the origin to the request origin
    res_header_append(res->headers, ALLOW_ORIGINS_HEADER, origin);
  }

  // Set the allowed methods, as a Preflight may have been sent if the client
  // included non-simple methods
  res_header_append(res->headers, ALLOW_METHODS_HEADER, req_method_header);

  // Set the allowed headers, as a Preflight may have been sent if the client
  // included non-simple headers.
  if (array_size(reqd_headers) > 0) {
    res_header_append(res->headers, ALLOW_HEADERS_HEADER,
                      str_join(c->allowed_headers, ", "));
  }

  // Allow the client to send credentials. If making an XHR request, the client
  // must set `withCredentials` to `true`.
  if (c->allow_credentials) {
    res_header_append(res->headers, ALLOW_CREDENTIALS_HEADER, "true");
  }

  // Set the Max Age. This is only necessary for Preflights given the Max Age
  // refers to server-suggested duration, in seconds, a response should stay in
  // the browser's cache before another Preflight is made
  if (c->max_age > 0) {
    res_header_append(res->headers, MAX_AGE_HEADER, fmt_str("%d", c->max_age));
  }

  return res;
}

cors_t *cors_init(cors_opts_t *opts) {
  cors_t *c = xmalloc(sizeof(cors_t));
  c->allow_credentials = opts->allow_credentials;
  c->max_age = opts->max_age;
  c->exposed_headers = opts->expose_headers;
  c->use_options_passthrough = opts->use_options_passthrough;
  c->allowed_origins = array_init();
  c->allowed_methods = array_init();

  // Register origins - if no given origins, default to allow all i.e. "*"
  // TODO: have array_size handle NULL?
  if (!opts->allowed_origins || array_size(opts->allowed_origins) == 0) {
    c->allow_all_origins = true;
  } else {
    foreach (opts->allowed_origins, i) {
      char *origin = array_get(opts->allowed_origins, i);
      if (str_equals("*", origin)) {
        c->allow_all_origins = true;
        break;
      }

      array_push(c->allowed_origins, origin);
    }

    // Append "null" to allow list to support testing / requests from files,
    // redirects, etc. Note: Used for redirects because the browser should not
    // expose the origin of the new server; redirects are followed
    // automatically
    array_push(c->allowed_origins, "null");
  }

  // Register headers - if no given headers, default to those allowed per the
  // spec. Although these headers are allowed by default, we add them anyway for
  // the sake of consistency
  if (!opts->allowed_headers || array_size(opts->allowed_headers) == 0) {
    // Default allowed headers. Defaults to the "Origin" header, though this
    // should be included automatically
    array_t *default_allowed_headers = array_init();
    array_push(default_allowed_headers, (void *)ORIGIN_HEADER);

    c->allowed_headers = default_allowed_headers;
  } else {
    c->allowed_headers = array_init();

    foreach (opts->allowed_headers, i) {
      char *header = array_get(opts->allowed_headers, i);

      if (str_equals("*", header)) {
        c->allow_all_headers = true;
        break;
      }

      array_push(c->allowed_headers, to_canonical_MIME_header_key(header));
    }
  }

  // TODO: in fact, these should be initialized anyway
  if (!opts->allowed_methods || array_size(opts->allowed_methods) == 0) {
    // Default allowed methods. Defaults to simple methods (those that do not
    // trigger a Preflight)
    array_t *default_allowed_methods = array_init();
    array_push(default_allowed_methods, (void *)http_method_names[METHOD_GET]);
    array_push(default_allowed_methods, (void *)http_method_names[METHOD_POST]);
    array_push(default_allowed_methods, (void *)http_method_names[METHOD_HEAD]);

    c->allowed_methods = default_allowed_methods;
  } else {
    foreach (opts->allowed_methods, i) {
      char *method = array_get(opts->allowed_methods, i);
      array_push(c->allowed_methods, to_upper(method));
    }
  }

  return c;
}

// TODO: handle
res_t *cors_handler(cors_t *c, req_t *req, res_t *res) {
  if (is_preflight_request(req)) {
    handle_preflight_request(c, req, res);

    if (c->use_options_passthrough) {
      // pass to next handler
    } else {
      set_status(res, STATUS_NO_CONTENT);
      // terminate
    }
  } else {
    handle_request(c, req, res);
    // pass to next handler
  }

  // terminate
  return res;
}

bool are_headers_allowed(cors_t *c, array_t *headers) {
  if (c->allow_all_headers || array_size(headers) == 0) {
    return true;
  }

  // TODO: initialize values
  if (!c->allowed_headers || array_size(c->allowed_headers) == 0) {
    return false;
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

bool is_origin_allowed(cors_t *c, char *origin) {
  if (c->allow_all_origins) {
    return true;
  }

  foreach (c->allowed_origins, i) {
    char *allowed_origin = array_get(c->allowed_origins, i);
    if (safe_strcasecmp(origin, allowed_origin)) {
      return true;
    }
  }

  return false;
}

bool is_method_allowed(cors_t *c, char *method) {
  if (array_size(c->allowed_methods) == 0) {
    return false;
  }

  if (safe_strcasecmp(method, http_method_names[METHOD_OPTIONS])) {
    return true;
  }

  foreach (c->allowed_methods, i) {
    char *allowed_method = array_get(c->allowed_methods, i);
    if (safe_strcasecmp(method, allowed_method)) {
      return true;
    }
  }

  return false;
}

array_t *derive_headers(req_t *req) {
  // TODO: make more dynamic by passing in header_str
  char *header_str = req_header_get(req->headers, REQUEST_HEADERS_HEADER);
  array_t *headers = array_init();

  if (!header_str || str_equals(header_str, "")) {
    return headers;
  }

  unsigned int len = strlen(header_str);
  array_t *tmp = array_init();

  for (unsigned int i = 0; i < len; i++) {
    char c = header_str[i];

    if ((c >= 'a' && c <= 'z') || c == '_' || c == '-' || c == '.' ||
        (c >= '0' && c <= '9')) {
      array_push(tmp, c);
    }

    if (c >= 'A' && c <= 'Z') {
      array_push(tmp, tolower(c));
    }

    if (c == ' ' || c == ',' || i == len - 1) {
      unsigned int size = array_size(tmp);
      if (size > 0) {
        char *v = xmalloc(size + 1);
        unsigned int i;
        for (i = 0; i < size; i++) {
          v[i] = array_get(tmp, i);
        }

        v[i + 1] = '\0';

        array_push(headers, v);

        array_free(tmp);
        tmp = array_init();
      }
    }
  }

  return headers;
}
