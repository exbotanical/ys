#include "libys.h"
#include "util.h"

#define X(e) [e] = #e + 7

// Mapping of HTTP status codes to stringified names
const char* http_status_names[] = {
    [STATUS_CONTINUE] = "Continue",
    [STATUS_SWITCHING_PROTOCOLS] = "Switching Protocols",
    [STATUS_PROCESSING] = "Processing",
    [STATUS_EARLY_HINTS] = "Early Hints",

    [STATUS_OK] = "OK",
    [STATUS_CREATED] = "Created",
    [STATUS_ACCEPTED] = "Accepted",
    [STATUS_NON_AUTHORITATIVE_INFO] = "Non-Authoritative Information",
    [STATUS_NO_CONTENT] = "No Content",
    [STATUS_RESET_CONTENT] = "Reset Content",
    [STATUS_PARTIAL_CONTENT] = "Partial Content",
    [STATUS_MULTISTATUS] = "Multi-Status",
    [STATUS_ALREADY_REPORTED] = "Already Reported",
    [STATUS_IM_USED] = "IM Used",

    [STATUS_MULTIPLE_CHOICES] = "Multiple Choices",
    [STATUS_MOVED_PERMANENTLY] = "Moved Permanently",
    [STATUS_FOUND] = "Found",
    [STATUS_SEE_OTHER] = "See Other",
    [STATUS_NOT_MODIFIED] = "Not Modified",
    [STATUS_USE_PROXY] = "Use Proxy",
    [STATUS_NULL_] = "NULL_",
    [STATUS_TEMPORARY_REDIRECT] = "Temporary Redirect",
    [STATUS_PERMANENT_REDIRECT] = "Permanent Redirect",

    [STATUS_BAD_REQUEST] = "Bad Request",
    [STATUS_UNAUTHORIZED] = "Unauthorized",
    [STATUS_PAYMENT_REQUIRED] = "Payment Required",
    [STATUS_FORBIDDEN] = "Forbidden",
    [STATUS_NOT_FOUND] = "Not Found",
    [STATUS_METHOD_NOT_ALLOWED] = "Method Not Allowed",
    [STATUS_NOT_ACCEPTABLE] = "Not Acceptable",
    [STATUS_PROXY_AUTH_REQUIRED] = "Proxy Authentication Required",
    [STATUS_REQUEST_TIMEOUT] = "Request Timeout",
    [STATUS_CONFLICT] = "Conflict",
    [STATUS_GONE] = "Gone",
    [STATUS_LENGTH_REQUIRED] = "Length Required",
    [STATUS_PRECONDITION_FAILED] = "Precondition Failed",
    [STATUS_REQUEST_ENTITY_TOO_LARGE] = "Request Entity Too Large",
    [STATUS_REQUEST_URI_TOO_LONG] = "Request URI Too Long",
    [STATUS_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type",
    [STATUS_REQUESTED_RANGE_NOT_SATISFIABLE] =
        "Requested Range Not Satisfiable",
    [STATUS_EXPECTATION_FAILED] = "Expectation Failed",
    [STATUS_TEAPOT] = "I'm a teapot",
    [STATUS_MISDIRECTED_REQUEST] = "Misdirected Request",
    [STATUS_UNPROCESSABLE_ENTITY] = "Unprocessable Entity",
    [STATUS_LOCKED] = "Locked",
    [STATUS_FAILED_DEPENDENCY] = "Failed Dependency",
    [STATUS_TOO_EARLY] = "Too Early",
    [STATUS_UPGRADE_REQUIRED] = "Upgrade Required",
    [STATUS_PRECONDITION_REQUIRED] = "Precondition Required",
    [STATUS_TOO_MANY_REQUESTS] = "Too Many Requests",
    [STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE] =
        "Request Header Fields Too Large",
    [STATUS_UNAVAILABLE_FOR_LEGAL_REASONS] = "Unavailable For Legal Reasons",

    [STATUS_INTERNAL_SERVER_ERROR] = "Internal Server Error",
    [STATUS_NOT_IMPLEMENTED] = "Not Implemented",
    [STATUS_BAD_GATEWAY] = "Bad Gateway",
    [STATUS_SERVICE_UNAVAILABLE] = "Service Unavailable",
    [STATUS_GATEWAY_TIMEOUT] = "Gateway Timeout",
    [STATUS_HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported",
    [STATUS_VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates",
    [STATUS_INSUFFICIENT_STORAGE] = "Insufficient Storage",
    [STATUS_LOOP_DETECTED] = "Loop Detected",
    [STATUS_NOT_EXTENDED] = "Not Extended",
    [STATUS_NETWORK_AUTHENTICATION_REQUIRED] =
        "Network Authentication Required",
};

// Mapping of HTTP method name enums to stringified names
const char* http_method_names[] = {
    X(METHOD_GET),     X(METHOD_HEAD),    X(METHOD_POST),
    X(METHOD_PUT),     X(METHOD_PATCH),   X(METHOD_DELETE),
    X(METHOD_CONNECT), X(METHOD_OPTIONS), X(METHOD_TRACE)};

const char* MIME_TYPE_3GP = "video/3gpp";
const char* MIME_TYPE_3GPP = "video/3gpp";
const char* MIME_TYPE_7Z = "application/x-7z-compressed";
const char* MIME_TYPE_AI = "application/postscript";
const char* MIME_TYPE_APNG = "image/apng";
const char* MIME_TYPE_ASF = "video/x-ms-asf";
const char* MIME_TYPE_ASX = "video/x-ms-asf";
const char* MIME_TYPE_ATOM = "application/atom+xml";
const char* MIME_TYPE_AVI = "video/x-msvideo";
const char* MIME_TYPE_AVIF = "image/avif";
const char* MIME_TYPE_BIN = "application/octet-stream";
const char* MIME_TYPE_BMP = "image/x-ms-bmp";
const char* MIME_TYPE_CCO = "application/x-cocoa";
const char* MIME_TYPE_CRT = "application/x-x509-ca-cert";
const char* MIME_TYPE_CSS = "text/css";
const char* MIME_TYPE_CSV = "text/csv";
const char* MIME_TYPE_CUR = "image/x-icon";
const char* MIME_TYPE_DEB = "application/vnd.debian.binary-package";
const char* MIME_TYPE_DER = "application/x-x509-ca-cert";
const char* MIME_TYPE_DLL = "application/vnd.microsoft.portable-executable";
const char* MIME_TYPE_Dmg = "application/octet-stream";
const char* MIME_TYPE_DOC = "application/msword";
const char* MIME_TYPE_DOCX =
    "application/"
    "vnd.openxmlformats-officedocument.wordprocessingml.document";
const char* MIME_TYPE_EAR = "application/java-archive";
const char* MIME_TYPE_EOT = "application/vnd.ms-fontobject";
const char* MIME_TYPE_EPS = "application/postscript";
const char* MIME_TYPE_EPUB = "application/epub+zip";
const char* MIME_TYPE_EXE = "application/vnd.microsoft.portable-executable";
const char* MIME_TYPE_FLV = "video/x-flv";
const char* MIME_TYPE_GEOJSON = "application/geo+json";
const char* MIME_TYPE_GIF = "image/gif";
const char* MIME_TYPE_GLB = "model/gltf-binary";
const char* MIME_TYPE_GLTF = "model/gltf+json";
const char* MIME_TYPE_HQX = "application/mac-binhex40";
const char* MIME_TYPE_HTC = "text/x-component";
const char* MIME_TYPE_HTM = "text/html";
const char* MIME_TYPE_HTML = "text/html";
const char* MIME_TYPE_ICO = "image/x-icon";
const char* MIME_TYPE_IMG = "application/octet-stream";
const char* MIME_TYPE_ISO = "application/octet-stream";
const char* MIME_TYPE_JAD = "text/vnd.sun.j2me.app-descriptor";
const char* MIME_TYPE_JAR = "application/java-archive";
const char* MIME_TYPE_JARDIFF = "application/x-java-archive-diff";
const char* MIME_TYPE_JNG = "image/x-jng";
const char* MIME_TYPE_JNLP = "application/x-java-jnlp-file";
const char* MIME_TYPE_JP2 = "image/jp2";
const char* MIME_TYPE_JPEG = "image/jpeg";
const char* MIME_TYPE_JPG = "image/jpeg";
const char* MIME_TYPE_JS = "text/javascript";
const char* MIME_TYPE_JSON = "application/json";
const char* MIME_TYPE_JSONLD = "application/ld+json";
const char* MIME_TYPE_JXL = "image/jxl";
const char* MIME_TYPE_JXR = "image/jxr";
const char* MIME_TYPE_KAR = "audio/midi";
const char* MIME_TYPE_KML = "application/vnd.google-earth.kml+xml";
const char* MIME_TYPE_KMZ = "application/vnd.google-earth.kmz";
const char* MIME_TYPE_M3U8 = "application/vnd.apple.mpegurl";
const char* MIME_TYPE_M4A = "audio/x-m4a";
const char* MIME_TYPE_M4V = "video/x-m4v";
const char* MIME_TYPE_MD = "text/markdown";
const char* MIME_TYPE_MID = "audio/midi";
const char* MIME_TYPE_MIDI = "audio/midi";
const char* MIME_TYPE_MJS = "text/javascript";
const char* MIME_TYPE_MML = "text/mathml";
const char* MIME_TYPE_MNG = "video/x-mng";
const char* MIME_TYPE_MOV = "video/quicktime";
const char* MIME_TYPE_MP3 = "audio/mpeg";
const char* MIME_TYPE_MP4 = "video/mp4";
const char* MIME_TYPE_MPEG = "video/mpeg";
const char* MIME_TYPE_MPG = "video/mpeg";
const char* MIME_TYPE_MSI = "application/octet-stream";
const char* MIME_TYPE_MSM = "application/octet-stream";
const char* MIME_TYPE_MSP = "application/octet-stream";
const char* MIME_TYPE_OGA = "audio/ogg";
const char* MIME_TYPE_OGG = "audio/ogg";
const char* MIME_TYPE_OGV = "video/ogg";
const char* MIME_TYPE_OGX = "application/ogg";
const char* MIME_TYPE_OPUS = "audio/ogg";
const char* MIME_TYPE_OTC = "font/collection";
const char* MIME_TYPE_OTF = "font/otf";
const char* MIME_TYPE_PDB = "application/x-pilot";
const char* MIME_TYPE_PDF = "application/pdf";
const char* MIME_TYPE_PEM = "application/x-x509-ca-cert";
const char* MIME_TYPE_PL = "application/x-perl";
const char* MIME_TYPE_PM = "application/x-perl";
const char* MIME_TYPE_PNG = "image/png";
const char* MIME_TYPE_PPT = "application/vnd.ms-powerpoint";
const char* MIME_TYPE_PPTX =
    "application/"
    "vnd.openxmlformats-officedocument.presentationml.presentation";
const char* MIME_TYPE_PRC = "application/x-pilot";
const char* MIME_TYPE_PS = "application/postscript";
const char* MIME_TYPE_RA = "audio/x-realaudio";
const char* MIME_TYPE_RAR = "application/vnd.rar";
const char* MIME_TYPE_RDF = "application/rdf+xml";
const char* MIME_TYPE_RPM = "application/x-redhat-package-manager";
const char* MIME_TYPE_RSS = "application/rss+xml";
const char* MIME_TYPE_RTF = "application/rtf";
const char* MIME_TYPE_RUN = "application/x-makeself";
const char* MIME_TYPE_SEA = "application/x-sea";
const char* MIME_TYPE_SHTML = "text/html";
const char* MIME_TYPE_SIT = "application/x-stuffit";
const char* MIME_TYPE_SPX = "audio/ogg";
const char* MIME_TYPE_SVG = "image/svg+xml";
const char* MIME_TYPE_SVGZ = "image/svg+xml";
const char* MIME_TYPE_SWF = "application/x-shockwave-flash";
const char* MIME_TYPE_TCL = "application/x-tcl";
const char* MIME_TYPE_TIF = "image/tiff";
const char* MIME_TYPE_TIFF = "image/tiff";
const char* MIME_TYPE_TK = "application/x-tcl";
const char* MIME_TYPE_TS = "video/mp2t";
const char* MIME_TYPE_TTC = "font/collection";
const char* MIME_TYPE_TTF = "font/ttf";
const char* MIME_TYPE_TTL = "text/turtle";
const char* MIME_TYPE_TXT = "text/plain";
const char* MIME_TYPE_UDEB = "application/vnd.debian.binary-package";
const char* MIME_TYPE_USDZ = "model/vnd.pixar.usd";
const char* MIME_TYPE_VTT = "text/vtt";
const char* MIME_TYPE_WAR = "application/java-archive";
const char* MIME_TYPE_WASM = "application/wasm";
const char* MIME_TYPE_WBMP = "image/vnd.wap.wbmp";
const char* MIME_TYPE_WEBM = "video/webm";
const char* MIME_TYPE_WEBP = "image/webp";
const char* MIME_TYPE_WML = "text/vnd.wap.wml";
const char* MIME_TYPE_WMLC = "application/vnd.wap.wmlc";
const char* MIME_TYPE_WMV = "video/x-ms-wmv";
const char* MIME_TYPE_WOFF = "font/woff";
const char* MIME_TYPE_WOFF2 = "font/woff2";
const char* MIME_TYPE_XHT = "application/xhtml+xml";
const char* MIME_TYPE_XHTML = "application/xhtml+xml";
const char* MIME_TYPE_XLS = "application/vnd.ms-excel";
const char* MIME_TYPE_XLSX =
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
const char* MIME_TYPE_XML = "text/xml";
const char* MIME_TYPE_XPI = "application/x-xpinstall";
const char* MIME_TYPE_XSPF = "application/xspf+xml";
const char* MIME_TYPE_ZIP = "application/zip";
const char* MIME_TYPE_ZST = "application/zstd";
