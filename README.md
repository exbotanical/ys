# Ys

Ys is a modern, minimal web application framework for the C programming language. With Ys, you can bootstrap functional APIs with minimal dev effort using a rich feature-set of high-level utilities.

[Read the docs](https://exbotanical.github.io/ys)

## A Glimpse
With Ys you can do something like this

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
