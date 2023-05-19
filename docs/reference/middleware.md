# Middleware APIs

## ys_use_middlewares

```c
void ys_use_middlewares(ys_router_attr *attr, ys_route_handler *mw, ...);
```

`ys_use_middlewares` binds *n* middleware handlers to the router attributes instance.
You do not need to pass a `NULL` sentinel to terminate the list; the `middleware` macro
will do this for you.

```c
ys_response *middleware1(ys_request *req, ys_response *res) {
  printf("middleware 1\n");

  return res;
}

ys_response *middleware2(ys_request *req, ys_response *res) {
  printf("middleware 2\n");

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_use_middlewares(attr, middleware1, middleware2);

  // ...
}
```

## ys_use_middleware

```c
void ys_use_middleware(ys_router_attr *attr, ys_route_handler *mw);
```

`ys_use_middleware` binds a new middleware to the routes attributes object. Middlewares will be run in a LIFO fashion before the route handler.

## ys_add_middleware_with_opts

```c
void ys_add_middleware_with_opts(ys_router_attr *attr, ys_route_handler *mw, char *ignore_path, ...);
```

`ys_add_middleware_with_opts` binds a new middleware — along with ignore
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
