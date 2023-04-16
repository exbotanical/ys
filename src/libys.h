#ifndef LIB_YS_H
#define LIB_YS_H

#include <stdbool.h>
#include <time.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"

/**********************************************************
 * Constants
 **********************************************************/

extern const char* MIME_TYPE_3GP;
extern const char* MIME_TYPE_3GPP;
extern const char* MIME_TYPE_7Z;
extern const char* MIME_TYPE_AI;
extern const char* MIME_TYPE_APNG;
extern const char* MIME_TYPE_ASF;
extern const char* MIME_TYPE_ASX;
extern const char* MIME_TYPE_ATOM;
extern const char* MIME_TYPE_AVI;
extern const char* MIME_TYPE_AVIF;
extern const char* MIME_TYPE_BIN;
extern const char* MIME_TYPE_BMP;
extern const char* MIME_TYPE_CCO;
extern const char* MIME_TYPE_CRT;
extern const char* MIME_TYPE_CSS;
extern const char* MIME_TYPE_CSV;
extern const char* MIME_TYPE_CUR;
extern const char* MIME_TYPE_DEB;
extern const char* MIME_TYPE_DER;
extern const char* MIME_TYPE_DLL;
extern const char* MIME_TYPE_Dmg;
extern const char* MIME_TYPE_DOC;
extern const char* MIME_TYPE_DOCX;
extern const char* MIME_TYPE_EAR;
extern const char* MIME_TYPE_EOT;
extern const char* MIME_TYPE_EPS;
extern const char* MIME_TYPE_EPUB;
extern const char* MIME_TYPE_EXE;
extern const char* MIME_TYPE_FLV;
extern const char* MIME_TYPE_GEOJSON;
extern const char* MIME_TYPE_GIF;
extern const char* MIME_TYPE_GLB;
extern const char* MIME_TYPE_GLTF;
extern const char* MIME_TYPE_HQX;
extern const char* MIME_TYPE_HTC;
extern const char* MIME_TYPE_HTM;
extern const char* MIME_TYPE_HTML;
extern const char* MIME_TYPE_ICO;
extern const char* MIME_TYPE_IMG;
extern const char* MIME_TYPE_ISO;
extern const char* MIME_TYPE_JAD;
extern const char* MIME_TYPE_JAR;
extern const char* MIME_TYPE_JARDIFF;
extern const char* MIME_TYPE_JNG;
extern const char* MIME_TYPE_JNLP;
extern const char* MIME_TYPE_JP2;
extern const char* MIME_TYPE_JPEG;
extern const char* MIME_TYPE_JPG;
extern const char* MIME_TYPE_JS;
extern const char* MIME_TYPE_JSON;
extern const char* MIME_TYPE_JSONLD;
extern const char* MIME_TYPE_JXL;
extern const char* MIME_TYPE_JXR;
extern const char* MIME_TYPE_KAR;
extern const char* MIME_TYPE_KML;
extern const char* MIME_TYPE_KMZ;
extern const char* MIME_TYPE_M3U8;
extern const char* MIME_TYPE_M4A;
extern const char* MIME_TYPE_M4V;
extern const char* MIME_TYPE_MD;
extern const char* MIME_TYPE_MID;
extern const char* MIME_TYPE_MIDI;
extern const char* MIME_TYPE_MJS;
extern const char* MIME_TYPE_MML;
extern const char* MIME_TYPE_MNG;
extern const char* MIME_TYPE_MOV;
extern const char* MIME_TYPE_MP3;
extern const char* MIME_TYPE_MP4;
extern const char* MIME_TYPE_MPEG;
extern const char* MIME_TYPE_MPG;
extern const char* MIME_TYPE_MSI;
extern const char* MIME_TYPE_MSM;
extern const char* MIME_TYPE_MSP;
extern const char* MIME_TYPE_OGA;
extern const char* MIME_TYPE_OGG;
extern const char* MIME_TYPE_OGV;
extern const char* MIME_TYPE_OGX;
extern const char* MIME_TYPE_OPUS;
extern const char* MIME_TYPE_OTC;
extern const char* MIME_TYPE_OTF;
extern const char* MIME_TYPE_PDB;
extern const char* MIME_TYPE_PDF;
extern const char* MIME_TYPE_PEM;
extern const char* MIME_TYPE_PL;
extern const char* MIME_TYPE_PM;
extern const char* MIME_TYPE_PNG;
extern const char* MIME_TYPE_PPT;
extern const char* MIME_TYPE_PPTX;
extern const char* MIME_TYPE_PRC;
extern const char* MIME_TYPE_PS;
extern const char* MIME_TYPE_RA;
extern const char* MIME_TYPE_RAR;
extern const char* MIME_TYPE_RDF;
extern const char* MIME_TYPE_RPM;
extern const char* MIME_TYPE_RSS;
extern const char* MIME_TYPE_RTF;
extern const char* MIME_TYPE_RUN;
extern const char* MIME_TYPE_SEA;
extern const char* MIME_TYPE_SHTML;
extern const char* MIME_TYPE_SIT;
extern const char* MIME_TYPE_SPX;
extern const char* MIME_TYPE_SVG;
extern const char* MIME_TYPE_SVGZ;
extern const char* MIME_TYPE_SWF;
extern const char* MIME_TYPE_TCL;
extern const char* MIME_TYPE_TIF;
extern const char* MIME_TYPE_TIFF;
extern const char* MIME_TYPE_TK;
extern const char* MIME_TYPE_TS;
extern const char* MIME_TYPE_TTC;
extern const char* MIME_TYPE_TTF;
extern const char* MIME_TYPE_TTL;
extern const char* MIME_TYPE_TXT;
extern const char* MIME_TYPE_UDEB;
extern const char* MIME_TYPE_USDZ;
extern const char* MIME_TYPE_VTT;
extern const char* MIME_TYPE_WAR;
extern const char* MIME_TYPE_WASM;
extern const char* MIME_TYPE_WBMP;
extern const char* MIME_TYPE_WEBM;
extern const char* MIME_TYPE_WEBP;
extern const char* MIME_TYPE_WML;
extern const char* MIME_TYPE_WMLC;
extern const char* MIME_TYPE_WMV;
extern const char* MIME_TYPE_WOFF;
extern const char* MIME_TYPE_WOFF2;
extern const char* MIME_TYPE_XHT;
extern const char* MIME_TYPE_XHTML;
extern const char* MIME_TYPE_XLS;
extern const char* MIME_TYPE_XLSX;
extern const char* MIME_TYPE_XML;
extern const char* MIME_TYPE_XPI;
extern const char* MIME_TYPE_XSPF;
extern const char* MIME_TYPE_ZIP;
extern const char* MIME_TYPE_ZST;

