#ifndef LIBHTTP_H
#define LIBHTTP_H

#include <stdbool.h>
#include <time.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "trie.h"

/**********************************************************
 * Constants
 **********************************************************/

/**
 * Mappings of HTTP status enums to string literal names
 */
extern const char *http_status_names[];

/**
 * Mappings of HTTP method enums to string literal names
 */
extern const char *http_method_names[];

/**
 * All possible HTTP methods
 */
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
} http_method;

/**
 * All RFC-specified HTTP status codes
 */
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
} http_status;

/**********************************************************
 * Request
 **********************************************************/

/**
 * A request contains request data such as the path, method, body, any route or
 * query parameters, etc.
 */
typedef struct request_internal *request;

/**
 * req_get_parameter retrieves the parameter value matching `key`, or NULL
 * if not extant
 *
 * @param req
 * @param key
 * @return void*
 */
char *req_get_parameter(request *req, const char *key);

/**
 * req_num_parameters returns the number of parameters matched on the given
 * request
 *
 * @param req
 * @return unsigned int
 */
unsigned int req_num_parameters(request *req);

/**
 * req_has_parameters returns a bool indicating whether the given request has
 * any parameters
 *
 * @param req
 * @return true if the request contains any matched parameters
 * @return false if the request contains no matched parameters
 */
bool req_has_parameters(request *req);

/**
 * req_get_query gets the list of values for a given query key
 *
 * @param req
 * @param key
 * @return char** A list of values, or NULL if no match
 * For example, if the request URL was some-url/?key=1&key=2&key=3, calling
 * req_get_query(req, "key") would yield ["1", "2", "3"]. Use `req_num_queries`
 * to retrieve the size of this list.
 */
char **req_get_query(request *req, const char *key);

/**
 * req_has_query tests whether the request has a query match for `key`
 *
 * @param req
 * @param key
 * @return true if the request has at least one query match for the given key
 * @return false if the request has no query matches for the given key
 */
bool req_has_query(request *req, const char *key);

/**
 * req_num_queries returns the number of values associated with a given key
 * `key`
 *
 * @param req
 * @param key
 * @return unsigned int
 */
unsigned int req_num_queries(request *req, const char *key);

/**
 * request_get_path returns the request path / URL
 *
 * @param req
 * @return char*
 */
char *request_get_path(request *req);

/**
 * request_get_method returns the request method
 *
 * @param req
 * @return char*
 */
char *request_get_method(request *req);

/**
 * request_get_body returns the full request body
 *
 * @param req
 * @return char*
 */
char *request_get_body(request *req);

/**
 * request_get_raw returns the entire, raw request as it was received by the
 * server
 *
 * @param req
 * @return char*
 */
char *request_get_raw(request *req);

/**
 * request_get_protocol returns the full protocol version string included in the
 * request
 *
 * @param req
 * @return char*
 */
char *request_get_protocol(request *req);

/**
 * request_get_content_type returns the value of the request's content-type
 * header
 *
 * @param req
 * @return char*
 */
char *request_get_version(request *req);

/**
 * request_get_header retrieves the first value for a given header key on
 * the request or NULL if not found
 *
 *  @param req
 * @param key
 * @return char*
 */
char *request_get_header(request *req, const char *key);

/**********************************************************
 * Response
 **********************************************************/

/**
 * A response holds the necessary metadata for the response that will be
 * sent to the client. Client handlers should use the library-provided
 * helpers to set the desired properties e.g. status, body, and headers.
 */
typedef struct response_internal *response;

/**
 * set_header inserts the given key/value pair as a header on the response
 *
 * @param res
 * @param key
 * @param value
 * @return true if adding the header succeeded
 * @return false if adding the header failed; this would indicate an OOM error
 */
bool set_header(response *res, const char *key, const char *value);

/**
 * set_body sets the given body on the given response
 *
 * @param res
 * @param body
 */
void set_body(response *res, const char *body);

/**
 * set_status sets the given status code on the given response
 *
 * @param res
 * @param status
 */
void set_status(response *res, http_status status);

/**
 * get_done returns the done status of the response
 *
 * @param res
 * @return true
 * @return false
 */
bool get_done(response *res);

/**
 * set_done sets the done status of the response to true. This operation is
 * idempotent.
 *
 * @param res
 */
void set_done(response *res);

