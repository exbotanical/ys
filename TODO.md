# TODOs

## Pre-release
- [ ] All best practices
- [ ] All fixes
- [x] SSL support
- [x] Cookies

- [x] Clean up all objects w/ free where necessary
- [x] Validate NULLs and allocated objects
- [x] Add logging
- [x] HTTP request parser
- [x] static
- [x] error-handling
- [x] properly parse entire request (we're using a placeholder method right now!)
- [x] ~~Use a union to store parameter values. We have enough info from the regex to determine whether the value should be an int or char*~~ - this would be a pain for users
- [x] pass a struct with context, req, res (maybe merge ctx and req)
- [x] integ tests
- [x] read entire request in chunks
- [x] send entire response in chunks
- [x] fix request hangups
  - [x] mostly fixed, happens when calling with a not allowed/found method w/body `Could not resolve host: <body>` (was non-issue; using curl wrong :|)
- [x] prefix all enums with `METHOD_` or `STATUS_`
- [x] handle duplicate request headers
  - Go style: append subsequent values, but `Get` retrieves the first only. `Values` returns list of all. In response, each duplicate header is written separately
- [x] xmalloc that dies

## Testing
- [ ] test handlers with a mocking library?
- [ ] test with a real server (E2E)

## UX
- [ ] cross-OS compatibility
  - [x] source strdup from dep
- [ ] review all `include`d functions and replace any non-standard ones
- [ ] documentation
- [ ] make all public structs opaque
- [ ] Use method enum in CORS middleware helpers
- [ ] Ensure everything opaque has getters and setters
- [ ] Allowlist paths for middleware

## New Features
- [x] ~~global hooks/interceptors (e.g. global HTTP headers)~~ just use middleware
- [x] allow middleware to exit early by sending response
- [x] Built-in middlewares
  - [x] CORS middleware
- [x] Cookies
- [x] support erroneous (Request / Response)s
- [x] use attr struct for fallback handlers
- [ ] Figure out file routing. The problem here is we want the `response_t` type to be accessible to consumers without having to compile the entire library and link it.
- [ ] SSL support
- [x] JSON parsed body in context (decided to offer JSON opt-in utilities)
- [ ] async i/o option using epoll
  - [ ] Evented server e.g. `onRequest`
- [x] global middlewares
- [ ] multiple routers
- [ ] handle signals for graceful shutdown
- [x] query strings
- [ ] wildcard routing
- [ ] serve entire dir
- [ ] set X-Powered-By

## Best Practices
- [x] ~~specific int types~~
- [ ] check all instances of `strcmp` and see if `strncmp` makes more sense
- [ ] Determine where we actually need to be passing by reference (currently, this is done everywhere)
- [ ] Add LICENSE of yyjson
- [ ] Pass correct function pointers e.g. https://stackoverflow.com/questions/14134245/iso-c-void-and-function-pointers
- [ ] `const` pointers
- [ ] `const` structs
- [x] fix `const char CONSTANT[n]`
- [ ] `free` everywhere we can
- [ ] mutexes in examples

## Spec Compliance
- [ ] handle duplicate response headers
- [ ] Full RFC compliance
  - [ ] handle accept header
  - [ ] Validate Content-Type for incoming stateful requests
  - [ ] keep-alive
  - [ ] cache-control
  - [ ] transfer-encoding
- [ ] Test URL fragments
- [x] URL queries

## Misc Improvements
- [ ] use a better regex engine than pcre
- [x] ~~make regex cache global (instead of per-trie-node)~~ bad idea
- [x] replace `LOG` macro calls with new logger
  - [x] fix log to stderr
  - [x] use function name macro instead of explicit string literals
- [ ] inline functions where reasonable
- [x] ~~Change middleware chaining to use `next`, the next middleware in the chain. This will allow a current handler to *not* invoke `next`, thus ending the chain. Note: Figuring this out is going to be a challenge~~
- [ ] relocate JSON to util library

## DX
- [ ] helper ??? `#define CAST(type, name, stmt) type *name = (type *)stmt`
- [x] Setup linter that uses Google's C/C++ style guide
- [ ] `@return` doc for `true` and `false` cases instead of just `bool`
- [x] pick a style for struct types
- [x] Debug helpers e.g.
  ```c
  static int c = 1;
  #define checkpoint() printf("CHECKPOINT %d\n", c++)
  #define isnull(thing) printf("IS NULL? %s\n", thing == NULL ? "yes" : "no")
  ```
- [ ] Shared test utils

## Fixes
- [x] Allow OPTIONS requests by default in router when CORS enabled
- [x] Urls with `-` segfault e.g. `/feed_meta/index-0.json`
  - [ ] Add test
- [ ] curl - Connection #0 to host localhost left intact?
- [ ] Address glibc malloc bug in ip
- [ ] Do two route handlers with diff methods for the same /route get handled separately?
- [ ] Does router handle URL fragments?

## Ecosystem
- [ ] Addon / plugin libraries
  - [ ] Metrics
  - [ ] Testing (stubs, mock calls etc)

# Style Guide
- Use [Go Doc-style](https://tip.golang.org/doc/comment) for doc  comments e.g. `// function_name does thing`

- comment what is used from an include header where possible

# Desired API
- Router
  - `router_init(router_attr*)`
  - `router_set_400_handler(router_attr*, route_handler*)`
  - `router_set_400_handler(router_attr*, route_handler*)`
  - `router_set_405_handler(router_attr*, route_handler*)`
  - `router_set_500_handler(router_attr*, route_handler*)`
  - `router_register`
- Server
  - `server_init(http_router*, int)`
  - `server_start(tcp_server*)`
  - `server_free(tcp_server*)`
- Request
  - `get_request_header(request*, const char*)`
- Response
  - `set_response_header(request*, const char*, const char*)`
  - `insert_response_header(request*, const char*, const char*)`


```c

hash_table* http_mime_map;

void http_mime_map_init() {
  http_mime_map = ht_init(135);  // just search the `ht_inserts` count

  ht_insert(http_mime_map, "3gp", "video/3gpp");
  ht_insert(http_mime_map, "3gpp", "video/3gpp");
  ht_insert(http_mime_map, "7z", "application/x-7z-compressed");
  ht_insert(http_mime_map, "ai", "application/postscript");
  ht_insert(http_mime_map, "apng", "image/apng");
  ht_insert(http_mime_map, "asf", "video/x-ms-asf");
  ht_insert(http_mime_map, "asx", "video/x-ms-asf");
  ht_insert(http_mime_map, "atom", "application/atom+xml");
  ht_insert(http_mime_map, "avi", "video/x-msvideo");
  ht_insert(http_mime_map, "avif", "image/avif");
  ht_insert(http_mime_map, "bin", "application/octet-stream");
  ht_insert(http_mime_map, "bmp", "image/x-ms-bmp");
  ht_insert(http_mime_map, "cco", "application/x-cocoa");
  ht_insert(http_mime_map, "crt", "application/x-x509-ca-cert");
  ht_insert(http_mime_map, "css", "text/css");
  ht_insert(http_mime_map, "csv", "text/csv");
  ht_insert(http_mime_map, "cur", "image/x-icon");
  ht_insert(http_mime_map, "deb", "application/vnd.debian.binary-package");
  ht_insert(http_mime_map, "der", "application/x-x509-ca-cert");
  ht_insert(http_mime_map, "dll",
            "application/vnd.microsoft.portable-executable");
  ht_insert(http_mime_map, "dmg", "application/octet-stream");
  ht_insert(http_mime_map, "doc", "application/msword");
  ht_insert(http_mime_map, "docx",
            "application/"
            "vnd.openxmlformats-officedocument.wordprocessingml.document");
  ht_insert(http_mime_map, "ear", "application/java-archive");
  ht_insert(http_mime_map, "eot", "application/vnd.ms-fontobject");
  ht_insert(http_mime_map, "eps", "application/postscript");
  ht_insert(http_mime_map, "epub", "application/epub+zip");
  ht_insert(http_mime_map, "exe",
            "application/vnd.microsoft.portable-executable");
  ht_insert(http_mime_map, "flv", "video/x-flv");
  ht_insert(http_mime_map, "geojson", "application/geo+json");
  ht_insert(http_mime_map, "gif", "image/gif");
  ht_insert(http_mime_map, "glb", "model/gltf-binary");
  ht_insert(http_mime_map, "gltf", "model/gltf+json");
  ht_insert(http_mime_map, "hqx", "application/mac-binhex40");
  ht_insert(http_mime_map, "htc", "text/x-component");
  ht_insert(http_mime_map, "htm", "text/html");
  ht_insert(http_mime_map, "html", "text/html");
  ht_insert(http_mime_map, "ico", "image/x-icon");
  ht_insert(http_mime_map, "img", "application/octet-stream");
  ht_insert(http_mime_map, "iso", "application/octet-stream");
  ht_insert(http_mime_map, "jad", "text/vnd.sun.j2me.app-descriptor");
  ht_insert(http_mime_map, "jar", "application/java-archive");
  ht_insert(http_mime_map, "jardiff", "application/x-java-archive-diff");
  ht_insert(http_mime_map, "jng", "image/x-jng");
  ht_insert(http_mime_map, "jnlp", "application/x-java-jnlp-file");
  ht_insert(http_mime_map, "jp2", "image/jp2");
  ht_insert(http_mime_map, "jpeg", "image/jpeg");
  ht_insert(http_mime_map, "jpg", "image/jpeg");
  ht_insert(http_mime_map, "js", "text/javascript");
  ht_insert(http_mime_map, "json", "application/json");
  ht_insert(http_mime_map, "jsonld", "application/ld+json");
  ht_insert(http_mime_map, "jxl", "image/jxl");
  ht_insert(http_mime_map, "jxr", "image/jxr");
  ht_insert(http_mime_map, "kar", "audio/midi");
  ht_insert(http_mime_map, "kml", "application/vnd.google-earth.kml+xml");
  ht_insert(http_mime_map, "kmz", "application/vnd.google-earth.kmz");
  ht_insert(http_mime_map, "m3u8", "application/vnd.apple.mpegurl");
  ht_insert(http_mime_map, "m4a", "audio/x-m4a");
  ht_insert(http_mime_map, "m4v", "video/x-m4v");
  ht_insert(http_mime_map, "md", "text/markdown");
  ht_insert(http_mime_map, "mid", "audio/midi");
  ht_insert(http_mime_map, "midi", "audio/midi");
  ht_insert(http_mime_map, "mjs", "text/javascript");
  ht_insert(http_mime_map, "mml", "text/mathml");
  ht_insert(http_mime_map, "mng", "video/x-mng");
  ht_insert(http_mime_map, "mov", "video/quicktime");
  ht_insert(http_mime_map, "mp3", "audio/mpeg");
  ht_insert(http_mime_map, "mp4", "video/mp4");
  ht_insert(http_mime_map, "mpeg", "video/mpeg");
  ht_insert(http_mime_map, "mpg", "video/mpeg");
  ht_insert(http_mime_map, "msi", "application/octet-stream");
  ht_insert(http_mime_map, "msm", "application/octet-stream");
  ht_insert(http_mime_map, "msp", "application/octet-stream");
  ht_insert(http_mime_map, "oga", "audio/ogg");
  ht_insert(http_mime_map, "ogg", "audio/ogg");
  ht_insert(http_mime_map, "ogv", "video/ogg");
  ht_insert(http_mime_map, "ogx", "application/ogg");
  ht_insert(http_mime_map, "opus", "audio/ogg");
  ht_insert(http_mime_map, "otc", "font/collection");
  ht_insert(http_mime_map, "otf", "font/otf");
  ht_insert(http_mime_map, "pdb", "application/x-pilot");
  ht_insert(http_mime_map, "pdf", "application/pdf");
  ht_insert(http_mime_map, "pem", "application/x-x509-ca-cert");
  ht_insert(http_mime_map, "pl", "application/x-perl");
  ht_insert(http_mime_map, "pm", "application/x-perl");
  ht_insert(http_mime_map, "png", "image/png");
  ht_insert(http_mime_map, "ppt", "application/vnd.ms-powerpoint");
  ht_insert(http_mime_map, "pptx",
            "application/"
            "vnd.openxmlformats-officedocument.presentationml.presentation");
  ht_insert(http_mime_map, "prc", "application/x-pilot");
  ht_insert(http_mime_map, "ps", "application/postscript");
  ht_insert(http_mime_map, "ra", "audio/x-realaudio");
  ht_insert(http_mime_map, "rar", "application/vnd.rar");
  ht_insert(http_mime_map, "rdf", "application/rdf+xml");
  ht_insert(http_mime_map, "rpm", "application/x-redhat-package-manager");
  ht_insert(http_mime_map, "rss", "application/rss+xml");
  ht_insert(http_mime_map, "rtf", "application/rtf");
  ht_insert(http_mime_map, "run", "application/x-makeself");
  ht_insert(http_mime_map, "sea", "application/x-sea");
  ht_insert(http_mime_map, "shtml", "text/html");
  ht_insert(http_mime_map, "sit", "application/x-stuffit");
  ht_insert(http_mime_map, "spx", "audio/ogg");
  ht_insert(http_mime_map, "svg", "image/svg+xml");
  ht_insert(http_mime_map, "svgz", "image/svg+xml");
  ht_insert(http_mime_map, "swf", "application/x-shockwave-flash");
  ht_insert(http_mime_map, "tcl", "application/x-tcl");
  ht_insert(http_mime_map, "tif", "image/tiff");
  ht_insert(http_mime_map, "tiff", "image/tiff");
  ht_insert(http_mime_map, "tk", "application/x-tcl");
  ht_insert(http_mime_map, "ts", "video/mp2t");
  ht_insert(http_mime_map, "ttc", "font/collection");
  ht_insert(http_mime_map, "ttf", "font/ttf");
  ht_insert(http_mime_map, "ttl", "text/turtle");
  ht_insert(http_mime_map, "txt", "text/plain");
  ht_insert(http_mime_map, "udeb", "application/vnd.debian.binary-package");
  ht_insert(http_mime_map, "usdz", "model/vnd.pixar.usd");
  ht_insert(http_mime_map, "vtt", "text/vtt");
  ht_insert(http_mime_map, "war", "application/java-archive");
  ht_insert(http_mime_map, "wasm", "application/wasm");
  ht_insert(http_mime_map, "wbmp", "image/vnd.wap.wbmp");
  ht_insert(http_mime_map, "webm", "video/webm");
  ht_insert(http_mime_map, "webp", "image/webp");
  ht_insert(http_mime_map, "wml", "text/vnd.wap.wml");
  ht_insert(http_mime_map, "wmlc", "application/vnd.wap.wmlc");
  ht_insert(http_mime_map, "wmv", "video/x-ms-wmv");
  ht_insert(http_mime_map, "woff", "font/woff");
  ht_insert(http_mime_map, "woff2", "font/woff2");
  ht_insert(http_mime_map, "xht", "application/xhtml+xml");
  ht_insert(http_mime_map, "xhtml", "application/xhtml+xml");
  ht_insert(http_mime_map, "xls", "application/vnd.ms-excel");
  ht_insert(
      http_mime_map, "xlsx",
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
  ht_insert(http_mime_map, "xml", "text/xml");
  ht_insert(http_mime_map, "xpi", "application/x-xpinstall");
  ht_insert(http_mime_map, "xspf", "application/xspf+xml");
  ht_insert(http_mime_map, "zip", "application/zip");
  ht_insert(http_mime_map, "zst", "application/zstd");
}
```
