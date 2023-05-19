#ifndef LIB_YS_H
#define LIB_YS_H

#include <stdbool.h>
#include <time.h>

#include "libhash/libhash.h"
#include "libutil/libutil.h"

/**********************************************************
 * Constants
 **********************************************************/

extern const char* YS_MIME_TYPE_3GP;
extern const char* YS_MIME_TYPE_3GPP;
extern const char* YS_MIME_TYPE_7Z;
extern const char* YS_MIME_TYPE_AI;
extern const char* YS_MIME_TYPE_APNG;
extern const char* YS_MIME_TYPE_ASF;
extern const char* YS_MIME_TYPE_ASX;
extern const char* YS_MIME_TYPE_ATOM;
extern const char* YS_MIME_TYPE_AVI;
extern const char* YS_MIME_TYPE_AVIF;
extern const char* YS_MIME_TYPE_BIN;
extern const char* YS_MIME_TYPE_BMP;
extern const char* YS_MIME_TYPE_CCO;
extern const char* YS_MIME_TYPE_CRT;
extern const char* YS_MIME_TYPE_CSS;
extern const char* YS_MIME_TYPE_CSV;
extern const char* YS_MIME_TYPE_CUR;
extern const char* YS_MIME_TYPE_DEB;
extern const char* YS_MIME_TYPE_DER;
extern const char* YS_MIME_TYPE_DLL;
extern const char* YS_MIME_TYPE_Dmg;
extern const char* YS_MIME_TYPE_DOC;
extern const char* YS_MIME_TYPE_DOCX;
extern const char* YS_MIME_TYPE_EAR;
extern const char* YS_MIME_TYPE_EOT;
extern const char* YS_MIME_TYPE_EPS;
extern const char* YS_MIME_TYPE_EPUB;
extern const char* YS_MIME_TYPE_EXE;
extern const char* YS_MIME_TYPE_FLV;
extern const char* YS_MIME_TYPE_GEOJSON;
extern const char* YS_MIME_TYPE_GIF;
extern const char* YS_MIME_TYPE_GLB;
extern const char* YS_MIME_TYPE_GLTF;
extern const char* YS_MIME_TYPE_HQX;
extern const char* YS_MIME_TYPE_HTC;
extern const char* YS_MIME_TYPE_HTM;
extern const char* YS_MIME_TYPE_HTML;
extern const char* YS_MIME_TYPE_ICO;
extern const char* YS_MIME_TYPE_IMG;
extern const char* YS_MIME_TYPE_ISO;
extern const char* YS_MIME_TYPE_JAD;
extern const char* YS_MIME_TYPE_JAR;
extern const char* YS_MIME_TYPE_JARDIFF;
extern const char* YS_MIME_TYPE_JNG;
extern const char* YS_MIME_TYPE_JNLP;
extern const char* YS_MIME_TYPE_JP2;
extern const char* YS_MIME_TYPE_JPEG;
extern const char* YS_MIME_TYPE_JPG;
extern const char* YS_MIME_TYPE_JS;
extern const char* YS_MIME_TYPE_JSON;
extern const char* YS_MIME_TYPE_JSONLD;
extern const char* YS_MIME_TYPE_JXL;
extern const char* YS_MIME_TYPE_JXR;
extern const char* YS_MIME_TYPE_KAR;
extern const char* YS_MIME_TYPE_KML;
extern const char* YS_MIME_TYPE_KMZ;
extern const char* YS_MIME_TYPE_M3U8;
extern const char* YS_MIME_TYPE_M4A;
extern const char* YS_MIME_TYPE_M4V;
extern const char* YS_MIME_TYPE_MD;
extern const char* YS_MIME_TYPE_MID;
extern const char* YS_MIME_TYPE_MIDI;
extern const char* YS_MIME_TYPE_MJS;
extern const char* YS_MIME_TYPE_MML;
extern const char* YS_MIME_TYPE_MNG;
extern const char* YS_MIME_TYPE_MOV;
extern const char* YS_MIME_TYPE_MP3;
extern const char* YS_MIME_TYPE_MP4;
extern const char* YS_MIME_TYPE_MPEG;
extern const char* YS_MIME_TYPE_MPG;
extern const char* YS_MIME_TYPE_MSI;
extern const char* YS_MIME_TYPE_MSM;
extern const char* YS_MIME_TYPE_MSP;
extern const char* YS_MIME_TYPE_OGA;
extern const char* YS_MIME_TYPE_OGG;
extern const char* YS_MIME_TYPE_OGV;
extern const char* YS_MIME_TYPE_OGX;
extern const char* YS_MIME_TYPE_OPUS;
extern const char* YS_MIME_TYPE_OTC;
extern const char* YS_MIME_TYPE_OTF;
extern const char* YS_MIME_TYPE_PDB;
extern const char* YS_MIME_TYPE_PDF;
extern const char* YS_MIME_TYPE_PEM;
extern const char* YS_MIME_TYPE_PL;
extern const char* YS_MIME_TYPE_PM;
extern const char* YS_MIME_TYPE_PNG;
extern const char* YS_MIME_TYPE_PPT;
extern const char* YS_MIME_TYPE_PPTX;
extern const char* YS_MIME_TYPE_PRC;
extern const char* YS_MIME_TYPE_PS;
extern const char* YS_MIME_TYPE_RA;
extern const char* YS_MIME_TYPE_RAR;
extern const char* YS_MIME_TYPE_RDF;
extern const char* YS_MIME_TYPE_RPM;
extern const char* YS_MIME_TYPE_RSS;
extern const char* YS_MIME_TYPE_RTF;
extern const char* YS_MIME_TYPE_RUN;
extern const char* YS_MIME_TYPE_SEA;
extern const char* YS_MIME_TYPE_SHTML;
extern const char* YS_MIME_TYPE_SIT;
extern const char* YS_MIME_TYPE_SPX;
extern const char* YS_MIME_TYPE_SVG;
extern const char* YS_MIME_TYPE_SVGZ;
extern const char* YS_MIME_TYPE_SWF;
extern const char* YS_MIME_TYPE_TCL;
extern const char* YS_MIME_TYPE_TIF;
extern const char* YS_MIME_TYPE_TIFF;
extern const char* YS_MIME_TYPE_TK;
extern const char* YS_MIME_TYPE_TS;
extern const char* YS_MIME_TYPE_TTC;
extern const char* YS_MIME_TYPE_TTF;
extern const char* YS_MIME_TYPE_TTL;
extern const char* YS_MIME_TYPE_TXT;
extern const char* YS_MIME_TYPE_UDEB;
extern const char* YS_MIME_TYPE_USDZ;
extern const char* YS_MIME_TYPE_VTT;
extern const char* YS_MIME_TYPE_WAR;
extern const char* YS_MIME_TYPE_WASM;
extern const char* YS_MIME_TYPE_WBMP;
extern const char* YS_MIME_TYPE_WEBM;
extern const char* YS_MIME_TYPE_WEBP;
extern const char* YS_MIME_TYPE_WML;
extern const char* YS_MIME_TYPE_WMLC;
extern const char* YS_MIME_TYPE_WMV;
extern const char* YS_MIME_TYPE_WOFF;
extern const char* YS_MIME_TYPE_WOFF2;
extern const char* YS_MIME_TYPE_XHT;
extern const char* YS_MIME_TYPE_XHTML;
extern const char* YS_MIME_TYPE_XLS;
extern const char* YS_MIME_TYPE_XLSX;
extern const char* YS_MIME_TYPE_XML;
extern const char* YS_MIME_TYPE_XPI;
extern const char* YS_MIME_TYPE_XSPF;
extern const char* YS_MIME_TYPE_ZIP;
extern const char* YS_MIME_TYPE_ZST;

