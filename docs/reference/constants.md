# Constants

## HTTP Methods

```c
typedef enum {
  METHOD_GET = 1,
  METHOD_POST,
  METHOD_PUT,
  METHOD_PATCH,
  METHOD_DELETE,
  METHOD_OPTIONS,
  METHOD_HEAD,
  METHOD_CONNECT,
  METHOD_TRACE
} http_method;
```

The `http_method` enum is used to reference HTTP methods in Ys' APIs. If you need a string literal of the method, use `http_method_names` e.g.

```c
char* get_str = http_method_names[METHOD_GET]; // "GET"
```

## HTTP Status Codes

```c
typedef enum {
  STATUS_CONTINUE = 100,
  STATUS_SWITCHING_PROTOCOLS = 101,
  STATUS_PROCESSING = 102,
  STATUS_EARLY_HINTS = 103,

  STATUS_OK = 200,
  STATUS_CREATED = 201,
  STATUS_ACCEPTED = 202,
  STATUS_NON_AUTHORITATIVE_INFO = 203,
  STATUS_NO_CONTENT = 204,
  STATUS_RESET_CONTENT = 205,
  STATUS_PARTIAL_CONTENT = 206,
  STATUS_MULTISTATUS = 207,
  STATUS_ALREADY_REPORTED = 208,
  STATUS_IM_USED = 226,

  STATUS_MULTIPLE_CHOICES = 300,
  STATUS_MOVED_PERMANENTLY = 301,
  STATUS_FOUND = 302,
  STATUS_SEE_OTHER = 303,
  STATUS_NOT_MODIFIED = 304,
  STATUS_USE_PROXY = 305,
  STATUS_NULL_ = 306,
  STATUS_TEMPORARY_REDIRECT = 307,
  STATUS_PERMANENT_REDIRECT = 308,

  STATUS_BAD_REQUEST = 400,
  STATUS_UNAUTHORIZED = 401,
  STATUS_PAYMENT_REQUIRED = 402,
  STATUS_FORBIDDEN = 403,
  STATUS_NOT_FOUND = 404,
  STATUS_METHOD_NOT_ALLOWED = 405,
  STATUS_NOT_ACCEPTABLE = 406,
  STATUS_PROXY_AUTH_REQUIRED = 407,
  STATUS_REQUEST_TIMEOUT = 408,
  STATUS_CONFLICT = 409,
  STATUS_GONE = 410,
  STATUS_LENGTH_REQUIRED = 411,
  STATUS_PRECONDITION_FAILED = 412,
  STATUS_REQUEST_ENTITY_TOO_LARGE = 413,
  STATUS_REQUEST_URI_TOO_LONG = 414,
  STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
  STATUS_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
  STATUS_EXPECTATION_FAILED = 417,
  STATUS_TEAPOT = 418,
  STATUS_MISDIRECTED_REQUEST = 421,
  STATUS_UNPROCESSABLE_ENTITY = 422,
  STATUS_LOCKED = 423,
  STATUS_FAILED_DEPENDENCY = 424,
  STATUS_TOO_EARLY = 425,
  STATUS_UPGRADE_REQUIRED = 426,
  STATUS_PRECONDITION_REQUIRED = 428,
  STATUS_TOO_MANY_REQUESTS = 429,
  STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  STATUS_UNAVAILABLE_FOR_LEGAL_REASONS = 451,

  STATUS_INTERNAL_SERVER_ERROR = 500,
  STATUS_NOT_IMPLEMENTED = 501,
  STATUS_BAD_GATEWAY = 502,
  STATUS_SERVICE_UNAVAILABLE = 503,
  STATUS_GATEWAY_TIMEOUT = 504,
  STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
  STATUS_VARIANT_ALSO_NEGOTIATES = 506,
  STATUS_INSUFFICIENT_STORAGE = 507,
  STATUS_LOOP_DETECTED = 508,
  STATUS_NOT_EXTENDED = 510,
  STATUS_NETWORK_AUTHENTICATION_REQUIRED = 511,
} http_status;
```

If you need a string literal of the status, use `http_status_names` e.g.

```c
char* get_str = http_status_names[STATUS_BAD_REQUEST]; // "Bad Request"
```

## MIME Type Constants

