#ifndef LIBHTTP_H
#define LIBHTTP_H

#include <stdbool.h>

#include "libhash/libhash.h"  // for hash tables
#include "libutil/libutil.h"
#include "trie.h"

// HTTP status names.
extern const char *http_status_names[];

// HTTP method names.
extern const char *http_method_names[];

// HTTP methods.
typedef enum http_method {
  // MUST start with 1 for varargs handling e.g. collect
  GET = 1,
  POST,
  PUT,
  PATCH,  // RFC 5789
  DELETE,
  OPTIONS,
  HEAD,
  CONNECT,
  TRACE
} http_method_t;

// HTTP status codes.
typedef enum http_status {
  CONTINUE = 100,             // RFC 7231, 6.2.1
  SWITCHING_PROTOCOLS = 101,  // RFC 7231, 6.2.2
  PROCESSING = 102,           // RFC 2518, 10.1
  EARLY_HINTS = 103,          // RFC 8297

  OK = 200,                      // RFC 7231, 6.3.1
  CREATED = 201,                 // RFC 7231, 6.3.2
  ACCEPTED = 202,                // RFC 7231, 6.3.3
  NON_AUTHORITATIVE_INFO = 203,  // RFC 7231, 6.3.4
  NO_CONTENT = 204,              // RFC 7231, 6.3.5
  RESET_CONTENT = 205,           // RFC 7231, 6.3.6
  PARTIAL_CONTENT = 206,         // RFC 7233, 4.1
  MULTISTATUS = 207,             // RFC 4918, 11.1
  ALREADY_REPORTED = 208,        // RFC 5842, 7.1
  IM_USED = 226,                 // RFC 3229, 10.4.1

  MULTIPLE_CHOICES = 300,    // RFC 7231, 6.4.1
  MOVED_PERMANENTLY = 301,   // RFC 7231, 6.4.2
  FOUND = 302,               // RFC 7231, 6.4.3
  SEE_OTHER = 303,           // RFC 7231, 6.4.4
  NOT_MODIFIED = 304,        // RFC 7232, 4.1
  USE_PROXY = 305,           // RFC 7231, 6.4.5
  NULL_ = 306,               // RFC 7231, 6.4.6 (Unused)
  TEMPORARY_REDIRECT = 307,  // RFC 7231, 6.4.7
  PERMANENT_REDIRECT = 308,  // RFC 7538, 3

  BAD_REQUEST = 400,                      // RFC 7231, 6.5.1
  UNAUTHORIZED = 401,                     // RFC 7235, 3.1
  PAYMENT_REQUIRED = 402,                 // RFC 7231, 6.5.2
  FORBIDDEN = 403,                        // RFC 7231, 6.5.3
  NOT_FOUND = 404,                        // RFC 7231, 6.5.4
  METHOD_NOT_ALLOWED = 405,               // RFC 7231, 6.5.5
  NOT_ACCEPTABLE = 406,                   // RFC 7231, 6.5.6
  PROXY_AUTH_REQUIRED = 407,              // RFC 7235, 3.2
  REQUEST_TIMEOUT = 408,                  // RFC 7231, 6.5.7
  CONFLICT = 409,                         // RFC 7231, 6.5.8
  GONE = 410,                             // RFC 7231, 6.5.9
  LENGTH_REQUIRED = 411,                  // RFC 7231, 6.5.10
  PRECONDITION_FAILED = 412,              // RFC 7232, 4.2
  REQUEST_ENTITY_TOO_LARGE = 413,         // RFC 7231, 6.5.11
  REQUEST_URI_TOO_LONG = 414,             // RFC 7231, 6.5.12
  UNSUPPORTED_MEDIA_TYPE = 415,           // RFC 7231, 6.5.13
  REQUESTED_RANGE_NOT_SATISFIABLE = 416,  // RFC 7233, 4.4
  EXPECTATION_FAILED = 417,               // RFC 7231, 6.5.14
  TEAPOT = 418,                           // RFC 7168, 2.3.3
  MISDIRECTED_REQUEST = 421,              // RFC 7540, 9.1.2
  UNPROCESSABLE_ENTITY = 422,             // RFC 4918, 11.2
  LOCKED = 423,                           // RFC 4918, 11.3
  FAILED_DEPENDENCY = 424,                // RFC 4918, 11.4
  TOO_EARLY = 425,                        // RFC 8470, 5.2.
  UPGRADE_REQUIRED = 426,                 // RFC 7231, 6.5.15
  PRECONDITION_REQUIRED = 428,            // RFC 6585, 3
  TOO_MANY_REQUESTS = 429,                // RFC 6585, 4
  REQUEST_HEADER_FIELDS_TOO_LARGE = 431,  // RFC 6585, 5
  UNAVAILABLE_FOR_LEGAL_REASONS = 451,    // RFC 7725, 3

  INTERNAL_SERVER_ERROR = 500,            // RFC 7231, 6.6.1
  NOT_IMPLEMENTED = 501,                  // RFC 7231, 6.6.2
  BAD_GATEWAY = 502,                      // RFC 7231, 6.6.3
  SERVICE_UNAVAILABLE = 503,              // RFC 7231, 6.6.4
  GATEWAY_TIMEOUT = 504,                  // RFC 7231, 6.6.5
  HTTP_VERSION_NOT_SUPPORTED = 505,       // RFC 7231, 6.6.6
  VARIANT_ALSO_NEGOTIATES = 506,          // RFC 2295, 8.1
  INSUFFICIENT_STORAGE = 507,             // RFC 4918, 11.5
  LOOP_DETECTED = 508,                    // RFC 5842, 7.2
  NOT_EXTENDED = 510,                     // RFC 2774, 7
  NETWORK_AUTHENTICATION_REQUIRED = 511,  // RFC 6585, 6

} http_status_t;