/**
 * Mappings of HTTP status enums to string literal names
 */
extern const char* ys_http_status_names[];

/**
 * Mappings of HTTP method enums to string literal names
 */
extern const char* ys_http_method_names[];

/**
 * All possible HTTP methods
 */
typedef enum {
  // MUST start with 1 for varargs handling e.g. collect
  YS_METHOD_GET = 1,
  YS_METHOD_POST,
  YS_METHOD_PUT,
  YS_METHOD_PATCH,  // RFC 5789
  YS_METHOD_DELETE,
  YS_METHOD_OPTIONS,
  YS_METHOD_HEAD,
  YS_METHOD_CONNECT,
  YS_METHOD_TRACE
} ys_http_method;

/**
 * All RFC-specified HTTP status codes
 */
typedef enum {
  YS_STATUS_CONTINUE = 100,             // RFC 7231, 6.2.1
  YS_STATUS_SWITCHING_PROTOCOLS = 101,  // RFC 7231, 6.2.2
  YS_STATUS_PROCESSING = 102,           // RFC 2518, 10.1
  YS_STATUS_EARLY_HINTS = 103,          // RFC 8297

  YS_STATUS_OK = 200,                      // RFC 7231, 6.3.1
  YS_STATUS_CREATED = 201,                 // RFC 7231, 6.3.2
  YS_STATUS_ACCEPTED = 202,                // RFC 7231, 6.3.3
  YS_STATUS_NON_AUTHORITATIVE_INFO = 203,  // RFC 7231, 6.3.4
  YS_STATUS_NO_CONTENT = 204,              // RFC 7231, 6.3.5
  YS_STATUS_RESET_CONTENT = 205,           // RFC 7231, 6.3.6
  YS_STATUS_PARTIAL_CONTENT = 206,         // RFC 7233, 4.1
  YS_STATUS_MULTISTATUS = 207,             // RFC 4918, 11.1
  YS_STATUS_ALREADY_REPORTED = 208,        // RFC 5842, 7.1
  YS_STATUS_IM_USED = 226,                 // RFC 3229, 10.4.1

  YS_STATUS_MULTIPLE_CHOICES = 300,    // RFC 7231, 6.4.1
  YS_STATUS_MOVED_PERMANENTLY = 301,   // RFC 7231, 6.4.2
  YS_STATUS_FOUND = 302,               // RFC 7231, 6.4.3
  YS_STATUS_SEE_OTHER = 303,           // RFC 7231, 6.4.4
  YS_STATUS_NOT_MODIFIED = 304,        // RFC 7232, 4.1
  YS_STATUS_USE_PROXY = 305,           // RFC 7231, 6.4.5
  YS_STATUS_NULL_ = 306,               // RFC 7231, 6.4.6 (Unused)
  YS_STATUS_TEMPORARY_REDIRECT = 307,  // RFC 7231, 6.4.7
  YS_STATUS_PERMANENT_REDIRECT = 308,  // RFC 7538, 3

  YS_STATUS_BAD_REQUEST = 400,                      // RFC 7231, 6.5.1
  YS_STATUS_UNAUTHORIZED = 401,                     // RFC 7235, 3.1
  YS_STATUS_PAYMENT_REQUIRED = 402,                 // RFC 7231, 6.5.2
  YS_STATUS_FORBIDDEN = 403,                        // RFC 7231, 6.5.3
  YS_STATUS_NOT_FOUND = 404,                        // RFC 7231, 6.5.4
  YS_STATUS_YS_METHOD_NOT_ALLOWED = 405,            // RFC 7231, 6.5.5
  YS_STATUS_NOT_ACCEPTABLE = 406,                   // RFC 7231, 6.5.6
  YS_STATUS_PROXY_AUTH_REQUIRED = 407,              // RFC 7235, 3.2
  YS_STATUS_REQUEST_TIMEOUT = 408,                  // RFC 7231, 6.5.7
  YS_STATUS_CONFLICT = 409,                         // RFC 7231, 6.5.8
  YS_STATUS_GONE = 410,                             // RFC 7231, 6.5.9
  YS_STATUS_LENGTH_REQUIRED = 411,                  // RFC 7231, 6.5.10
  YS_STATUS_PRECONDITION_FAILED = 412,              // RFC 7232, 4.2
  YS_STATUS_REQUEST_ENTITY_TOO_LARGE = 413,         // RFC 7231, 6.5.11
  YS_STATUS_REQUEST_URI_TOO_LONG = 414,             // RFC 7231, 6.5.12
  YS_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,           // RFC 7231, 6.5.13
  YS_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,  // RFC 7233, 4.4
  YS_STATUS_EXPECTATION_FAILED = 417,               // RFC 7231, 6.5.14
  YS_STATUS_TEAPOT = 418,                           // RFC 7168, 2.3.3
  YS_STATUS_MISDIRECTED_REQUEST = 421,              // RFC 7540, 9.1.2
  YS_STATUS_UNPROCESSABLE_ENTITY = 422,             // RFC 4918, 11.2
  YS_STATUS_LOCKED = 423,                           // RFC 4918, 11.3
  YS_STATUS_FAILED_DEPENDENCY = 424,                // RFC 4918, 11.4
  YS_STATUS_TOO_EARLY = 425,                        // RFC 8470, 5.2.
  YS_STATUS_UPGRADE_REQUIRED = 426,                 // RFC 7231, 6.5.15
  YS_STATUS_PRECONDITION_REQUIRED = 428,            // RFC 6585, 3
  YS_STATUS_TOO_MANY_REQUESTS = 429,                // RFC 6585, 4
  YS_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,  // RFC 6585, 5
  YS_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,    // RFC 7725, 3

  YS_STATUS_INTERNAL_SERVER_ERROR = 500,            // RFC 7231, 6.6.1
  YS_STATUS_NOT_IMPLEMENTED = 501,                  // RFC 7231, 6.6.2
  YS_STATUS_BAD_GATEWAY = 502,                      // RFC 7231, 6.6.3
  YS_STATUS_SERVICE_UNAVAILABLE = 503,              // RFC 7231, 6.6.4
  YS_STATUS_GATEWAY_TIMEOUT = 504,                  // RFC 7231, 6.6.5
  YS_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,       // RFC 7231, 6.6.6
  YS_STATUS_VARIANT_ALSO_NEGOTIATES = 506,          // RFC 2295, 8.1
  YS_STATUS_INSUFFICIENT_STORAGE = 507,             // RFC 4918, 11.5
  YS_STATUS_LOOP_DETECTED = 508,                    // RFC 5842, 7.2
  YS_STATUS_NOT_EXTENDED = 510,                     // RFC 2774, 7
  YS_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,  // RFC 6585, 6
} ys_http_status;

