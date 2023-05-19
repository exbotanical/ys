# Router Attributes APIs

## ys_router_attr*

`ys_router_attr*` is an attributes object for storing configurations and handlers used by a `ys_router*` instance.

## ys_router_attr_init

```c
ys_router_attr *ys_router_attr_init(void);
```

`ys_router_attr_init` initializes a new router attributes object. This stores data such as the 404, 405, and 500 handlers as well as any middleware set by the user.
For more details, see [the middleware reference](./middleware.md).

## ys_router_register_404_handler

```c
void ys_router_register_404_handler(ys_router_attr *attr, ys_route_handler *h);
```

`ys_router_register_404_handler` registers a custom 404 handler that will be used whenever a route path is not found.

If you do not set a status in this handler, it will be defaulted to 404.

## ys_router_register_405_handler

```c
void ys_router_register_405_handler(ys_router_attr *attr, ys_route_handler *h);
```

`ys_router_register_405_handler` registers a custom 405 handler that will be used whenever a route path is not found.

If you do not set a status in this handler, it will be defaulted to 405.

## ys_router_register_500_handler

```c
void ys_router_register_500_handler(ys_router_attr *attr, ys_route_handler *h);
```

`ys_router_register_500_handler` registers a custom 405 handler that will be used whenever a route path is not found.

If you do not set a status in this handler, it will be defaulted to 500.