```c
extern const*
```
|name|value|
|-|-|
|MIME_TYPE_3GP| video/3gpp|
|MIME_TYPE_3GPP| video/3gpp|
|MIME_TYPE_7Z| application/x-7z-compressed|
|MIME_TYPE_AI| application/postscript|
|MIME_TYPE_APNG| image/apng|
|MIME_TYPE_ASF| video/x-ms-asf|
|MIME_TYPE_ASX| video/x-ms-asf|
|MIME_TYPE_ATOM| application/atom+xml|
|MIME_TYPE_AVI| video/x-msvideo|
|MIME_TYPE_AVIF| image/avif|
|MIME_TYPE_BIN| application/octet-stream|
|MIME_TYPE_BMP| image/x-ms-bmp|
|MIME_TYPE_CCO| application/x-cocoa|
|MIME_TYPE_CRT| application/x-x509-ca-cert|
|MIME_TYPE_CSS| text/css|
|MIME_TYPE_CSV| text/csv|
|MIME_TYPE_CUR| image/x-icon|
|MIME_TYPE_DEB| application/vnd.debian.binary-package|
|MIME_TYPE_DER| application/x-x509-ca-cert|
|MIME_TYPE_DLL| application/vnd.microsoft.portable-executable|
|MIME_TYPE_Dmg| application/octet-stream|
|MIME_TYPE_DOC| application/msword|
|MIME_TYPE_DOCX| application/vnd.openxmlformats-officedocument.wordprocessingml.document;
|MIME_TYPE_EAR| application/java-archive|
|MIME_TYPE_EOT| application/vnd.ms-fontobject|
|MIME_TYPE_EPS| application/postscript|
|MIME_TYPE_EPUB| application/epub+zip|
|MIME_TYPE_EXE| application/vnd.microsoft.portable-executable|
|MIME_TYPE_FLV| video/x-flv|
|MIME_TYPE_GEOJSON| application/geo+json|
|MIME_TYPE_GIF| image/gif|
|MIME_TYPE_GLB| model/gltf-binary|
|MIME_TYPE_GLTF| model/gltf+json|
|MIME_TYPE_HQX| application/mac-binhex40|
|MIME_TYPE_HTC| text/x-component|
|MIME_TYPE_HTM| text/html|
|MIME_TYPE_HTML| text/html|
|MIME_TYPE_ICO| image/x-icon|
|MIME_TYPE_IMG| application/octet-stream|
|MIME_TYPE_ISO| application/octet-stream|
|MIME_TYPE_JAD| text/vnd.sun.j2me.app-descriptor|
|MIME_TYPE_JAR| application/java-archive|
|MIME_TYPE_JARDIFF| application/x-java-archive-diff|
|MIME_TYPE_JNG| image/x-jng|
|MIME_TYPE_JNLP| application/x-java-jnlp-file|
|MIME_TYPE_JP2| image/jp2|
|MIME_TYPE_JPEG| image/jpeg|
|MIME_TYPE_JPG| image/jpeg|
|MIME_TYPE_JS| text/javascript|
|MIME_TYPE_JSON| application/json|
|MIME_TYPE_JSONLD| application/ld+json|
|MIME_TYPE_JXL| image/jxl|
|MIME_TYPE_JXR| image/jxr|
|MIME_TYPE_KAR| audio/midi|
|MIME_TYPE_KML| application/vnd.google-earth.kml+xml|
|MIME_TYPE_KMZ| application/vnd.google-earth.kmz|
|MIME_TYPE_M3U8| application/vnd.apple.mpegurl|
|MIME_TYPE_M4A| audio/x-m4a|
|MIME_TYPE_M4V| video/x-m4v|
|MIME_TYPE_MD| text/markdown|
|MIME_TYPE_MID| audio/midi|
|MIME_TYPE_MIDI| audio/midi|
|MIME_TYPE_MJS| text/javascript|
|MIME_TYPE_MML| text/mathml|
|MIME_TYPE_MNG| video/x-mng|
|MIME_TYPE_MOV| video/quicktime|
|MIME_TYPE_MP3| audio/mpeg|
|MIME_TYPE_MP4| video/mp4|
|MIME_TYPE_MPEG| video/mpeg|
|MIME_TYPE_MPG| video/mpeg|
|MIME_TYPE_MSI| application/octet-stream|
|MIME_TYPE_MSM| application/octet-stream|
|MIME_TYPE_MSP| application/octet-stream|
|MIME_TYPE_OGA| audio/ogg|
|MIME_TYPE_OGG| audio/ogg|
|MIME_TYPE_OGV| video/ogg|
|MIME_TYPE_OGX| application/ogg|
|MIME_TYPE_OPUS| audio/ogg|
|MIME_TYPE_OTC| font/collection|
|MIME_TYPE_OTF| font/otf|
|MIME_TYPE_PDB| application/x-pilot|
|MIME_TYPE_PDF| application/pdf|
|MIME_TYPE_PEM| application/x-x509-ca-cert|
|MIME_TYPE_PL| application/x-perl|
|MIME_TYPE_PM| application/x-perl|
|MIME_TYPE_PNG| image/png|
|MIME_TYPE_PPT| application/vnd.ms-powerpoint|
|MIME_TYPE_PPTX|     application/vnd.openxmlformats-officedocument.presentationml.presentation;
|MIME_TYPE_PRC| application/x-pilot|
|MIME_TYPE_PS| application/postscript|
|MIME_TYPE_RA| audio/x-realaudio|
|MIME_TYPE_RAR| application/vnd.rar|
|MIME_TYPE_RDF| application/rdf+xml|
|MIME_TYPE_RPM| application/x-redhat-package-manager|
|MIME_TYPE_RSS| application/rss+xml|
|MIME_TYPE_RTF| application/rtf|
|MIME_TYPE_RUN| application/x-makeself|
|MIME_TYPE_SEA| application/x-sea|
|MIME_TYPE_SHTML| text/html|
|MIME_TYPE_SIT| application/x-stuffit|
|MIME_TYPE_SPX| audio/ogg|
|MIME_TYPE_SVG| image/svg+xml|
|MIME_TYPE_SVGZ| image/svg+xml|
|MIME_TYPE_SWF| application/x-shockwave-flash|
|MIME_TYPE_TCL| application/x-tcl|
|MIME_TYPE_TIF| image/tiff|
|MIME_TYPE_TIFF| image/tiff|
|MIME_TYPE_TK| application/x-tcl|
|MIME_TYPE_TS| video/mp2t|
|MIME_TYPE_TTC| font/collection|
|MIME_TYPE_TTF| font/ttf|
|MIME_TYPE_TTL| text/turtle|
|MIME_TYPE_TXT| text/plain|
|MIME_TYPE_UDEB| application/vnd.debian.binary-package|
|MIME_TYPE_USDZ| model/vnd.pixar.usd|
|MIME_TYPE_VTT| text/vtt|
|MIME_TYPE_WAR| application/java-archive|
|MIME_TYPE_WASM| application/wasm|
|MIME_TYPE_WBMP| image/vnd.wap.wbmp|
|MIME_TYPE_WEBM| video/webm|
|MIME_TYPE_WEBP| image/webp|
|MIME_TYPE_WML| text/vnd.wap.wml|
|MIME_TYPE_WMLC| application/vnd.wap.wmlc|
|MIME_TYPE_WMV| video/x-ms-wmv|
|MIME_TYPE_WOFF| font/woff|
|MIME_TYPE_WOFF2| font/woff2|
|MIME_TYPE_XHT| application/xhtml+xml|
|MIME_TYPE_XHTML| application/xhtml+xml|
|MIME_TYPE_XLS| application/vnd.ms-excel|
|MIME_TYPE_XLSX|application/vnd.openxmlformats-officedocument.spreadsheetml.sheet|
|MIME_TYPE_XML| text/xml|
|MIME_TYPE_XPI| application/x-xpinstall|
|MIME_TYPE_XSPF| application/xspf+xml|
|MIME_TYPE_ZIP| application/zip|
|MIME_TYPE_ZST| application/zstd|

## Time Helpers

### n_minutes_from_now

```c
time_t n_minutes_from_now(unsigned int n)
```

`n_minutes_from_now` returns a `time_t` representing the current time plus *n* minutes.


### n_hours_from_now

```c
time_t n_hours_from_now(unsigned int n)
```

`n_hours_from_now` returns a `time_t` representing the current time plus *n* hours.

### n_days_from_now

```c
time_t n_days_from_now(unsigned int n)
```

`n_days_from_now` returns a `time_t` representing the current time plus *n* days.
