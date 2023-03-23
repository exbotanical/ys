#ifndef CORS_H
#define CORS_H

#include "libhttp.h"
#include "libutil/libutil.h"  // for arrays

// CORS configurations
typedef struct {
  unsigned int max_age;
  bool allow_credentials;
  bool allow_all_headers;
  bool use_options_passthrough;
  bool allow_all_origins;
  array_t *allowed_methods;
  array_t *allowed_origins;
  array_t *allowed_headers;
  array_t *exposed_headers;
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
 * @param req
 * @param res
 * @return res_t*
 */
res_t *cors_handler(req_t *req, res_t *res);

#endif /* CORS_H */
