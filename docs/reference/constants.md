# Constants

## HTTP Methods

```c
typedef enum {
  YS_METHOD_GET = 1,
  YS_METHOD_POST,
  YS_METHOD_PUT,
  YS_METHOD_PATCH,
  YS_METHOD_DELETE,
  YS_METHOD_OPTIONS,
  YS_METHOD_HEAD,
  YS_METHOD_CONNECT,
  YS_METHOD_TRACE
} ys_http_method;
```

The `ys_http_method` enum is used to reference HTTP methods in Ys' APIs. If you need a string literal of the method, use `ys_http_method_names` e.g.

```c
char* get_str = ys_http_method_names[YS_METHOD_GET]; // "GET"
```

## HTTP Status Codes

```c
typedef enum {
  YS_STATUS_CONTINUE = 100,
  YS_STATUS_SWITCHING_PROTOCOLS = 101,
  YS_STATUS_PROCESSING = 102,
  YS_STATUS_EARLY_HINTS = 103,

  YS_STATUS_OK = 200,
  YS_STATUS_CREATED = 201,
  YS_STATUS_ACCEPTED = 202,
  YS_STATUS_NON_AUTHORITATIVE_INFO = 203,
  YS_STATUS_NO_CONTENT = 204,
  YS_STATUS_RESET_CONTENT = 205,
  YS_STATUS_PARTIAL_CONTENT = 206,
  YS_STATUS_MULTISTATUS = 207,
  YS_STATUS_ALREADY_REPORTED = 208,
  YS_STATUS_IM_USED = 226,

  YS_STATUS_MULTIPLE_CHOICES = 300,
  YS_STATUS_MOVED_PERMANENTLY = 301,
  YS_STATUS_FOUND = 302,
  YS_STATUS_SEE_OTHER = 303,
  YS_STATUS_NOT_MODIFIED = 304,
  YS_STATUS_USE_PROXY = 305,
  YS_STATUS_NULL_ = 306,
  YS_STATUS_TEMPORARY_REDIRECT = 307,
  YS_STATUS_PERMANENT_REDIRECT = 308,

  YS_STATUS_BAD_REQUEST = 400,
  YS_STATUS_UNAUTHORIZED = 401,
  YS_STATUS_PAYMENT_REQUIRED = 402,
  YS_STATUS_FORBIDDEN = 403,
  YS_STATUS_NOT_FOUND = 404,
  YS_STATUS_YS_METHOD_NOT_ALLOWED = 405,
  YS_STATUS_NOT_ACCEPTABLE = 406,
  YS_STATUS_PROXY_AUTH_REQUIRED = 407,
  YS_STATUS_REQUEST_TIMEOUT = 408,
  YS_STATUS_CONFLICT = 409,
  YS_STATUS_GONE = 410,
  YS_STATUS_LENGTH_REQUIRED = 411,
  YS_STATUS_PRECONDITION_FAILED = 412,
  YS_STATUS_REQUEST_ENTITY_TOO_LARGE = 413,
  YS_STATUS_REQUEST_URI_TOO_LONG = 414,
  YS_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
  YS_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
  YS_STATUS_EXPECTATION_FAILED = 417,
  YS_STATUS_TEAPOT = 418,
  YS_STATUS_MISDIRECTED_REQUEST = 421,
  YS_STATUS_UNPROCESSABLE_ENTITY = 422,
  YS_STATUS_LOCKED = 423,
  YS_STATUS_FAILED_DEPENDENCY = 424,
  YS_STATUS_TOO_EARLY = 425,
  YS_STATUS_UPGRADE_REQUIRED = 426,
  YS_STATUS_PRECONDITION_REQUIRED = 428,
  YS_STATUS_TOO_MANY_REQUESTS = 429,
  YS_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  YS_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

  YS_STATUS_INTERNAL_SERVER_ERROR = 500,
  YS_STATUS_NOT_IMPLEMENTED = 501,
  YS_STATUS_BAD_GATEWAY = 502,
  YS_STATUS_SERVICE_UNAVAILABLE = 503,
  YS_STATUS_GATEWAY_TIMEOUT = 504,
  YS_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
  YS_STATUS_VARIANT_ALSO_NEGOTIATES = 506,
  YS_STATUS_INSUFFICIENT_STORAGE = 507,
  YS_STATUS_LOOP_DETECTED = 508,
  YS_STATUS_NOT_EXTENDED = 510,
  YS_STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,
} ys_http_status;
```

If you need a string literal of the status, use `ys_http_status_names` e.g.

```c
char* get_str = ys_http_status_names[YS_STATUS_BAD_REQUEST]; // "Bad Request"
```

## MIME Type Constants

