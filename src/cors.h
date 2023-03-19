#ifndef CORS_H
#define CORS_H

#include "libhttp.h"
#include "libutil/libutil.h"  // for arrays

// CORS configuration options
typedef struct {
  array_t *allowed_origins;
  array_t *allowed_methods;
  array_t *allowed_headers;
  array_t *expose_headers;
  bool allow_credentials;
  bool use_options_passthrough;
  unsigned int max_age;
} cors_opts_t;

// CORS configurations
typedef struct {
  array_t *allowed_origins;
  array_t *allowed_methods;
  array_t *allowed_headers;
  array_t *exposed_headers;
  bool allow_credentials;
  bool allow_all_origins;
  bool allow_all_headers;
  bool use_options_passthrough;
  unsigned int max_age;
} cors_t;

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

/**
 * cors_init initializes a CORS context using the user-specified settings on the
 * given cors_opts_t `opts`
 *
 * @param opts
 * @return cors_t*
 */
cors_t *cors_init(cors_opts_t *opts);

/**
 * cors_handler is a middleware handler that executes a spec-compliant CORS
 * workflow
 *
 * @param c
 * @param req
 * @param res
 * @return req_t*
 */
req_t *cors_handler(cors_t *c, req_t *req, res_t *res);

/**
 * derive_headers extracts the headers in the value of the
 * `Access-Control-Request-Headers` header
 *
 * @param req
 * @return array_t*
 * TODO: move to headers.h and remove enum specificity
 */
array_t *derive_headers(req_t *req);

/**
 * are_headers_allowed determines whether the given headers are allowed per
 * the user-defined allow list
 *
 * @param c
 * @param headers
 * @return bool
 */
bool are_headers_allowed(cors_t *c, array_t *headers);

/**
 * is_method_allowed determines whether the given method is allowed per the
 * user-defined allow list
 *
 * @param c
 * @param method
 * @return bool
 */
bool is_method_allowed(cors_t *c, char *method);

/**
 * is_origin_allowed determines whether the given origin is allowed per the
 * user-defined allow list
 *
 * @param c
 * @param origin
 * @return bool
 */
bool is_origin_allowed(cors_t *c, char *origin);

#endif /* CORS_H */