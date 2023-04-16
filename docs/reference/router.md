# Router APIs

## http_router*

A `http_router*` is a request router / HTTP multiplexer.

## router_init

```c
http_router *router_init(router_attr *attr);
```

`router_init` allocates memory for a new router and initializes its members. It uses the `router_attr*` to set the 404, 405, and 500 handlers if set, as well as any registered middleware.

## router_register

```c
void router_register(http_router *router, const char *path,
                     route_handler *handler, http_method method, ...);
```


`router_register` registers a new route record. Registered routes will be matched against incoming requests. The list of HTTP methods must be `NULL`-terminated.

## router_free

```c
void router_free(http_router *router);
```

`router_free` deallocates memory for the provided `http_router*` instance.

## router_register_sub

```c
http_router *router_register_sub(http_router *parent_router, router_attr *attr, const char *subpath);
```

`router_register_sub` registers a sub-router on `parent_router` at `subpath`.

For example, suppose we have a root router

```c
http_router *router = router_init(router_attr *attr);
```

Requests will always be matched using this router instance. However, if we register a sub-router at `/api`...

```c{3-6}
http_router *router = router_init(router_attr *attr);

http_router *api_router = router_register_sub(router, attr, "/api");
router_register(api_router, "/", api_handler, METHOD_GET, NULL);
router_register(api_router, "/demo", demo_handler, METHOD_GET, NULL);
```

...all requests to `api_router` will be relative to that sub-router's root path `/api`. Thus, `/` matches on `/api`, and `/demo` matches on `/api/demo`.