/**
 * Mappings of HTTP status enums to string literal names
 */
extern const char* http_status_names[];

/**
 * Mappings of HTTP method enums to string literal names
 */
extern const char* http_method_names[];

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
typedef struct request_internal* request;

/**
 * req_get_parameter retrieves the parameter value matching `key`, or NULL
 * if not extant
 */
char* req_get_parameter(request* req, const char* key);

/**
 * req_num_parameters returns the number of parameters matched on the given
 * request
 */
unsigned int req_num_parameters(request* req);

/**
 * req_has_parameters returns a bool indicating whether the given request has
 * any parameters
 */
bool req_has_parameters(request* req);

/**
 * req_get_query gets the list of values for a given query key. Returns a list
 * of values, or NULL if no match For example, if the request URL was
 * some-url/?key=1&key=2&key=3, calling req_get_query(req, "key") would yield
 * ["1", "2", "3"]. Use `req_num_queries` to retrieve the size of this list.
 */
char** req_get_query(request* req, const char* key);

/**
 * req_has_query tests whether the request has a query match for `key`
 */
bool req_has_query(request* req, const char* key);

/**
 * req_num_queries returns the number of values associated with a given key
 * `key`
 */
unsigned int req_num_queries(request* req, const char* key);

/**
 * req_get_path returns the full request path
 */
char* req_get_path(request* req);