/**
 * from_file reads a file into a string buffer, which may then be passed
 * directly to `set_body` e.g. set_body(res, from_file("./index.html"));
 *
 * @param filename
 * @return char*
 */
char *from_file(const char *filename);

/**
 * response_get_header retrieves the first value for a given header key on
 * the response or NULL if not found
 *
 * @param res
 * @param key
 * @return char*
 */
char *response_get_header(response *res, const char *key);

/**********************************************************
 * Router
 **********************************************************/

/**
 * An alias type for request handlers
 */
typedef response *route_handler(request *, response *);

/**
 * A router / HTTP multiplexer
 */
typedef struct router_internal *http_router;

/**
 * An attributes object for configuring a http_router
 */
typedef struct router_attr_internal router_attr;

/**
 * Initialize a router attributes object
 *
 * @return router_attr*
 */
router_attr *router_attr_init();

/**
 * router_init allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be
 * used).
 * @param router_attr* Router attributes to apply to the new http_router
 * instance
 * @return http_router*
 */
http_router *router_init(router_attr *attr);

/**
 * router_register registers a new route record. Registered routes will be
 * matched against when used with `router_run`
 *
 * @param router The router instance in which to register the route
 * @param path The path to associate with the route
 * @param handler The handler to associate with the route
 * @param method
 * @param ... Methods to associate with the route. You must pass a NULL sentinel
 * to indicate the end of the list e.g.
 * `router_register(...args, METHOD_GET, METHOD_POST, NULL)`
 */
void router_register(http_router *router, const char *path,
                     route_handler *handler, http_method method, ...);

/**
 * router_free deallocates memory for http_router `router`
 *
 * @param router
 */
void router_free(http_router *router);

// TODO: t + d
http_router *router_register_sub(http_router *parent_router, router_attr *attr,
                                 const char *subpath);

/**********************************************************
 * Server
 **********************************************************/

/**
 * A server configuration object that stores settings for the HTTP server
 */
typedef struct server_internal *tcp_server;

/**
 * server_init allocates the necessary memory for a `tcp_server`
 *
 * @param router The router instance to run on new client connections
 * @param port The port on which the server will listen for incoming connections
 */
tcp_server *server_init(http_router *router, int port);

/**
 * server_start listens for client connections and executes routing
 *
 * @param server The server instance on which to start listening
 */
void server_start(tcp_server *server);

/**
 * server_set_cert sets the cert and key filepaths for use with TLS.
 * This has no effect whatsoever unless you've compiled with the `USE_TLS` flag
 *
 * @param server
 * @param certfile
 * @param keyfile
 */
void server_set_cert(tcp_server *server, char *certfile, char *keyfile);

/**
 * Deallocates a tcp_server's heap memory
 *
 * @param server
 */
void server_free(tcp_server *server);

/**********************************************************
 * Middleware
 **********************************************************/

/**
 * middlewares binds n middleware handlers to the router attributes instance.
 * You do not need to pass a NULL sentinel to terminate the list; this macro
 * will do this for you.
 *
 * @param router_attr*
 * @param route_handler*[]
 */
#define middlewares(attr, ...) __middlewares(attr, __VA_ARGS__, NULL)

/**
 * @internal
 */
void __middlewares(router_attr *attr, route_handler *mw, ...);

/**
 * add_middleware binds a new middleware to the routes attributes object.
 * Middlewares will be run in a LIFO fashion.
 *
 * @param attr
 * @param mw
 */
void add_middleware(router_attr *attr, route_handler *mw);

/**********************************************************
 * CORS
 **********************************************************/

/**
 * CORS configuration options
 */
typedef struct cors_opts_internal cors_opts;

/**
 * cors_opts_init initializes a new CORS options object
 *
 * @return cors_opts*
 */
cors_opts *cors_opts_init();

/**
 * use_cors binds the CORS global middleware to the router attributes instance
 *
 * @param attr
 * @param opts
 */
void use_cors(router_attr *attr, cors_opts *opts);

/**
 * set_allowed_origins sets allowed origins on given the cors_opts. You do not
 * need to pass a NULL sentinel to terminate the list; this macro will do this
 * for you.
 *
 * @param opts cors_opts*
 * @param ... char*[]
 */