/**********************************************************
 * Request
 **********************************************************/

/**
 * A ys_request contains request data such as the path, method, body, any route
 * or query parameters, etc.
 */
typedef struct request_internal* ys_request;

/**
 * ys_req_get_parameter retrieves the parameter value matching `key`, or NULL
 * if not extant
 */
char* ys_req_get_parameter(ys_request* req, const char* key);

/**
 * ys_req_num_parameters returns the number of parameters matched on the given
 * request
 */
unsigned int ys_req_num_parameters(ys_request* req);

/**
 * ys_req_has_parameters returns a bool indicating whether the given request has
 * any parameters
 */
bool ys_req_has_parameters(ys_request* req);

/**
 * ys_req_get_query gets the list of values for a given query key. Returns a
 * list of values, or NULL if no match For example, if the request URL was
 * some-url/?key=1&key=2&key=3, calling ys_req_get_query(req, "key") would yield
 * ["1", "2", "3"]. Use `ys_req_num_queries` to retrieve the size of this list.
 */
char** ys_req_get_query(ys_request* req, const char* key);

/**
 * ys_req_has_query tests whether the request has a query match for `key`
 */
bool ys_req_has_query(ys_request* req, const char* key);

/**
 * ys_req_num_queries returns the number of values associated with a given key
 * `key`
 */