/**
 * req_get_route_path returns the request path segment that was matched on
 * the router. For sub-routers, this will be the sub-path
 * e.g. path = /api/demo, route_path = /demo
 */
char* req_get_route_path(request* req);

/**
 * req_get_method returns the request method
 */
char* req_get_method(request* req);

/**
 * req_get_body returns the full request body
 */
char* req_get_body(request* req);

/**
 * req_get_raw returns the entire, raw request as it was received by the
 * server
 */
char* req_get_raw(request* req);

/**
 * req_get_version returns the protocol version string included in the
 * request
 */
char* req_get_version(request* req);

/**
 * req_get_header retrieves the first value for a given header key on
 * the request or NULL if not found
 */
char* req_get_header(request* req, const char* key);

/**********************************************************
 * Response
 **********************************************************/

/**
 * A response holds the necessary metadata for the response that will be
 * sent to the client. Client handlers should use the library-provided
 * helpers to set the desired properties e.g. status, body, and headers.
 */
typedef struct response_internal* response;

/**
 * set_header inserts the given key/value pair as a header on the response.
 * Returns true if adding the header succeeded; false if adding the header
 * failed (this would indicate an OOM error).
 */
bool set_header(response* res, const char* key, const char* value);

/**
 * set_body sets the given body on the response
 */
void set_body(response* res, const char* body);

/**
 * set_status sets the given status code on the response
 */
void set_status(response* res, http_status status);

/**
 * get_done returns the done status of the response
 */
bool get_done(response* res);

/**
 * set_done sets the done status of the response to true. This operation is
 * idempotent
 */
void set_done(response* res);

/**
 * get_header retrieves the first header value for a given key on
 * the response or NULL if not found
 */
char* get_header(response* res, const char* key);

/**
 * from_file reads a file into a string buffer, which may then be passed
 * directly to `set_body` e.g. set_body(res, from_file("./index.html"));
 */
char* from_file(const char* filename);

/**********************************************************
 * Router
 **********************************************************/

/**
 * An alias type for request handlers
 */
typedef response* route_handler(request*, response*);

/**
 * A router / HTTP multiplexer
 */
typedef struct router_internal* http_router;

/**
 * An attributes object for configuring a http_router
 */
typedef struct router_attr_internal router_attr;

/**
 * router_attr_init initializes a new router attributes object
 */
router_attr* router_attr_init(void);

/**
 * router_init allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be
 * used).
 */
http_router* router_init(router_attr* attr);

/**
 * router_register registers a new route record. Registered routes will be
 * matched against when used with `router_run`. Last argument is methods to
 * associate with the route.
 */
#define router_register(router, path, handler, ...) \
  __router_register(router, path, handler, __VA_ARGS__, NULL)

void __router_register(http_router* router, const char* path,
                       route_handler* handler, http_method method, ...);

/**
 * router_free deallocates memory for http_router `router`
 */
void router_free(http_router* router);

/**
 * router_register_sub registers a sub-router for routing requests based off of
 * a specific path.
 * TODO: t
 */
http_router* router_register_sub(http_router* parent_router, router_attr* attr,
                                 const char* subpath);

/**
 * router_register_404_handler registers the handler to be used for handling
 * requests to a non-registered route. If you do not set a status in this
 * handler, it will be defaulted to 404.
 */
void router_register_404_handler(router_attr* attr, route_handler* h);

/**
 * router_register_405_handler registers the handler to be used for handling
 * requests to a non-registered method for a registered path. If you do not set
 * a status in this handler, it will be defaulted to 405.
 */
void router_register_405_handler(router_attr* attr, route_handler* h);

/**
 * router_register_500_handler registers the handler to be used for handling
 * erroneous requests. If you do not set a status in this handler, it will be
 * defaulted to 500.
 */
void router_register_500_handler(router_attr* attr, route_handler* h);

/**********************************************************
 * Server
 **********************************************************/

/**
 * A server attributes object that stores settings for the tcp_server
 */
typedef struct server_attr_internal* tcp_server_attr;

/**
 * A server object that represents the HTTP server
 */
typedef struct server_internal* tcp_server;

/**
 * server_attr_init initializes a new server attributes object
 */