```c
extern const*
```
|name|value|
|-|-|
|YS_MIME_TYPE_3GP| video/3gpp|
|YS_MIME_TYPE_3GPP| video/3gpp|
|YS_MIME_TYPE_7Z| application/x-7z-compressed|
|YS_MIME_TYPE_AI| application/postscript|
|YS_MIME_TYPE_APNG| image/apng|
|YS_MIME_TYPE_ASF| video/x-ms-asf|
|YS_MIME_TYPE_ASX| video/x-ms-asf|
|YS_MIME_TYPE_ATOM| application/atom+xml|
|YS_MIME_TYPE_AVI| video/x-msvideo|
|YS_MIME_TYPE_AVIF| image/avif|
|YS_MIME_TYPE_BIN| application/octet-stream|
|YS_MIME_TYPE_BMP| image/x-ms-bmp|
|YS_MIME_TYPE_CCO| application/x-cocoa|
|YS_MIME_TYPE_CRT| application/x-x509-ca-cert|
|YS_MIME_TYPE_CSS| text/css|
|YS_MIME_TYPE_CSV| text/csv|
|YS_MIME_TYPE_CUR| image/x-icon|
|YS_MIME_TYPE_DEB| application/vnd.debian.binary-package|
|YS_MIME_TYPE_DER| application/x-x509-ca-cert|
|YS_MIME_TYPE_DLL| application/vnd.microsoft.portable-executable|
|YS_MIME_TYPE_Dmg| application/octet-stream|
|YS_MIME_TYPE_DOC| application/msword|
|YS_MIME_TYPE_DOCX| application/vnd.openxmlformats-officedocument.wordprocessingml.document;
|YS_MIME_TYPE_EAR| application/java-archive|
|YS_MIME_TYPE_EOT| application/vnd.ms-fontobject|
|YS_MIME_TYPE_EPS| application/postscript|
|YS_MIME_TYPE_EPUB| application/epub+zip|
|YS_MIME_TYPE_EXE| application/vnd.microsoft.portable-executable|
|YS_MIME_TYPE_FLV| video/x-flv|
|YS_MIME_TYPE_GEOJSON| application/geo+json|
|YS_MIME_TYPE_GIF| image/gif|
|YS_MIME_TYPE_GLB| model/gltf-binary|
|YS_MIME_TYPE_GLTF| model/gltf+json|
|YS_MIME_TYPE_HQX| application/mac-binhex40|
|YS_MIME_TYPE_HTC| text/x-component|
|YS_MIME_TYPE_HTM| text/html|
|YS_MIME_TYPE_HTML| text/html|
|YS_MIME_TYPE_ICO| image/x-icon|
|YS_MIME_TYPE_IMG| application/octet-stream|
|YS_MIME_TYPE_ISO| application/octet-stream|
|YS_MIME_TYPE_JAD| text/vnd.sun.j2me.app-descriptor|
|YS_MIME_TYPE_JAR| application/java-archive|
|YS_MIME_TYPE_JARDIFF| application/x-java-archive-diff|
|YS_MIME_TYPE_JNG| image/x-jng|
|YS_MIME_TYPE_JNLP| application/x-java-jnlp-file|
|YS_MIME_TYPE_JP2| image/jp2|
|YS_MIME_TYPE_JPEG| image/jpeg|
|YS_MIME_TYPE_JPG| image/jpeg|
|YS_MIME_TYPE_JS| text/javascript|
|YS_MIME_TYPE_JSON| application/json|
|YS_MIME_TYPE_JSONLD| application/ld+json|
|YS_MIME_TYPE_JXL| image/jxl|
|YS_MIME_TYPE_JXR| image/jxr|
|YS_MIME_TYPE_KAR| audio/midi|
|YS_MIME_TYPE_KML| application/vnd.google-earth.kml+xml|
|YS_MIME_TYPE_KMZ| application/vnd.google-earth.kmz|
|YS_MIME_TYPE_M3U8| application/vnd.apple.mpegurl|
|YS_MIME_TYPE_M4A| audio/x-m4a|
|YS_MIME_TYPE_M4V| video/x-m4v|
|YS_MIME_TYPE_MD| text/markdown|
|YS_MIME_TYPE_MID| audio/midi|
|YS_MIME_TYPE_MIDI| audio/midi|
|YS_MIME_TYPE_MJS| text/javascript|
|YS_MIME_TYPE_MML| text/mathml|
|YS_MIME_TYPE_MNG| video/x-mng|
|YS_MIME_TYPE_MOV| video/quicktime|
|YS_MIME_TYPE_MP3| audio/mpeg|
|YS_MIME_TYPE_MP4| video/mp4|
|YS_MIME_TYPE_MPEG| video/mpeg|
|YS_MIME_TYPE_MPG| video/mpeg|
|YS_MIME_TYPE_MSI| application/octet-stream|
|YS_MIME_TYPE_MSM| application/octet-stream|
|YS_MIME_TYPE_MSP| application/octet-stream|
|YS_MIME_TYPE_OGA| audio/ogg|
|YS_MIME_TYPE_OGG| audio/ogg|
|YS_MIME_TYPE_OGV| video/ogg|
|YS_MIME_TYPE_OGX| application/ogg|
|YS_MIME_TYPE_OPUS| audio/ogg|
|YS_MIME_TYPE_OTC| font/collection|
|YS_MIME_TYPE_OTF| font/otf|
|YS_MIME_TYPE_PDB| application/x-pilot|
|YS_MIME_TYPE_PDF| application/pdf|
|YS_MIME_TYPE_PEM| application/x-x509-ca-cert|
|YS_MIME_TYPE_PL| application/x-perl|
|YS_MIME_TYPE_PM| application/x-perl|
|YS_MIME_TYPE_PNG| image/png|
|YS_MIME_TYPE_PPT| application/vnd.ms-powerpoint|
|YS_MIME_TYPE_PPTX|     application/vnd.openxmlformats-officedocument.presentationml.presentation;
|YS_MIME_TYPE_PRC| application/x-pilot|
|YS_MIME_TYPE_PS| application/postscript|
|YS_MIME_TYPE_RA| audio/x-realaudio|
|YS_MIME_TYPE_RAR| application/vnd.rar|
|YS_MIME_TYPE_RDF| application/rdf+xml|
|YS_MIME_TYPE_RPM| application/x-redhat-package-manager|
|YS_MIME_TYPE_RSS| application/rss+xml|
|YS_MIME_TYPE_RTF| application/rtf|
|YS_MIME_TYPE_RUN| application/x-makeself|
|YS_MIME_TYPE_SEA| application/x-sea|
|YS_MIME_TYPE_SHTML| text/html|
|YS_MIME_TYPE_SIT| application/x-stuffit|
|YS_MIME_TYPE_SPX| audio/ogg|
|YS_MIME_TYPE_SVG| image/svg+xml|
|YS_MIME_TYPE_SVGZ| image/svg+xml|
|YS_MIME_TYPE_SWF| application/x-shockwave-flash|
|YS_MIME_TYPE_TCL| application/x-tcl|
|YS_MIME_TYPE_TIF| image/tiff|
|YS_MIME_TYPE_TIFF| image/tiff|
|YS_MIME_TYPE_TK| application/x-tcl|
|YS_MIME_TYPE_TS| video/mp2t|
|YS_MIME_TYPE_TTC| font/collection|
|YS_MIME_TYPE_TTF| font/ttf|
|YS_MIME_TYPE_TTL| text/turtle|
|YS_MIME_TYPE_TXT| text/plain|
|YS_MIME_TYPE_UDEB| application/vnd.debian.binary-package|
|YS_MIME_TYPE_USDZ| model/vnd.pixar.usd|
|YS_MIME_TYPE_VTT| text/vtt|
|YS_MIME_TYPE_WAR| application/java-archive|
|YS_MIME_TYPE_WASM| application/wasm|
|YS_MIME_TYPE_WBMP| image/vnd.wap.wbmp|
|YS_MIME_TYPE_WEBM| video/webm|
|YS_MIME_TYPE_WEBP| image/webp|
|YS_MIME_TYPE_WML| text/vnd.wap.wml|
|YS_MIME_TYPE_WMLC| application/vnd.wap.wmlc|
|YS_MIME_TYPE_WMV| video/x-ms-wmv|
|YS_MIME_TYPE_WOFF| font/woff|
|YS_MIME_TYPE_WOFF2| font/woff2|
|YS_MIME_TYPE_XHT| application/xhtml+xml|
|YS_MIME_TYPE_XHTML| application/xhtml+xml|
|YS_MIME_TYPE_XLS| application/vnd.ms-excel|
|YS_MIME_TYPE_XLSX|application/vnd.openxmlformats-officedocument.spreadsheetml.sheet|
|YS_MIME_TYPE_XML| text/xml|
|YS_MIME_TYPE_XPI| application/x-xpinstall|
|YS_MIME_TYPE_XSPF| application/xspf+xml|
|YS_MIME_TYPE_ZIP| application/zip|
|YS_MIME_TYPE_ZST| application/zstd|

## Time Helpers

### ys_n_minutes_from_now

```c
time_t ys_n_minutes_from_now(unsigned int n)
```

`ys_n_minutes_from_now` returns a `time_t` representing the current time plus *n* minutes.


### ys_n_hours_from_now

```c
time_t ys_n_hours_from_now(unsigned int n)
```

`ys_n_hours_from_now` returns a `time_t` representing the current time plus *n* hours.

### ys_n_days_from_now

```c
time_t ys_n_days_from_now(unsigned int n)
```

`ys_n_days_from_now` returns a `time_t` representing the current time plus *n* days.