// A context object containing metadata to be passed to matched route handlers
typedef struct {
  const char *path;
  const char *method;
  const char *accept;
  const char *body;
  const char *raw;
  const char *host;
  const char *protocol;
  const char *user_agent;
  const char *content_type;
  const char *version;
  array_t *parameters;
  hash_table *headers;
  int content_length;
} req_t;

// TODO: relocate + opaque
typedef struct {
  const char *key;
  const char *value;
} header_t;

// A response object; client handlers must return this struct to be sent to the
// client.
typedef struct {
  // HTTP status code - required
  http_status_t status;
  // HTTP headers - optional, but you should pass content-type if sending a body
  array_t *headers;
  // res_t body - optional; Content-length header will be set for you
  char *body;
} res_t;

// An alias type for request handlers
typedef res_t *handler_t(req_t *, res_t *);

// A router object
typedef struct {
  trie_t *trie;
  handler_t *not_found_handler;
  handler_t *method_not_allowed_handler;
} __router_t;
typedef __router_t *router_t;

// A server configuration object that stores settings for the HTTP server
typedef struct {
  __router_t *router;
  int port;
} __server_t;
typedef __server_t *server_t;

/**
 * set_header sets the given header on the given response
 *
 * @param response
 * @param header
 * @return bool
 */
bool set_header(res_t *response, char *header);

/**
 * set_body sets the given body on the given response
 *
 * @param response
 * @param body
 */
void set_body(res_t *response, const char *body);

/**
 * set_status sets the given status code on the given response
 *
 * @param response
 * @param status
 */
void set_status(res_t *response, http_status_t status);

array_t *collect_middleware(handler_t *middleware, ...);

/**
 * router_free deallocates memory for router_t `router`
 *
 * @param router
 */
void router_free(router_t *router);

/**
 * router_init allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be used).
 * @param not_found_handler
 * @param method_not_allowed_handler
 * @return router_t*
 */
router_t *router_init(handler_t *not_found_handler,
                      handler_t *method_not_allowed_handler);

/**
 * router_register registers a new route record. Registered routes will be
 * matched against when used with `router_run`
 *
 * @param router The router instance in which to register the route
 * @param path The path to associate with the route
 * @param handler The handler to associate with the route
 * @param middlewares Additional handlers that will be invoked in a chain prior
 * to `handler`
 * @param method
 * @param ...Methods to associate with the route
 */
void router_register(router_t *router, const char *path, handler_t *handler,
                     array_t *middlewares, http_method_t method, ...);

/**
 * server_init allocates the necessary memory for a `server_t`
 *
 * @param router
 * @param port
 */
server_t *server_init(router_t *router, int port);

/**
 * server_start listens for client connections and executes routing
 *
 * @param server
 * @returns bool indicating whether this blocking operation failed
 */
bool server_start(server_t *server);

/**
 * Deallocates a server_t
 *
 * @param server
 */
void server_free(server_t *server);

/**
 * req_get_parameter gets parameter value matching `key`, or NULL if not extant
 *
 * @param req
 * @param key
 * @return void*
 */
void *req_get_parameter(req_t *req, const char *key);

/**
 * req_get_parameter_at gets a request parameter at the specified index
 *
 * @param req
 * @param idx
 * @return parameter_t*
 */
parameter_t *req_get_parameter_at(req_t *req, unsigned int idx);

/**
 * req_num_parameters returns the number of parameters on the given request
 *
 * @param req
 * @return true
 * @return false
 */
bool req_num_parameters(req_t *req);

/**
 * req_has_parameters returns a bool indicating whether the given request has
 * any parameters
 *
 * @param req
 * @return bool
 */
bool req_has_parameters(req_t *req);

#endif /* LIBHTTP_H */