#define set_allowed_origins(opts, ...) \
  __set_allowed_origins(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_origins(cors_opts *opts, array_t *origins);

/**
 * set_allowed_methods sets the allowed methods on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 *
 * @param opts cors_opts*
 * @param ... http_method*[]
 */
#define set_allowed_methods(opts, ...) \
  __set_allowed_methods(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_methods(cors_opts *opts, array_t *methods);

/**
 * set_allowed_headers sets the allowed headers on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 *
 * @param opts cors_opts*
 * @param ... char*[]
 */
#define set_allowed_headers(opts, ...) \
  __set_allowed_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_headers(cors_opts *opts, array_t *headers);

/**
 * set_expose_headers sets the expose headers on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 *
 * @param opts cors_opts*
 * @param ... char*[]
 */
#define set_expose_headers(opts, ...) \
  __set_expose_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_expose_headers(cors_opts *opts, array_t *headers);

/**
 * set_allow_credentials sets the cors_opts allowed_credentials option
 *
 * @param opts
 * @param allow
 */
void set_allow_credentials(cors_opts *opts, bool allow);

/**
 * set_use_options_passthrough sets the cors_opts use_options_passthrough
 * option
 *
 * @param opts
 * @param use
 */
void set_use_options_passthrough(cors_opts *opts, bool use);

/**
 * set_max_age sets the cors_opts max age option
 *
 * @param opts
 * @param max_age an int, interpreted as seconds
 */
void set_max_age(cors_opts *opts, int max_age);

/**
 * cors_allow_all initializes a new CORS options object with sensible, lax
 * defaults
 *
 * @return cors_opts*
 */
cors_opts *cors_allow_all();

/**********************************************************
 * Cookies
 **********************************************************/

typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} same_site_mode;

typedef struct cookie_internal *cookie;

/**
 * Initialize a new Cookie with a given name and value. All other fields will be
 * initialized to NULL or field-equivalent values such that the field is
 * ignored. Use the cookie_set_* helpers to set the other attributes.
 *
 * @param name
 * @param value
 * @return cookie*
 */
cookie *cookie_init(const char *name, const char *value);

/**
 * Set the given Cookie's domain attribute
 *
 * @param c
 * @param domain
 */
void cookie_set_domain(cookie *c, const char *domain);

/**
 * Set the given Cookie's expires attribute
 *
 * @param c
 * @param when
 */
void cookie_set_expires(cookie *c, time_t when);

/**
 * Set the given Cookie's http_only attribute
 *
 * @param c
 * @param http_only
 */
void cookie_set_http_only(cookie *c, bool http_only);

/**
 * Set the given Cookie's age attribute
 *
 * @param c
 * @param age
 */
void cookie_set_max_age(cookie *c, int age);

/**
 * Set the given Cookie's path attribute
 *
 * @param c
 * @param path
 */
void cookie_set_path(cookie *c, const char *path);

/**
 * Set the given Cookie's same_site attribute
 *
 * @param c
 * @param mode
 */
void cookie_set_same_site(cookie *c, same_site_mode mode);

/**
 * Set the given Cookie's secure attribute
 *
 * @param c
 * @param secure
 */
void cookie_set_secure(cookie *c, bool secure);

/**
 * get_cookie returns the named cookie provided in the request. If multiple
 * cookies match the given name, only the first matched cookie will be returned.
 *
 * @param req
 * @param name
 * @return cookie*
 */
cookie *get_cookie(request *req, const char *name);

/**
 * set_cookie adds a Set-Cookie header to the provided response's headers.
 * The provided cookie must have a valid Name. Invalid cookies may be silently
 * omitted.
 * @param res
 * @param c
 */
void set_cookie(response *res, cookie *c);

char *cookie_get_name(cookie *c);

char *cookie_get_value(cookie *c);

char *cookie_get_domain(cookie *c);

time_t cookie_get_expires(cookie *c);

bool cookie_get_http_only(cookie *c);

int cookie_get_max_age(cookie *c);

char *cookie_get_path(cookie *c);

same_site_mode cookie_get_same_site(cookie *c);

bool cookie_get_secure(cookie *c);

void cookie_free(cookie *c);

/**********************************************************
 * Utilities
 **********************************************************/

/* Time Helpers */
time_t n_minutes_from_now(unsigned int n);
time_t n_hours_from_now(unsigned int n);
time_t n_days_from_now(unsigned int n);

#endif /* LIBHTTP_H */
