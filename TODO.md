# TODOs

- [x] Clean up all objects w/ free where necessary
- [x] Validate NULLs and allocated objects
- [x] Add logging
- [ ] Add global config (e.g. global HTTP headers)
- [ ] HTTP request parser
- [ ] test handlers with a mocking library
- [x] static
- [x] logging
- [x] error-handling
- [ ] documentation
- [ ] use attr struct for fallback handlers
- [ ] Figure out file routing. The problem here is we want the `response_t` type to be accessible to consumers without having to compile the entire library and link it.
- [ ] SSL support
- [ ] properly parse entire request (we're using a placeholder method right now!)
- [ ] Use a union to store parameter values. We have enough info from the regex to determine whether the value should be an int or char*
- [ ] JSON parsed body in context
- [x] pass a struct with context, req, res (maybe merge ctx and req)
- [ ] helper ??? `#define CAST(type, name, stmt) type *name = (type *)stmt`
- [ ] async i/o option using epoll
- [ ] inline functions where reasonable
- [ ] integ tests
- [ ] test with a real server
- [ ] read entire request in chunks
- [ ] send entire response in chunks
- [ ] Evented server e.g. `onRequest`
- [x] fix request hangups
  - [ ] mostly fixed, happens when calling with a not allowed/found method w/body `Could not resolve host: <body>`
- [ ] Built-in middlewares
  - [ ] CORS middleware
- [ ] Cookies
- [ ] Validate Content-Type for incoming stateful requests
- [ ] support erroneous (Request / Response)s
- [ ] check all instances of `strcmp` and see if `strncmp` makes more sense
- [ ] handle accept header


## Style Guide
- Use [Go Doc-style](https://tip.golang.org/doc/comment) for doc  comments e.g. `// function_name does thing`

- comment what is used from a header where possible
