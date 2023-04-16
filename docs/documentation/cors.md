# CORS
<!-- TODO: PRERELEASE FIX NEEDED CORS wiki link -->
Ys offers full support for CORS. Ys' CORS module is implemented as a middleware that can be bound to a router instance.

The CORS middleware will handle Preflight requests and apply the specified CORS policy automatically.

To quickly initialize CORS middleware with sensible defaults, use the `cors_allow_all` utility.
<!-- TODO: PRERELEASE FIX NEEDED explain settings -->
```c
router_attr *attr = router_attr_init();
use_cors(attr, cors_allow_all());
```

To build your own CORS policy, use `cors_opts_init` to initialize a new CORS options object, then use the `cors_set_*` utilities to customize it.
<!-- TODO: PRERELEASE FIX NEEDED set_ -> cors_* -->

```c
cors_opts *opts = cors_opts_init();

set_allowed_methods(opts, METHOD_GET, METHOD_DELETE);
set_allowed_headers(opts, "X-Test-Header");
set_expose_headers(opts, "X-Powered-By");
set_allowed_origins(opts, "test.com");
```

Finally, we set the `cors_opts*` on the router attributes object.

```c
router_attr *attr = router_attr_init();
use_cors(attr, cors);
```

See a full accounting of the available CORS APIs [here](../reference/cors.md).
