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
typedef enum {

  // MUST start with 1 for varargs handling e.g. collect
  METHOD_GET = 1,
  METHOD_POST,
  METHOD_PUT,
  METHOD_PATCH,  // RFC 5789
  METHOD_DELETE,
  METHOD_OPTIONS,
  METHOD_HEAD,
  METHOD_CONNECT,
  METHOD_TRACE
} http_method_t;

// HTTP status codes.
typedef enum {
  STATUS_CONTINUE = 100,             // RFC 7231, 6.2.1
  STATUS_SWITCHING_PROTOCOLS = 101,  // RFC 7231, 6.2.2
  STATUS_PROCESSING = 102,           // RFC 2518, 10.1
  STATUS_EARLY_HINTS = 103,          // RFC 8297

  STATUS_OK = 200,                      // RFC 7231, 6.3.1
  STATUS_CREATED = 201,                 // RFC 7231, 6.3.2
  STATUS_ACCEPTED = 202,                // RFC 7231, 6.3.3
  STATUS_NON_AUTHORITATIVE_INFO = 203,  // RFC 7231, 6.3.4
  STATUS_NO_CONTENT = 204,              // RFC 7231, 6.3.5
  STATUS_RESET_CONTENT = 205,           // RFC 7231, 6.3.6
  STATUS_PARTIAL_CONTENT = 206,         // RFC 7233, 4.1
  STATUS_MULTISTATUS = 207,             // RFC 4918, 11.1
  STATUS_ALREADY_REPORTED = 208,        // RFC 5842, 7.1
  STATUS_IM_USED = 226,                 // RFC 3229, 10.4.1

  STATUS_MULTIPLE_CHOICES = 300,    // RFC 7231, 6.4.1
  STATUS_MOVED_PERMANENTLY = 301,   // RFC 7231, 6.4.2
  STATUS_FOUND = 302,               // RFC 7231, 6.4.3
  STATUS_SEE_OTHER = 303,           // RFC 7231, 6.4.4
  STATUS_NOT_MODIFIED = 304,        // RFC 7232, 4.1
  STATUS_USE_PROXY = 305,           // RFC 7231, 6.4.5
  STATUS_NULL_ = 306,               // RFC 7231, 6.4.6 (Unused)
  STATUS_TEMPORARY_REDIRECT = 307,  // RFC 7231, 6.4.7
  STATUS_PERMANENT_REDIRECT = 308,  // RFC 7538, 3

  STATUS_BAD_REQUEST = 400,                      // RFC 7231, 6.5.1
  STATUS_UNAUTHORIZED = 401,                     // RFC 7235, 3.1
  STATUS_PAYMENT_REQUIRED = 402,                 // RFC 7231, 6.5.2
  STATUS_FORBIDDEN = 403,                        // RFC 7231, 6.5.3
  STATUS_NOT_FOUND = 404,                        // RFC 7231, 6.5.4
  STATUS_METHOD_NOT_ALLOWED = 405,               // RFC 7231, 6.5.5
  STATUS_NOT_ACCEPTABLE = 406,                   // RFC 7231, 6.5.6
  STATUS_PROXY_AUTH_REQUIRED = 407,              // RFC 7235, 3.2
  STATUS_REQUEST_TIMEOUT = 408,                  // RFC 7231, 6.5.7
  STATUS_CONFLICT = 409,                         // RFC 7231, 6.5.8
  STATUS_GONE = 410,                             // RFC 7231, 6.5.9
  STATUS_LENGTH_REQUIRED = 411,                  // RFC 7231, 6.5.10
  STATUS_PRECONDITION_FAILED = 412,              // RFC 7232, 4.2
  STATUS_REQUEST_ENTITY_TOO_LARGE = 413,         // RFC 7231, 6.5.11
  STATUS_REQUEST_URI_TOO_LONG = 414,             // RFC 7231, 6.5.12
  STATUS_UNSUPPORTED_MEDIA_TYPE = 415,           // RFC 7231, 6.5.13
  STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,  // RFC 7233, 4.4
  STATUS_EXPECTATION_FAILED = 417,               // RFC 7231, 6.5.14
  STATUS_TEAPOT = 418,                           // RFC 7168, 2.3.3
  STATUS_MISDIRECTED_REQUEST = 421,              // RFC 7540, 9.1.2
  STATUS_UNPROCESSABLE_ENTITY = 422,             // RFC 4918, 11.2
  STATUS_LOCKED = 423,                           // RFC 4918, 11.3
  STATUS_FAILED_DEPENDENCY = 424,                // RFC 4918, 11.4
  STATUS_TOO_EARLY = 425,                        // RFC 8470, 5.2.
  STATUS_UPGRADE_REQUIRED = 426,                 // RFC 7231, 6.5.15
  STATUS_PRECONDITION_REQUIRED = 428,            // RFC 6585, 3
  STATUS_TOO_MANY_REQUESTS = 429,                // RFC 6585, 4
  STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,  // RFC 6585, 5
  STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,    // RFC 7725, 3

  STATUS_INTERNAL_SERVER_ERROR = 500,            // RFC 7231, 6.6.1
  STATUS_NOT_IMPLEMENTED = 501,                  // RFC 7231, 6.6.2
  STATUS_BAD_GATEWAY = 502,                      // RFC 7231, 6.6.3
  STATUS_SERVICE_UNAVAILABLE = 503,              // RFC 7231, 6.6.4
  STATUS_GATEWAY_TIMEOUT = 504,                  // RFC 7231, 6.6.5
  STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,       // RFC 7231, 6.6.6
  STATUS_VARIANT_ALSO_NEGOTIATES = 506,          // RFC 2295, 8.1
  STATUS_INSUFFICIENT_STORAGE = 507,             // RFC 4918, 11.5
  STATUS_LOOP_DETECTED = 508,                    // RFC 5842, 7.2
  STATUS_NOT_EXTENDED = 510,                     // RFC 2774, 7
  STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,  // RFC 6585, 6
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
  // A flag used by middleware - setting this to true will stop the middleware
  // chain and prevent subsequent middlewares from being run
  bool done;

  // HTTP status code - required
  http_status_t status;
  // HTTP headers - optional, but you should pass content-type if sending a body
  array_t *headers;
  // res_t body - optional; Content-length header will be set for you
  char *body;
} res_t;

