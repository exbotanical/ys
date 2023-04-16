# Middleware APIs

## use_middlewares

```c
void use_middlewares(router_attr *attr, route_handler *mw, ...);
```

`use_middlewares` binds *n* middleware handlers to the router attributes instance.
You do not need to pass a `NULL` sentinel to terminate the list; the `middleware` macro
will do this for you.

```c
response *middleware1(request *req, response *res) {
  printf("middleware 1\n");

  return res;
}

response *middleware2(request *req, response *res) {
  printf("middleware 2\n");

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  use_middlewares(attr, middleware1, middleware2);

  // ...
}
```

## use_middleware

```c
void use_middleware(router_attr *attr, route_handler *mw);
```

`use_middleware` binds a new middleware to the routes attributes object. Middlewares will be run in a LIFO fashion before the route handler.

## add_middleware_with_opts

```c
void add_middleware_with_opts(router_attr *attr, route_handler *mw, char *ignore_path, ...);
```

`add_middleware_with_opts` binds a new middleware — along with ignore
paths — to the routes attributes object, where `ignore_paths` is a list of paths or regular expressions on which the middleware should be ignored.
That is, when handling a request, if the request path matches one of the ignore paths for a middleware, that
middleware will not be invoked. Think of ignore paths as a disallow-list.

Ignore paths are compared in full, even for sub-routers. If the request is
`/api/path` and a sub-router matches on `/api` (making the `route_path` `/path`),
`/api/path` is compared to the ignore paths.

Examples:

```sh
ignore_paths = ["/internal", "/internal/app", "/private/request"]

request->path = "/demo" -> middleware runs
request->path = "/internal" -> middleware does not run
request->path = "/internal/app" -> middleware does not run
request->path = "/internal/request" -> middleware runs
request->path = "/private" -> middleware runs
request->path = "/private/request" -> middleware does not run
```

The list of ignore paths does not need to be sentinel-terminated.
