# Routing and Middleware

Under the hood, the Ys router is implemented as a trie data structure. When you register routes, the corresponding route records are inserted into the trie.

When a request is received, it is parsed into a `ys_request*` object and a trie search is performed using the request path.

If the trie search yields no result, the 404 handler is invoked. If the user does not attach a custom 404 handler to the router, the default handler will be used (likewise for 405 and 500 handling).

If the trie search *does* yield a result, but the request method does not match any request method registered in the route record, the 405 handler is invoked.

Finally, if the trie search yields a result and the request method is valid, the matching route handler is invoked.


You can use middleware to pre-empt these handlers. Middlewares are invoked in a LIFO fashion, with the actual route or fallback handler executed last.

This means middleware handlers have an opportunity to intercept route handlers and perform tasks such as authorization, apply a CORS policy, etc. In order to stop a request handler chain, you can set the `done` flag on the `ys_response*` with `ys_set_done`. If a middleware handler sets the `done` flag, no subsequent handlers will be invoked; instead, the response will immediately be serialized and sent back to the client.


## Initializing Router Attributes

```c{4}
#include "libys.h"

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  // ...
}
```

## Initializing a Router

```c{5}
#include "libys.h"

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  // ...
}
```

## Registering a Route Handler

Route handlers are functions that implement the `ys_route_handler*` interface, then registered as either route record handlers or middleware. Route handlers and middleware are executed as part of a LIFO chain.

When invoked, each route handler is passed the `ys_request*` and a pre-initialized `ys_response*`. While the `ys_request*` is immutable, the `ys_response*` may be modified to customize what will eventually be sent to the client.

Each route handler should return the response so it may be passed to the next handler.

```c{3-6,12}
#include "libys.h"

ys_response* handler(ys_request* req, ys_response* res) {
  // ...
  return res;
}

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET, YS_METHOD_POST);

  // ...
}
```

Here, `handler` will be invoked for any `GET` or `POST` request at `/`. Non-route matches will trigger the 404 handler. Route matches with a non-registered HTTP method will trigger the 405 handler. For erroneous or invalid requests, the route will be diverted to the 500 handler. See [Registering Custom Fallback Handlers](#registering-custom-fallback-handlers).


## Registering a Parameterized Route Handler
```c{3-6,14}
#include "libys.h"

ys_response* handler(ys_request* req, ys_response* res) {
  char *id = ys_req_get_parameter(req, "id");
  if (strcmp(id, "12") == 0) { //... }
  // ...
  return res;
}

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/:id[^\\d+$]", handler, YS_METHOD_GET);

  // ...
}
```

Here, `handler` will be invoked for any `GET` request at `/`, followed by a digit e.g. `/12`. The syntax is `:<parameter_name>[<regex>]`. In the above example, we've named our route parameter `id`. In the corresponding route handler, we retrieve the `id` using `ys_req_get_parameter`.

## Retrieving Request Query Data

Similar to request parameters, Ys will automatically parse URL queries and make them available in the route handler.

For example, if we receive a request `/api?username=the_user&password=the_password`, we can retrieve the query data using `ys_req_get_query`. Because URL query keys may have multiple values, the result of `ys_req_get_query` will be a `char*` array of values, or `NULL` if no values matched for the query key.

You can use `ys_req_has_query` or `ys_req_num_queries` to determine whether there was a result and the number of values for the query key, respectively.

```c{2,4}
ys_response *handler(ys_request *req, ys_response *res) {
  char **usernames = ys_req_get_query(req, "username");

  if (ys_req_has_query(usernames) && strcmp(usernames[0], "the_user") == 0) {
    // ...
  }
  // ...
}
```

::: warning NOTE
URL queries are not matched during routing. When matching a request path, anything subsequent to the first `?` is ignored, as it is not part of the request path. Thus, `/?key=value` and `/` will both match routes registered at `/`.
:::

## Registering Multiple Routes

```c{3-6,8-11,17-18}
#include "libys.h"

ys_response* get_handler(ys_request* req, ys_response* res) {
  // ...
  return res;
}

ys_response* post_handler(ys_request* req, ys_response* res) {
  // ...
  return res;
}

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET);
  ys_router_register(router, "/", handler, YS_METHOD_POST);

  // ...
}
```

Here, we've registered two separate route handlers at `/` — one for `GET` requests, and one for `POST` requests.

## Registering Custom Fallback Handlers

```c{8-12,16}
#include "libys.h"

ys_response* handler(ys_request* req, ys_response* res) {
  // ...
  return res;
}

ys_response* not_found_handler(ys_request* req, ys_response* res) {
  // Do stuff...
  ys_set_status(res, YS_STATUS_NOT_FOUND);
  return res;
}

int main () {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router_register_404_handler(attr, not_found_handler);

  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET);
  // ...
}
```

Using `ys_router_register_404_handler`, we can set a custom 404 handler that will be invoked any time we receive a request that does not match a route path. 404 handling supersedes 405 handling, so a request `POST /hello` in this example will trigger 404, not 405.

Similarly, use `ys_router_register_405_handler` for 405 handling and `ys_router_register_500_handler` for 500 handling.

## Nested Routing

Ys supports nested routing, allowing you to encapsulate route logic in a specific router instance. To initialize a sub-router, use the `ys_router_register_sub` function and bind the sub-router to the root router instance.

```c{6-8}
int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  ys_router *api_router = ys_router_register_sub(router, attr, "/api");
  ys_router_register(api_router, "/", api_handler, YS_METHOD_GET);
  ys_router_register(api_router, "/demo", demo_handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);
}
```

With this example configuration, calls to `/api` and `/api/demo` will be handled by the `api_router`; meanwhile, calls to `/` will be directed to the root router `router`.

## Registering Middleware

There are two ways to register middleware on a router. The first way we'll look at collects multiple middlewares and registers them at once. Recall that middlewares will be executed in a LIFO fashion, followed finally by the route handler.

```c{7-10,12-15,19}
// ...
ys_response *handler(ys_request *req, ys_response *res) {
  printf("handler\n");
  return res;
}

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
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET);
  // ...
}
```

Making a `GET` request to `/`, the following will be printed to stdout:

```sh
middleware 2
middleware 1
handler
```

::: warning NOTE
Middlewares are specific to the `ys_router_attr*` on which they've been registered. If you pass a different `ys_router_attr*` object to a sub-router, it will not inherit any middlewares applied to the root router.
:::

## Using CORS

[See CORS](./cors.md).