// An alias type for request handlers
typedef res_t *handler_t(req_t *, res_t *);

// CORS configuration options
typedef struct {
  bool allow_credentials;
  bool use_options_passthrough;
  unsigned int max_age;
  array_t *allowed_origins;
  array_t *allowed_methods;
  array_t *allowed_headers;
  array_t *expose_headers;
} __cors_opts_t;
typedef __cors_opts_t cors_opts_t;

// A router object
typedef struct {
  bool use_cors;
  trie_t *trie;
  handler_t *not_found_handler;
  handler_t *method_not_allowed_handler;
  handler_t *internal_error_handler;
  array_t *middlewares;
} __router_t;
typedef __router_t *router_t;

typedef struct {
  bool use_cors;
  handler_t *not_found_handler;
  handler_t *internal_error_handler;
  handler_t *method_not_allowed_handler;
  array_t *middlewares;
} router_attr_t;

#define ROUTE_ATTR_INITIALIZER \
  { false, NULL, NULL, NULL, NULL, }

// A server configuration object that stores settings for the HTTP server
typedef struct {
  __router_t *router;
  int port;
} __server_t;
typedef __server_t *server_t;

typedef array_t *middlewares_t;

/**
 * set_header appends the given key/value pair as a header object in
 * `headers`
 *
 * @param res
 * @param key
 * @param value
 * @return bool
 */
bool set_header(res_t *res, const char *key, const char *value);

/**
 * res_setbody sets the given body on the given response
 *
 * @param response
 * @param body
 */
void res_setbody(res_t *response, const char *body);

/**
 * res_setstatus sets the given status code on the given response
 *
 * @param response
 * @param status
 */
void res_setstatus(res_t *response, http_status_t status);

/**
 * router_free deallocates memory for router_t `router`
 *
 * @param router
 */
void router_free(router_t *router);

/**
 * router_init allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be used).
 * @param router_attr_t TODO:
 * @return router_t*
 */
router_t *router_init(router_attr_t attr);

/**
 * router_register registers a new route record. Registered routes will be
 * matched against when used with `router_run`
 *
 * @param router The router instance in which to register the route
 * @param path The path to associate with the route
 * @param handler The handler to associate with the route
 * @param method
 * @param ...Methods to associate with the route
 */
void router_register(router_t *router, const char *path, handler_t *handler,
                     http_method_t method, ...);

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
 * @return unsigned int
 */
unsigned int req_num_parameters(req_t *req);

/**
 * req_has_parameters returns a bool indicating whether the given request has
 * any parameters
 *
 * @param req
 * @return bool
 */
bool req_has_parameters(req_t *req);

/**
 * middlewares binds n middleware handlers to the router attributes instance
 *
 * @param router_attr_t*
 * @param handler_t*[]
 */
#define middlewares(r, ...) __middlewares(r, __VA_ARGS__, NULL)

void __middlewares(router_attr_t *r, handler_t *mw, ...);
void add_middleware(router_attr_t *r, handler_t *mw);

/**
 * set_allowed_origins sets the cors_opts_t allowed origins
 * @param o cors_opts_t*
 * @param ... char*[]
 */
#define set_allowed_origins(c, ...) \
  c->allowed_origins = array_collect(__VA_ARGS__)

/**
 * set_allowed_methods sets the cors_opts_t allowed methods
 * @param o cors_opts_t*
 * @param ... char*[]
 */
#define set_allowed_methods(c, ...) \
  c->allowed_methods = array_collect(__VA_ARGS__)

/**
 * set_allowed_headers sets the cors_opts_t allowed headers
 * @param o cors_opts_t*
 * @param ... char*[]
 */
#define set_allowed_headers(o, ...) \
  o->allowed_headers = array_collect(__VA_ARGS__)

/**
 * cors_opts_init initializes a new CORS options object
 *
 * @return cors_opts_t*
 */
cors_opts_t *cors_opts_init();

/**
 * set_allow_credentials sets the cors_opts_t allowed_credentials option
 *
 * @param c
 * @param allow
 */
void set_allow_credentials(cors_opts_t *c, bool allow);

/**
 * set_use_options_passthrough sets the cors_opts_t use_options_passthrough
 * option
 *
 * @param c
 * @param use
 */
void set_use_options_passthrough(cors_opts_t *c, bool use);

/**
 * set_max_age sets the cors_opts_t max age option
 *
 * @param c
 * @param max_age
 */
void set_max_age(cors_opts_t *c, int max_age);

/**
 * cors_allow_all initializes a new CORS options object with sensible, lax
 * defaults
 *
 * @return cors_opts_t*
 */
cors_opts_t *cors_allow_all();

/**
 * use_cors binds the CORS global middleware to the router attributes instance
 * TODO: test
 * @param r
 * @param opts
 */
void use_cors(router_attr_t *r, cors_opts_t *opts);

#endif /* LIBHTTP_H */