unsigned int ys_req_num_queries(ys_request* req, const char* key);

/**
 * ys_req_get_path returns the full request path
 */
char* ys_req_get_path(ys_request* req);

/**
 * ys_req_get_route_path returns the request path segment that was matched on
 * the router. For sub-routers, this will be the sub-path
 * e.g. path = /api/demo, route_path = /demo
 */
char* ys_req_get_route_path(ys_request* req);

/**
 * ys_req_get_method returns the request method
 */
char* ys_req_get_method(ys_request* req);

/**
 * ys_req_get_body returns the full request body
 */
char* ys_req_get_body(ys_request* req);

/**
 * ys_req_get_raw returns the entire, raw request as it was received by the
 * server
 */
char* ys_req_get_raw(ys_request* req);

/**
 * ys_req_get_version returns the protocol version string included in the
 * request
 */
char* ys_req_get_version(ys_request* req);

/**
 * ys_req_get_header retrieves the first value for a given header key on
 * the request or NULL if not found
 */
char* ys_req_get_header(ys_request* req, const char* key);

/**********************************************************
 * Response
 **********************************************************/

/**
 * A ys_response holds the necessary metadata for the response that will be
 * sent to the client. Client handlers should use the library-provided
 * helpers to set the desired properties e.g. status, body, and headers.
 */
