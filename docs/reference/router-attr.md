# Router Attributes APIs

## router_attr*

`router_attr*` is an attributes object for storing configurations and handlers used by a `http_router*` instance.

## router_attr_init

```c
router_attr *router_attr_init();
```

`router_attr_init` initializes a new router attributes object. This stores data such as the 404, 405, and 500 handlers as well as any middleware set by the user.
For more details, see [the middleware reference](./middleware.md).