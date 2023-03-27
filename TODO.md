# TODOs

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

## New Features
- [ ] global hooks/interceptors (e.g. global HTTP headers)
- [x] allow middleware to exit early by sending response
- [x] Built-in middlewares
  - [x] CORS middleware
- [ ] Cookies
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
- [ ] query strings

## Best Practices
- [x] ~~specific int types~~
- [ ] check all instances of `strcmp` and see if `strncmp` makes more sense
- [ ] Determine where we actually need to be passing by reference (currently, this is done everywhere)

## Spec Compliance
- [ ] handle duplicate response headers
- [ ] Full RFC compliance
  - [ ] handle accept header
  - [ ] Validate Content-Type for incoming stateful requests
  - [ ] keep-alive
  - [ ] cache-control
  - [ ] transfer-encoding
- [ ] Test URL fragments
- [ ] URL queries

## Misc Improvements
- [ ] use a better regex engine than pcre
- [ ] ~~make regex cache global (instead of per-trie-node)~~ bad idea
- [x] replace `LOG` macro calls with new logger
  - [x] fix log to stderr
  - [x] use function name macro instead of explicit string literals
- [ ] inline functions where reasonable
- [x] ~~Change middleware chaining to use `next`, the next middleware in the chain. This will allow a current handler to *not* invoke `next`, thus ending the chain. Note: Figuring this out is going to be a challenge~~

## DX
- [ ] helper ??? `#define CAST(type, name, stmt) type *name = (type *)stmt`
- [x] Setup linter that uses Google's C/C++ style guide
- [ ] `@return` doc for `true` and `false` cases instead of just `bool`
- [ ] pick a style for struct types

## Fixes
- [x] Allow OPTIONS requests by default in router when CORS enabled
- [ ] Urls with `-` segfault e.g. `/feed_meta/index-0.json`

# Style Guide
- Use [Go Doc-style](https://tip.golang.org/doc/comment) for doc  comments e.g. `// function_name does thing`

- comment what is used from an include header where possible

# Desired API
- Router
  - `router_init(router_attr_t*)`
  - `router_set_400_handler(router_attr_t*, handler_t*)`
  - `router_set_400_handler(router_attr_t*, handler_t*)`
  - `router_set_405_handler(router_attr_t*, handler_t*)`
  - `router_set_500_handler(router_attr_t*, handler_t*)`
  - `router_register`
- Server
  - `server_init(router_t*, int)`
  - `server_start(server_t*)`
  - `server_free(server_t*)`
- Request
  - `get_request_header(req_t*, const char*)`
- Response
  - `set_response_header(req_t*, const char*, const char*)`
  - `insert_response_header(req_t*, const char*, const char*)`