typedef struct response_internal* ys_response;

/**
 * ys_set_header inserts the given key/value pair as a header on the response.
 * Returns true if adding the header succeeded; false if adding the header
 * failed (this would indicate an OOM error).
 */
bool ys_set_header(ys_response* res, const char* key, const char* value);

/**
 * ys_set_body sets the given body on the response
 */
void ys_set_body(ys_response* res, const char* fmt, ...);

/**
 * ys_set_status sets the given status code on the response
 */
void ys_set_status(ys_response* res, ys_http_status status);

/**
 * ys_get_done returns the done status of the response
 */
bool ys_get_done(ys_response* res);

/**
 * ys_set_done sets the done status of the response to true. This operation is
 * idempotent
 */
void ys_set_done(ys_response* res);

/**
 * ys_get_header retrieves the first header value for a given key on
 * the response or NULL if not found
 */
char* ys_get_header(ys_response* res, const char* key);

/**
 * ys_from_file reads a file into a string buffer, which may then be passed
 * directly to `ys_set_body` e.g. ys_set_body(res,
 * ys_from_file("./index.html"));
 */
char* ys_from_file(const char* filename);

/**********************************************************
 * Router
 **********************************************************/

/**
 * An alias type for request handlers
 */
typedef ys_response* ys_route_handler(ys_request*, ys_response*);

/**
 * A router / HTTP multiplexer
 */
typedef struct router_internal* ys_router;

/**
 * An attributes object for configuring a ys_router
 */
typedef struct router_attr_internal ys_router_attr;

/**
 * ys_router_attr_init initializes a new router attributes object
 */
ys_router_attr* ys_router_attr_init(void);

/**
 * ys_router_init allocates memory for a new router and its `trie` member;
 * sets the handlers for 404 and 405 (if none provided, defaults will be
 * used).
 */
ys_router* ys_router_init(ys_router_attr* attr);

/**
 * ys_router_register registers a new route record. Registered routes will be
 * matched against when used with `router_run`. Last argument is methods to
 * associate with the route.
 */
#define ys_router_register(router, path, handler, ...) \
  __router_register(router, path, handler, __VA_ARGS__, NULL)

void __router_register(ys_router* router, const char* path,
                       ys_route_handler* handler, ys_http_method method, ...);

/**
 * ys_router_free deallocates memory for ys_router `router`
 */
void ys_router_free(ys_router* router);

/**
 * ys_router_register_sub registers a sub-router for routing requests based off
 * of a specific path.
 * TODO: t
 */
ys_router* ys_router_register_sub(ys_router* parent_router,
                                  ys_router_attr* attr, const char* subpath);

/**
 * ys_router_register_404_handler registers the handler to be used for handling
 * requests to a non-registered route. If you do not set a status in this
 * handler, it will be defaulted to 404.
 */
void ys_router_register_404_handler(ys_router_attr* attr, ys_route_handler* h);

/**
 * ys_router_register_405_handler registers the handler to be used for handling
 * requests to a non-registered method for a registered path. If you do not set
 * a status in this handler, it will be defaulted to 405.
 */
void ys_router_register_405_handler(ys_router_attr* attr, ys_route_handler* h);

/**
 * ys_router_register_500_handler registers the handler to be used for handling
 * erroneous requests. If you do not set a status in this handler, it will be
 * defaulted to 500.
 */
void ys_router_register_500_handler(ys_router_attr* attr, ys_route_handler* h);

/**********************************************************
 * Server
 **********************************************************/

/**
 * A server attributes object that stores settings for the ys_server
 */
typedef struct server_attr_internal* ys_server_attr;

/**
 * A server object that represents the HTTP server
 */
typedef struct server_internal* ys_server;

/**
 * ys_server_attr_init initializes a new server attributes object
 */
ys_server_attr* ys_server_attr_init(ys_router* router);

