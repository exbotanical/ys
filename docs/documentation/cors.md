# CORS
Ys offers full support for [CORS](https://en.wikipedia.org/wiki/Cross-origin_resource_sharing). Ys' CORS module is implemented as a middleware that can be bound to a router instance.

The CORS middleware will handle Preflight requests and apply the specified CORS policy automatically.

To quickly initialize CORS middleware with sensible defaults, use the `cors_allow_all` utility.

```c
router_attr *attr = router_attr_init();
use_cors(attr, cors_allow_all());
```

`cors_allow_all` configures the following CORS policy

|property|value|
|-|-|
|allowed origins|*|
|allowed methods|HEAD, GET, POST, PATCH, PUT, DELETE|
|allowed headers|*|
|allow credentials|false|
|max age|0|
|exposed headers|none|
|use options passthrough|false|

::: info NOTE
You may use a wildcard (`*`) to set the allowed headers or origins to allow all values.
:::

To build your own CORS policy, use `cors_opts_init` to initialize a new CORS options object, then use the `cors_set_*` utilities to customize it.

```c
cors_opts *opts = cors_opts_init();

cors_allow_methods(opts, METHOD_GET, METHOD_DELETE);
cors_allow_headers(opts, "X-Test-Header");
cors_expose_headers(opts, "X-Powered-By");
cors_allow_origins(opts, "test.com");
```

Finally, we set the `cors_opts*` on the router attributes object.

```c
router_attr *attr = router_attr_init();
use_cors(attr, cors);
```

See a full accounting of the available CORS APIs [here](../reference/cors.md).
