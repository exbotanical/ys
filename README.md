# Ys

Ys is a modern, minimal web application framework for the C programming language. With Ys, you can bootstrap functional APIs with minimal dev effort using a rich feature-set of high-level utilities.

[Read the docs](https://exbotanical.github.io/ys)

## A Glimpse
With Ys you can do something like this

```c
#include "libys.h"
// ...

response *handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  cookie *c = cookie_init(COOKIE_ID, sid);
  cookie_set_expires(c, n_minutes_from_now(10));
  set_cookie(res, c);

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  use_cors(attr, cors_allow_all());
  http_router *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET);

  tcp_server *server = server_init(router, PORT);
  server_start(server);

  return EXIT_SUCCESS;
}
```