/**
 * ys_server_attr_init_with aggregates all of the server_attr_* methods into a
 * single, convenient helper function. If the cert or key filepaths are NULL,
 * `use_https` will be false.
 */
ys_server_attr* ys_server_attr_init_with(ys_router* router, int port,
                                         char* cert_path, char* key_path);

/**
 * ys_server_set_port sets a non-default port on the server
 */
void ys_server_set_port(ys_server_attr* attr, int port);

/**
 * ys_server_use_https instructs Ys to use SSL for the server
 */
void ys_server_use_https(ys_server_attr* attr, char* cert_path, char* key_path);

/**
 * ys_server_disable_https disables HTTPs support. This is a convenience
 * function typically used for testing, where cert files were set on the
 * ys_server_attr* (meaning HTTPs is enabled), but you still do not want
 * to use HTTPs
 */
void ys_server_disable_https(ys_server_attr* attr);

/**
 * ys_server_init allocates the necessary memory for a `ys_server`
 */
ys_server* ys_server_init(ys_server_attr*);

/**
 * ys_server_start listens for client connections and executes routing
 */
void ys_server_start(ys_server* server);

/**
 * ys_server_free deallocates memory for the provided ys_server* instance
 */
void ys_server_free(ys_server* server);

/**********************************************************
 * Middleware
 **********************************************************/

/**
 * ys_use_middlewares binds n middleware handlers to the router attributes
 * instance. You do not need to pass a NULL sentinel to terminate the list; this
 * macro will do this for you.
 */
#define ys_use_middlewares(attr, ...) __middlewares(attr, __VA_ARGS__, NULL)

/**
 * @internal
 */
void __middlewares(ys_router_attr* attr, ys_route_handler* mw, ...);

/**
 * ys_add_middleware_with_opts binds a new middleware - along with ignore
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
#define ys_add_middleware_with_opts(attr, mw, ...) \
  __add_middleware_with_opts(attr, mw, __VA_ARGS__, NULL)

/**
 * @internal
 */
void __add_middleware_with_opts(ys_router_attr* attr, ys_route_handler* mw,
                                char* ignore_path, ...);

/**
 * ys_use_middleware binds a new middleware to the routes attributes object.
 * Middlewares will be run in a LIFO fashion.
 */
void ys_use_middleware(ys_router_attr* attr, ys_route_handler* mw);

/**********************************************************
 * CORS
 **********************************************************/

/**
 * CORS configuration options
 */
typedef struct cors_opts_internal ys_cors_opts;

/**
 * ys_cors_opts_init initializes a new CORS options object
 */
ys_cors_opts* ys_cors_opts_init(void);

/**
 * ys_use_cors binds the CORS global middleware to the router attributes
 * instance
 */
void ys_use_cors(ys_router_attr* attr, ys_cors_opts* opts);

/**
 * ys_cors_allow_origins sets allowed origins on given the ys_cors_opts. You do
 * not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define ys_cors_allow_origins(opts, ...) \
  __set_allowed_origins(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_origins(ys_cors_opts* opts, array_t* origins);

/**
 * ys_cors_allow_methods sets the allowed methods on the given ys_cors_opts. You
 * do not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define ys_cors_allow_methods(opts, ...) \
  __set_allowed_methods(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_methods(ys_cors_opts* opts, array_t* methods);

/**
 * ys_cors_allow_headers sets the allowed headers on the given ys_cors_opts. You
 * do not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define ys_cors_allow_headers(opts, ...) \
  __set_allowed_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_allowed_headers(ys_cors_opts* opts, array_t* headers);

/**
 * ys_cors_expose_headers sets the expose headers on the given ys_cors_opts. You
 * do not need to pass a NULL sentinel to terminate the list; this macro will do
 * this for you.
 */
#define ys_cors_expose_headers(opts, ...) \
  __set_expose_headers(opts, array_collect(__VA_ARGS__))

/**
 * @internal
 */
void __set_expose_headers(ys_cors_opts* opts, array_t* headers);

/**
 * ys_cors_allow_credentials sets the ys_cors_opts allowed_credentials option
 */
void ys_cors_allow_credentials(ys_cors_opts* opts, bool allow);

/**
 * ys_cors_use_options_passthrough sets the ys_cors_opts use_options_passthrough
 * option
 */
