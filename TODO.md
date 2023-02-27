# TODOs

- [x] Clean up all objects w/ free where necessary
- [x] Validate NULLs and allocated objects
- [x] Add logging
- [ ] Add global config (e.g. global HTTP headers)
- [ ] HTTP request parser
- [ ] test handlers with a mocking library
- [ ] static
- [ ] logging
  - [ ] syslog
- [ ] error-handling
- [ ] documentation
- [ ] use attr struct for fallback handlers
- [ ] Figure out file routing. The problem here is we want the `response_t` type to be accessible to consumers without having to compile the entire library and link it.
