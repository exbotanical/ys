# TODOs

## Docs
- [x] replace - with em dash —_— —v— —^— —7—
- [x] name header file
- [ ] ensure example + docs code runs
- [ ] Use Fathom analytics
- [x] github workflows

## Pre-release
- [x] SSL support
- [x] Cookies
- [x] Name this project: Ys?
- [x] Lock deps versions
- [x] Document
- [x] Filter out test deps in builds

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
- [x] prefix all enums with `YS_METHOD_` or `YS_STATUS_`
- [x] handle duplicate request headers
  - Go style: append subsequent values, but `Get` retrieves the first only. `Values` returns list of all. In response, each duplicate header is written separately
- [x] xmalloc that dies

## Perf
- [ ] Increase to 15k TPS (As of 06/03/2023, Ys supports ~5k TPS. We want to improve this 3x to ~15k)

## Testing
- [x] ~~test handlers with a mocking library?~~ no need w/ integ
- [ ] test with a real server (E2E)

## UX
- [ ] cross-OS compatibility
  - [x] source strdup from dep
- [ ] review all `include`d functions and replace any non-standard ones
- [x] documentation
- [x] make all public structs opaque
- [x] Use method enum in CORS middleware helpers
- [x] Ensure everything opaque has getters and setters
- [x] Deny list paths for middleware
  - [ ] Store URL in struct (if needed)
  - [x] Allow the deny list to use regex
  - [x] Handle paths with params etc
- [x] middleware ordering - before err handlers?
- [ ] ask users - should CORS be inherited by sub-routers automatically? try an `inherit` mode
- [ ] Non-param path regex
- [x] Offer Content-Types
- [ ] Use hash set for headers to de-duplicate
- [x] Comma-delimit duplicate response headers
- [x] Use user-defined content length header OR overwrite it - but handle it
- [x] fmt string in `ys_set_body`
- [x] Default Content-Type to text/plain?
- [x] Remove the need for null-terminator everywhere else (incl `ys_router_register`)
- [x] set default status on registered fallback handlers with no `ys_set_status` call


## New Features
- [x] ~~global hooks/interceptors (e.g. global HTTP headers)~~ just use middleware
- [x] allow middleware to exit early by sending response
- [x] Built-in middlewares
  - [x] CORS middleware
- [x] Cookies
- [x] support erroneous (Request / Response)s
- [x] use attr struct for fallback handlers
- [x] ~~Figure out file routing. The problem here is we want the `response_t` type to be accessible to consumers without having to compile the entire library and link it.~~ Make this an addon
- [x] SSL/TLS support
- [x] JSON parsed body in context (decided to offer JSON opt-in utilities)
- [ ] async i/o option using epoll
  - [ ] Fully evented server e.g. `onRequest`
- [x] global middlewares
- [x] multiple routers
- [x] handle signals for graceful shutdown
- [x] query strings
- [x] ~~wildcard routing~~ just use middleware
- [ ] serve entire dir
- [x] ~~set X-Powered-By~~ nah, better to be less invasive

## Best Practices
- [x] ~~specific int types~~
- [x] check all instances of `strcmp` and see if `strncmp` makes more sense
- [ ] Determine where we actually need to be passing by reference (currently, this is done everywhere)
- [x] ~~Add LICENSE of yyjson~~
- [ ] Pass correct function pointers see:
  * https://stackoverflow.com/questions/14134245/iso-c-void-and-function-pointers
  * https://stackoverflow.com/questions/5579835/c-function-pointer-casting-to-void-pointer/5579907#5579907
  * https://stackoverflow.com/questions/36645660/why-cant-i-cast-a-function-pointer-to-void

- [ ] `const` pointers
  - [ ] `const` in structs
- [x] fix `const char CONSTANT[n],m`
- [x] `free` everywhere we can
- [ ] mutexes in examples

## Spec Compliance
- [ ] handle duplicate response headers
- [ ] Full RFC compliance
  - [ ] handle accept header
  - [ ] Validate Content-Type for incoming stateful requests
  - [ ] keep-alive
  - [ ] cache-control
  - [ ] transfer-encoding
- [x] Test URL fragments
- [x] URL queries

## Misc Improvements
- [x] ~~use a better regex engine than pcre~~ it's fine
- [x] ~~make regex cache global (instead of per-trie-node)~~ bad idea
- [x] replace `LOG` macro calls with new logger
  - [x] fix log to stderr
  - [x] use function name macro instead of explicit string literals
- [x] ~~inline functions where reasonable~~
- [x] ~~Change middleware chaining to use `next`, the next middleware in the chain. This will allow a current handler to *not* invoke `next`, thus ending the chain. Note: Figuring this out is going to be a challenge~~
- [x] relocate JSON to util library

## DX
- [x] ~~helper ??? `#define CAST(type, name, stmt) type *name = (type *)stmt`~~
- [x] Setup linter that uses Google's C/C++ style guide
- [x] `@return` doc for `true` and `false` cases instead of just `bool`
- [x] pick a style for struct types
- [x] Debug helpers e.g.
  ```c
  static int c = 1;
  #define checkpoint() printf("CHECKPOINT %d\n", c++)
  #define isnull(thing) printf("IS NULL? %s\n", thing == NULL ? "yes" : "no")
  ```
- [x] Shared test utils
- [x] Consolidate integ test servers
- [x] get rid of doc comments with `@param` - it offers no value since we're not generating anything with the doc comments and is difficult to maintain
- [x] cleanup tests and use unified framework so we have full test count in aggregate, can filter tests, and have a reporter
- [ ] Remove localhost SSL certs and add a script for developers to generate their own

## Fixes
- [x] Allow OPTIONS requests by default in router when CORS enabled
- [x] Urls with `-` segfault e.g. `/feed_meta/index-0.json`
  - [ ] Add test
- [x] ~~curl - Connection #0 to host localhost left intact?~~ (see https://stackoverflow.com/a/72045706)
- [x] Address glibc malloc bug in ip
- [x] ~~Do two route handlers with diff methods for the same /route get handled separately?~~ Yes, it's per route/method. With GET / handlera and GET / handlerb, the most-recently registered will overwrite.
- [x] Does router handle URL fragments?
  - [x] Failed curl localhost:9000/?cookie
  - [x] Failed curl 'localhost:9000/?cookie=1&value=1'
- [x] Why does http req hang when using TLS?
- [x] Don't fallthrough to route handler on OPTIONS req
  - [x] Middleware should run before the fallback handlers and routing - that will fix this.

## Ecosystem
- [ ] Addon / plugin libraries
  - [ ] Metrics
  - [ ] Testing (stubs, mock calls etc) (req, res)
  - [ ] User-logger that serializes opaque objects