tcp_server_attr* server_attr_init(http_router* router);

/**
 * server_attr_init_with aggregates all of the server_attr_* methods into a
 * single, convenient helper function. If the cert or key filepaths are NULL,
 * `use_https` will be false.
 */
tcp_server_attr* server_attr_init_with(http_router* router, int port,
                                       char* cert_path, char* key_path);

/**
 * server_set_port sets a non-default port on the server
 */
void server_set_port(tcp_server_attr* attr, int port);

/**
 * server_use_https instructs Ys to use SSL for the server
 */
void server_use_https(tcp_server_attr* attr, char* cert_path, char* key_path);

/**
 * server_disable_https disables HTTPs support. This is a convenience function
 * typically used for testing, where cert files were set on the tcp_server_attr*
 * (meaning HTTPs is enabled), but you still do not want to use HTTPs
 */
void server_disable_https(tcp_server_attr* attr);

/**
 * server_init allocates the necessary memory for a `tcp_server`
 */
tcp_server* server_init(tcp_server_attr*);

/**
 * server_start listens for client connections and executes routing
 */
void server_start(tcp_server* server);

/**
 * server_free deallocates memory for the provided tcp_server* instance
 */
void server_free(tcp_server* server);

/**********************************************************
 * Middleware
 **********************************************************/

/**
 * use_middlewares binds n middleware handlers to the router attributes
 * instance. You do not need to pass a NULL sentinel to terminate the list; this
 * macro will do this for you.
 */
#define use_middlewares(attr, ...) __middlewares(attr, __VA_ARGS__, NULL)

/**
 * @internal
 */
void __middlewares(router_attr* attr, route_handler* mw, ...);

/**
 * add_middleware_with_opts binds a new middleware - along with ignore
 * paths - to the routes attributes object. When handling a request, if the
 * request path matches one of the ignore paths for a middleware, that
 * middleware will not be invoked. Think of ignore paths as a disallow-list.
 * ignore_paths is a list of ignore paths. Does not need to be
 * sentinel-terminated.
 *
 * Ignore paths are compared in full, even for sub-routers. If the request is
 * /api/path and a sub-router matches on /api (making the route_path /path),
 * /api/path is compared to the ignore paths.
 *
 * @example
 * ignore_paths = [/internal, /internal/app, /private/request]
 *
 * request->path = /demo -> middleware runs
 * request->path = /internal -> middleware does not run
 * request->path = /internal/app -> middleware does not run
 * request->path = /internal/request -> middleware runs
 * request->path = /private -> middleware runs
 * request->path = /private/request -> middleware does not run
 */
#define add_middleware_with_opts(attr, mw, ...) \
  __add_middleware_with_opts(attr, mw, __VA_ARGS__, NULL)

/**
 * @internal
 */
void __add_middleware_with_opts(router_attr* attr, route_handler* mw,
                                char* ignore_path, ...);

/**
 * use_middleware binds a new middleware to the routes attributes object.
 * Middlewares will be run in a LIFO fashion.
 */
void use_middleware(router_attr* attr, route_handler* mw);

/**********************************************************
 * CORS
 **********************************************************/

/**
 * CORS configuration options
 */
typedef struct cors_opts_internal cors_opts;

/**
 * cors_opts_init initializes a new CORS options object
 */
cors_opts* cors_opts_init(void);

/**
 * use_cors binds the CORS global middleware to the router attributes instance
 */
void use_cors(router_attr* attr, cors_opts* opts);

/**
 * cors_allow_origins sets allowed origins on given the cors_opts. You do not
 * need to pass a NULL sentinel to terminate the list; this macro will do this
 * for you.
 */