void ys_cors_use_options_passthrough(ys_cors_opts* opts, bool use);

/**
 * ys_cors_set_max_age sets the ys_cors_opts max age option
 */
void ys_cors_set_max_age(ys_cors_opts* opts, int max_age);

/**
 * ys_cors_allow_all initializes a new CORS options object with sensible, lax
 * defaults
 */
ys_cors_opts* ys_cors_allow_all(void);

/**********************************************************
 * Cookies
 **********************************************************/

typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} ys_same_site_mode;

typedef struct cookie_internal* ys_cookie;

/**
 * ys_cookie_init initializes a new Cookie with a given name and value. All
 * other fields will be initialized to NULL or field-equivalent values such that
 * the field is ignored. Use the cookie_set_* helpers to set the other
 * attributes.
 */
ys_cookie* ys_cookie_init(const char* name, const char* value);

/**
 * ys_cookie_set_domain sets the given Cookie's domain attribute
 */
void ys_cookie_set_domain(ys_cookie* c, const char* domain);

/**
 * ys_cookie_set_expires sets the given Cookie's expires attribute
 */
void ys_cookie_set_expires(ys_cookie* c, time_t when);

/**
 * ys_cookie_set_http_only sets the given Cookie's http_only attribute
 */
void ys_cookie_set_http_only(ys_cookie* c, bool http_only);

/**
 * ys_cookie_set_max_age sets the given Cookie's age attribute
 */
void ys_cookie_set_max_age(ys_cookie* c, int age);

/**
 * ys_cookie_set_path sets the given Cookie's path attribute
 */
void ys_cookie_set_path(ys_cookie* c, const char* path);

/**
 * ys_cookie_set_same_site sets the given Cookie's same_site attribute
 */
void ys_cookie_set_same_site(ys_cookie* c, ys_same_site_mode mode);

/**
 * ys_cookie_set_secure sets the the Cookie's secure attribute to `secure`
 */
void ys_cookie_set_secure(ys_cookie* c, bool secure);

/**
 * ys_get_cookie returns the named cookie provided in the request. If multiple
 * cookies match the given name, only the first matched cookie will be returned.
 */
ys_cookie* ys_get_cookie(ys_request* req, const char* name);

/**
 * ys_set_cookie adds a Set-Cookie header to the provided response's headers.
 * The provided cookie must have a valid Name. Invalid cookies may be silently
 * omitted.
 */
void ys_set_cookie(ys_response* res, ys_cookie* c);

/**
 * ys_cookie_get_name returns the cookie's name
 */
char* ys_cookie_get_name(ys_cookie* c);

/**
 * ys_cookie_get_value returns the cookie's value
 */
char* ys_cookie_get_value(ys_cookie* c);

/**
 * ys_cookie_get_domain returns the cookie's domain
 */
char* ys_cookie_get_domain(ys_cookie* c);

/**
 * ys_cookie_get_expires returns the cookie's expiry
 */
time_t ys_cookie_get_expires(ys_cookie* c);

/**
 * ys_cookie_get_http_only returns a boolean indicating whether the cookie is
 * http only
 */
bool ys_cookie_get_http_only(ys_cookie* c);

/**
 * ys_cookie_get_max_age returns the value of the cookie's max_age property
 */
int ys_cookie_get_max_age(ys_cookie* c);

/**
 * ys_cookie_get_path returns the value of the cookie's path property
 */
char* ys_cookie_get_path(ys_cookie* c);

/**
 * ys_cookie_get_same_site returns the value of the cookie's same_site property
 */
ys_same_site_mode ys_cookie_get_same_site(ys_cookie* c);

/**
 * ys_cookie_get_secure returns the value of the cookie's secure property
 */
bool ys_cookie_get_secure(ys_cookie* c);

/**
 * ys_cookie_free frees all ys_cookie* heap memory
 */
void ys_cookie_free(ys_cookie* c);

/**********************************************************
 * Utilities
 **********************************************************/

/* Time Helpers */
time_t ys_n_minutes_from_now(unsigned int n);
time_t ys_n_hours_from_now(unsigned int n);
time_t ys_n_days_from_now(unsigned int n);

#endif /* LIB_YS_H */
