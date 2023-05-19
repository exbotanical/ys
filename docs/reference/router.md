# Router APIs

## ys_router*

A `ys_router*` is a request router / HTTP multiplexer.

## ys_router_init

```c
ys_router *ys_router_init(ys_router_attr *attr);
```

`ys_router_init` allocates memory for a new router and initializes its members. It uses the `ys_router_attr*` to set the 404, 405, and 500 handlers if set, as well as any registered middleware.

## ys_router_register

```c
void ys_router_register(ys_router *router, const char *path,
                     ys_route_handler *handler, ys_http_method method, ...);
```


`ys_router_register` registers a new route record. Registered routes will be matched against incoming requests. The list of HTTP methods must be `NULL`-terminated.

## ys_router_free

```c
void ys_router_free(ys_router *router);
```

`ys_router_free` deallocates memory for the provided `ys_router*` instance.

## ys_router_register_sub

```c
ys_router *ys_router_register_sub(ys_router *parent_router, ys_router_attr *attr, const char *subpath);
```

`ys_router_register_sub` registers a sub-router on `parent_router` at `subpath`.

For example, suppose we have a root router

```c
ys_router *router = ys_router_init(ys_router_attr *attr);
```

Requests will always be matched using this router instance. However, if we register a sub-router at `/api`...

```c{3-6}
ys_router *router = ys_router_init(ys_router_attr *attr);

ys_router *api_router = ys_router_register_sub(router, attr, "/api");
ys_router_register(api_router, "/", api_handler, YS_METHOD_GET);
ys_router_register(api_router, "/demo", demo_handler, YS_METHOD_GET);
```

...all requests to `api_router` will be relative to that sub-router's root path `/api`. Thus, `/` matches on `/api`, and `/demo` matches on `/api/demo`.