#define cors_allow_origins(opts, ...) \
  __set_allowed_origins(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_origins(cors_opts* opts, array_t* origins);

/**
 * cors_allow_methods sets the allowed methods on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define cors_allow_methods(opts, ...) \
  __set_allowed_methods(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_methods(cors_opts* opts, array_t* methods);

/**
 * cors_allow_headers sets the allowed headers on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define cors_allow_headers(opts, ...) \
  __set_allowed_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_headers(cors_opts* opts, array_t* headers);

/**
 * cors_expose_headers sets the expose headers on the given cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define cors_expose_headers(opts, ...) \
  __set_expose_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_expose_headers(cors_opts* opts, array_t* headers);

/**
 * cors_allow_credentials sets the cors_opts allowed_credentials option
 */
void cors_allow_credentials(cors_opts* opts, bool allow);

/**
 * cors_use_options_passthrough sets the cors_opts use_options_passthrough
 * option
 */
void cors_use_options_passthrough(cors_opts* opts, bool use);

/**
 * cors_set_max_age sets the cors_opts max age option
 */
void cors_set_max_age(cors_opts* opts, int max_age);

/**
 * cors_allow_all initializes a new CORS options object with sensible, lax
 * defaults
 */
cors_opts* cors_allow_all(void);

/**********************************************************
 * Cookies
 **********************************************************/

typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} same_site_mode;

typedef struct cookie_internal* cookie;

/**
 * cookie_init initializes a new Cookie with a given name and value. All other
 * fields will be initialized to NULL or field-equivalent values such that the
 * field is ignored. Use the cookie_set_* helpers to set the other attributes.
 */
cookie* cookie_init(const char* name, const char* value);

/**
 * cookie_set_domain sets the given Cookie's domain attribute
 */
void cookie_set_domain(cookie* c, const char* domain);

/**
 * cookie_set_expires sets the given Cookie's expires attribute
 */
void cookie_set_expires(cookie* c, time_t when);

/**
 * cookie_set_http_only sets the given Cookie's http_only attribute
 */
void cookie_set_http_only(cookie* c, bool http_only);

/**
 * cookie_set_max_age sets the given Cookie's age attribute
 */
void cookie_set_max_age(cookie* c, int age);

/**
 * cookie_set_path sets the given Cookie's path attribute
 */
void cookie_set_path(cookie* c, const char* path);

/**
 * cookie_set_same_site sets the given Cookie's same_site attribute
 */
void cookie_set_same_site(cookie* c, same_site_mode mode);

/**
 * cookie_set_secure sets the the Cookie's secure attribute to `secure`
 */
void cookie_set_secure(cookie* c, bool secure);

/**
 * get_cookie returns the named cookie provided in the request. If multiple
 * cookies match the given name, only the first matched cookie will be returned.
 */
cookie* get_cookie(request* req, const char* name);

/**
 * set_cookie adds a Set-Cookie header to the provided response's headers.
 * The provided cookie must have a valid Name. Invalid cookies may be silently
 * omitted.
 */
void set_cookie(response* res, cookie* c);

/**
 * cookie_get_name returns the cookie's name
 */
char* cookie_get_name(cookie* c);

/**
 * cookie_get_value returns the cookie's value
 */
char* cookie_get_value(cookie* c);

/**
 * cookie_get_domain returns the cookie's domain
 */
char* cookie_get_domain(cookie* c);

/**
 * cookie_get_expires returns the cookie's expiry
 */
time_t cookie_get_expires(cookie* c);

/**
 * cookie_get_http_only returns a boolean indicating whether the cookie is http
 * only
 */
bool cookie_get_http_only(cookie* c);

/**
 * cookie_get_max_age returns the value of the cookie's max_age property
 */
int cookie_get_max_age(cookie* c);

/**
 * cookie_get_path returns the value of the cookie's path property
 */
char* cookie_get_path(cookie* c);

/**
 * cookie_get_same_site returns the value of the cookie's same_site property
 */
same_site_mode cookie_get_same_site(cookie* c);

/**
 * cookie_get_secure returns the value of the cookie's secure property
 */
bool cookie_get_secure(cookie* c);

/**
 * cookie_free frees all cookie* heap memory
 */
void cookie_free(cookie* c);

/**********************************************************
 * Utilities
 **********************************************************/

/* Time Helpers */
time_t n_minutes_from_now(unsigned int n);
time_t n_hours_from_now(unsigned int n);
time_t n_days_from_now(unsigned int n);

#endif /* LIB_YS_H */
