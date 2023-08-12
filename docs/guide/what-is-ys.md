# What is Ys?

Ys is a modern, minimal web application framework for the C programming language. With Ys, you can bootstrap functional APIs with minimal dev effort using a rich feature-set of high-level utilities.

## At a Glance

- [Routing and Middleware](../documentation/routing.md)
- [TLS support](../documentation/https-support.md)
- [Logging](../documentation/configuration-and-logging.md)
- [Request-level metadata](../reference/request.md)
- [Zero-config CORS](../documentation/cors.md)
- [Cookies](../documentation/cookies.md)

```c
#include "libys.h"
// ...

ys_response *handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");

  ys_cookie *c = ys_cookie_init(COOKIE_ID, sid);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(10));
  ys_set_cookie(res, c);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_use_cors(attr, ys_cors_allow_all());
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(router, PORT);
  ys_server_start(server);

  return EXIT_SUCCESS;
}
```

## Why?

C programs tend to be verbose. You seldom see C servers in the wild because the development overhead is absolutely massive and mired in complexity; before you can think about building an API, you'll need to sift through system calls, parse raw requests, juggle socket descriptors, and become intimately familiar with the HTTP specification.

Shipped with a robust feature-set inspired by the Express JavaScript framework, Ys provides the building blocks you'll need for most use-cases and takes care of the complexity so you can focus on what matters — building your application.

## How Do I Pronounce That Again?

Ys is pronounced *ē·s*.

## What Ys is *Not* For

- **Ys is not designed for building enterprise-grade servers.**

  If you plan on launching your next product, you probably don't want to be building it in C and moreover Ys. While we aim to carry Ys to a production-ready state, we don't recommend it for your business needs.

- **Ys is performant but only insofar as C itself is performant.**

  We prioritize developer experience and ease-of-use over ever-optimized performance.
