#include "libys.h"
#include "util.h"

#define X(e) [e] = &(#e)[3] + 7

// Mapping of HTTP status codes to stringified names
const char* ys_http_status_names[] = {
    [YS_STATUS_CONTINUE] = "Continue",
    [YS_STATUS_SWITCHING_PROTOCOLS] = "Switching Protocols",
    [YS_STATUS_PROCESSING] = "Processing",
    [YS_STATUS_EARLY_HINTS] = "Early Hints",

    [YS_STATUS_OK] = "OK",
    [YS_STATUS_CREATED] = "Created",
    [YS_STATUS_ACCEPTED] = "Accepted",
    [YS_STATUS_NON_AUTHORITATIVE_INFO] = "Non-Authoritative Information",
    [YS_STATUS_NO_CONTENT] = "No Content",
    [YS_STATUS_RESET_CONTENT] = "Reset Content",
    [YS_STATUS_PARTIAL_CONTENT] = "Partial Content",
    [YS_STATUS_MULTISTATUS] = "Multi-Status",
    [YS_STATUS_ALREADY_REPORTED] = "Already Reported",
    [YS_STATUS_IM_USED] = "IM Used",

    [YS_STATUS_MULTIPLE_CHOICES] = "Multiple Choices",
    [YS_STATUS_MOVED_PERMANENTLY] = "Moved Permanently",
    [YS_STATUS_FOUND] = "Found",
    [YS_STATUS_SEE_OTHER] = "See Other",
    [YS_STATUS_NOT_MODIFIED] = "Not Modified",
    [YS_STATUS_USE_PROXY] = "Use Proxy",
    [YS_STATUS_NULL_] = "NULL_",
    [YS_STATUS_TEMPORARY_REDIRECT] = "Temporary Redirect",
    [YS_STATUS_PERMANENT_REDIRECT] = "Permanent Redirect",

    [YS_STATUS_BAD_REQUEST] = "Bad Request",
    [YS_STATUS_UNAUTHORIZED] = "Unauthorized",
    [YS_STATUS_PAYMENT_REQUIRED] = "Payment Required",
    [YS_STATUS_FORBIDDEN] = "Forbidden",
    [YS_STATUS_NOT_FOUND] = "Not Found",
    [YS_STATUS_YS_METHOD_NOT_ALLOWED] = "Method Not Allowed",
    [YS_STATUS_NOT_ACCEPTABLE] = "Not Acceptable",
    [YS_STATUS_PROXY_AUTH_REQUIRED] = "Proxy Authentication Required",
    [YS_STATUS_REQUEST_TIMEOUT] = "Request Timeout",
    [YS_STATUS_CONFLICT] = "Conflict",
    [YS_STATUS_GONE] = "Gone",
    [YS_STATUS_LENGTH_REQUIRED] = "Length Required",
    [YS_STATUS_PRECONDITION_FAILED] = "Precondition Failed",
    [YS_STATUS_REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large",
    [YS_STATUS_REQUEST_URI_TOO_LONG] = "Request URI Too Long",
    [YS_STATUS_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type",
    [YS_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE] =
        "Requested Range Not Satisfiable",
    [YS_STATUS_EXPECTATION_FAILED] = "Expectation Failed",
    [YS_STATUS_TEAPOT] = "I'm a teapot",
    [YS_STATUS_MISDIRECTED_REQUEST] = "Misdirected Request",
    [YS_STATUS_UNPROCESSABLE_ENTITY] = "Unprocessable Entity",
    [YS_STATUS_LOCKED] = "Locked",
    [YS_STATUS_FAILED_DEPENDENCY] = "Failed Dependency",
    [YS_STATUS_TOO_EARLY] = "Too Early",
    [YS_STATUS_UPGRADE_REQUIRED] = "Upgrade Required",
    [YS_STATUS_PRECONDITION_REQUIRED] = "Precondition Required",
    [YS_STATUS_TOO_MANY_REQUESTS] = "Too Many Requests",
    [YS_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE] =
        "Request Header Fields Too Large",
    [YS_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS] = "Unavailable For Legal Reasons",

    [YS_STATUS_INTERNAL_SERVER_ERROR] = "Internal Server Error",
    [YS_STATUS_NOT_IMPLEMENTED] = "Not Implemented",
    [YS_STATUS_BAD_GATEWAY] = "Bad Gateway",
    [YS_STATUS_SERVICE_UNAVAILABLE] = "Service Unavailable",
    [YS_STATUS_GATEWAY_TIMEOUT] = "Gateway Timeout",
    [YS_STATUS_HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported",
    [YS_STATUS_VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates",
    [YS_STATUS_INSUFFICIENT_STORAGE] = "Insufficient Storage",
    [YS_STATUS_LOOP_DETECTED] = "Loop Detected",
    [YS_STATUS_NOT_EXTENDED] = "Not Extended",
    [YS_STATUS_NETWORK_AUTHENTICATION_REQUIRED] =
        "Network Authentication Required",
};

// Mapping of HTTP method name enums to stringified names
const char* ys_http_method_names[] = {
    X(YS_METHOD_GET),     X(YS_METHOD_HEAD),    X(YS_METHOD_POST),
    X(YS_METHOD_PUT),     X(YS_METHOD_PATCH),   X(YS_METHOD_DELETE),
    X(YS_METHOD_CONNECT), X(YS_METHOD_OPTIONS), X(YS_METHOD_TRACE)};

const char* YS_MIME_TYPE_3GP = "video/3gpp";
const char* YS_MIME_TYPE_3GPP = "video/3gpp";
const char* YS_MIME_TYPE_7Z = "application/x-7z-compressed";
const char* YS_MIME_TYPE_AI = "application/postscript";
const char* YS_MIME_TYPE_APNG = "image/apng";
const char* YS_MIME_TYPE_ASF = "video/x-ms-asf";
const char* YS_MIME_TYPE_ASX = "video/x-ms-asf";
const char* YS_MIME_TYPE_ATOM = "application/atom+xml";
const char* YS_MIME_TYPE_AVI = "video/x-msvideo";
const char* YS_MIME_TYPE_AVIF = "image/avif";
const char* YS_MIME_TYPE_BIN = "application/octet-stream";
const char* YS_MIME_TYPE_BMP = "image/x-ms-bmp";
const char* YS_MIME_TYPE_CCO = "application/x-cocoa";
const char* YS_MIME_TYPE_CRT = "application/x-x509-ca-cert";
const char* YS_MIME_TYPE_CSS = "text/css";
const char* YS_MIME_TYPE_CSV = "text/csv";
const char* YS_MIME_TYPE_CUR = "image/x-icon";
const char* YS_MIME_TYPE_DEB = "application/vnd.debian.binary-package";
const char* YS_MIME_TYPE_DER = "application/x-x509-ca-cert";
const char* YS_MIME_TYPE_DLL = "application/vnd.microsoft.portable-executable";
const char* YS_MIME_TYPE_Dmg = "application/octet-stream";
const char* YS_MIME_TYPE_DOC = "application/msword";
const char* YS_MIME_TYPE_DOCX =
    "application/"
    "vnd.openxmlformats-officedocument.wordprocessingml.document";
const char* YS_MIME_TYPE_EAR = "application/java-archive";
const char* YS_MIME_TYPE_EOT = "application/vnd.ms-fontobject";
const char* YS_MIME_TYPE_EPS = "application/postscript";
const char* YS_MIME_TYPE_EPUB = "application/epub+zip";
const char* YS_MIME_TYPE_EXE = "application/vnd.microsoft.portable-executable";
const char* YS_MIME_TYPE_FLV = "video/x-flv";
const char* YS_MIME_TYPE_GEOJSON = "application/geo+json";
const char* YS_MIME_TYPE_GIF = "image/gif";
const char* YS_MIME_TYPE_GLB = "model/gltf-binary";
const char* YS_MIME_TYPE_GLTF = "model/gltf+json";
const char* YS_MIME_TYPE_HQX = "application/mac-binhex40";
const char* YS_MIME_TYPE_HTC = "text/x-component";
const char* YS_MIME_TYPE_HTM = "text/html";
const char* YS_MIME_TYPE_HTML = "text/html";
const char* YS_MIME_TYPE_ICO = "image/x-icon";
const char* YS_MIME_TYPE_IMG = "application/octet-stream";
const char* YS_MIME_TYPE_ISO = "application/octet-stream";
const char* YS_MIME_TYPE_JAD = "text/vnd.sun.j2me.app-descriptor";
const char* YS_MIME_TYPE_JAR = "application/java-archive";
const char* YS_MIME_TYPE_JARDIFF = "application/x-java-archive-diff";
const char* YS_MIME_TYPE_JNG = "image/x-jng";
const char* YS_MIME_TYPE_JNLP = "application/x-java-jnlp-file";
const char* YS_MIME_TYPE_JP2 = "image/jp2";
const char* YS_MIME_TYPE_JPEG = "image/jpeg";
const char* YS_MIME_TYPE_JPG = "image/jpeg";
const char* YS_MIME_TYPE_JS = "text/javascript";
const char* YS_MIME_TYPE_JSON = "application/json";
const char* YS_MIME_TYPE_JSONLD = "application/ld+json";
const char* YS_MIME_TYPE_JXL = "image/jxl";
const char* YS_MIME_TYPE_JXR = "image/jxr";
const char* YS_MIME_TYPE_KAR = "audio/midi";
const char* YS_MIME_TYPE_KML = "application/vnd.google-earth.kml+xml";
const char* YS_MIME_TYPE_KMZ = "application/vnd.google-earth.kmz";
const char* YS_MIME_TYPE_M3U8 = "application/vnd.apple.mpegurl";
const char* YS_MIME_TYPE_M4A = "audio/x-m4a";
const char* YS_MIME_TYPE_M4V = "video/x-m4v";
const char* YS_MIME_TYPE_MD = "text/markdown";
const char* YS_MIME_TYPE_MID = "audio/midi";
const char* YS_MIME_TYPE_MIDI = "audio/midi";
const char* YS_MIME_TYPE_MJS = "text/javascript";
const char* YS_MIME_TYPE_MML = "text/mathml";
const char* YS_MIME_TYPE_MNG = "video/x-mng";
const char* YS_MIME_TYPE_MOV = "video/quicktime";
const char* YS_MIME_TYPE_MP3 = "audio/mpeg";
const char* YS_MIME_TYPE_MP4 = "video/mp4";
const char* YS_MIME_TYPE_MPEG = "video/mpeg";
const char* YS_MIME_TYPE_MPG = "video/mpeg";
const char* YS_MIME_TYPE_MSI = "application/octet-stream";
const char* YS_MIME_TYPE_MSM = "application/octet-stream";
const char* YS_MIME_TYPE_MSP = "application/octet-stream";
const char* YS_MIME_TYPE_OGA = "audio/ogg";
const char* YS_MIME_TYPE_OGG = "audio/ogg";
const char* YS_MIME_TYPE_OGV = "video/ogg";
const char* YS_MIME_TYPE_OGX = "application/ogg";
const char* YS_MIME_TYPE_OPUS = "audio/ogg";
const char* YS_MIME_TYPE_OTC = "font/collection";
const char* YS_MIME_TYPE_OTF = "font/otf";
const char* YS_MIME_TYPE_PDB = "application/x-pilot";
const char* YS_MIME_TYPE_PDF = "application/pdf";
const char* YS_MIME_TYPE_PEM = "application/x-x509-ca-cert";
const char* YS_MIME_TYPE_PL = "application/x-perl";
const char* YS_MIME_TYPE_PM = "application/x-perl";
const char* YS_MIME_TYPE_PNG = "image/png";
const char* YS_MIME_TYPE_PPT = "application/vnd.ms-powerpoint";
const char* YS_MIME_TYPE_PPTX =
    "application/"
    "vnd.openxmlformats-officedocument.presentationml.presentation";
const char* YS_MIME_TYPE_PRC = "application/x-pilot";
const char* YS_MIME_TYPE_PS = "application/postscript";
const char* YS_MIME_TYPE_RA = "audio/x-realaudio";
const char* YS_MIME_TYPE_RAR = "application/vnd.rar";
const char* YS_MIME_TYPE_RDF = "application/rdf+xml";
const char* YS_MIME_TYPE_RPM = "application/x-redhat-package-manager";
const char* YS_MIME_TYPE_RSS = "application/rss+xml";
const char* YS_MIME_TYPE_RTF = "application/rtf";
const char* YS_MIME_TYPE_RUN = "application/x-makeself";
const char* YS_MIME_TYPE_SEA = "application/x-sea";
const char* YS_MIME_TYPE_SHTML = "text/html";
const char* YS_MIME_TYPE_SIT = "application/x-stuffit";
const char* YS_MIME_TYPE_SPX = "audio/ogg";
const char* YS_MIME_TYPE_SVG = "image/svg+xml";
const char* YS_MIME_TYPE_SVGZ = "image/svg+xml";
const char* YS_MIME_TYPE_SWF = "application/x-shockwave-flash";
const char* YS_MIME_TYPE_TCL = "application/x-tcl";
const char* YS_MIME_TYPE_TIF = "image/tiff";
const char* YS_MIME_TYPE_TIFF = "image/tiff";
const char* YS_MIME_TYPE_TK = "application/x-tcl";
const char* YS_MIME_TYPE_TS = "video/mp2t";
const char* YS_MIME_TYPE_TTC = "font/collection";
const char* YS_MIME_TYPE_TTF = "font/ttf";
const char* YS_MIME_TYPE_TTL = "text/turtle";
const char* YS_MIME_TYPE_TXT = "text/plain";
const char* YS_MIME_TYPE_UDEB = "application/vnd.debian.binary-package";
const char* YS_MIME_TYPE_USDZ = "model/vnd.pixar.usd";
const char* YS_MIME_TYPE_VTT = "text/vtt";
const char* YS_MIME_TYPE_WAR = "application/java-archive";
const char* YS_MIME_TYPE_WASM = "application/wasm";
const char* YS_MIME_TYPE_WBMP = "image/vnd.wap.wbmp";
const char* YS_MIME_TYPE_WEBM = "video/webm";
const char* YS_MIME_TYPE_WEBP = "image/webp";
const char* YS_MIME_TYPE_WML = "text/vnd.wap.wml";
const char* YS_MIME_TYPE_WMLC = "application/vnd.wap.wmlc";
const char* YS_MIME_TYPE_WMV = "video/x-ms-wmv";
const char* YS_MIME_TYPE_WOFF = "font/woff";
const char* YS_MIME_TYPE_WOFF2 = "font/woff2";
const char* YS_MIME_TYPE_XHT = "application/xhtml+xml";
const char* YS_MIME_TYPE_XHTML = "application/xhtml+xml";
const char* YS_MIME_TYPE_XLS = "application/vnd.ms-excel";
const char* YS_MIME_TYPE_XLSX =
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
const char* YS_MIME_TYPE_XML = "text/xml";
const char* YS_MIME_TYPE_XPI = "application/x-xpinstall";
const char* YS_MIME_TYPE_XSPF = "application/xspf+xml";
const char* YS_MIME_TYPE_ZIP = "application/zip";
const char* YS_MIME_TYPE_ZST = "application/zstd";
