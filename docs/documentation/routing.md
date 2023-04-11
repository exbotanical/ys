# Routing and Middleware

Under the hood, a router in Ys is implemented as a trie data structure. When you register routes, the corresponding route records are inserted into the trie.

When a request is received, a thread is dispatched from the thread pool. The request is parsed into a `request*` object and a trie search is performed using the request path.

If the trie search yields no result, the 404 handler is invoked. If the user does not attach one to the `router_attr*` used to initialize the router, the default handler will be used (likewise for 405 and 500 handling).

If the trie search does yield a result, but the request method does not match any request method registered in the route record, the 405 handler is invoked.

Finally, if the trie search yields a result and the request method is valid, the matching route handler is invoked, but not before any registered middlewares. Middlewares are invoked in a LIFO fashion, with the actual route handler executed last.

This means middleware handlers have an opportunity to intercept route handlers and perform tasks such as authorization, apply a CORS policy, etc. In order to stop a request handler chain, you can set the `done` flag on the `response*` with `set_done`. If a middleware handler sets the `done` flag, no subsequent handlers will be invoked; instead, the response will be serialized and sent back to the client.


## Initializing Router Attributes

```c{4}
#include "libys.h"

int main () {
  router_attr *attr = router_attr_init();
  // ...
}
```

## Initializing a Router

```c{5}
#include "libys.h"

int main () {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);
  // ...
}
```

## Registering a Route Handler

Route handlers are functions that implement the `route_handler*` interface, then registered as either route record handlers or middleware. Route handlers and middleware are executed as part of a LIFO chain.

When invoked, each route handler is passed the `request*` and a pre-initialized `response*`. While the `request*` should be treated as immutable, the `response*` may be modified to craft what will eventually be sent to the client.

Each route handler should return the response so it may be passed to the next handler.

```c{3-6,12}
#include "libys.h"

response* handler(request* req, response* res) {
  // ...
  return res;
}

int main () {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET, METHOD_POST, NULL);

  // ...
}
```

Here, `handler` will be invoked for any `GET` or `POST` request at `/`. Non-route matches will trigger the 404 handler. Route matches with an un-registered HTTP method will trigger the 405 handler. Errors will be diverted into the 500 handler. [TODO link to handlers]


## Registering a Parameterized Route Handler
```c{3-6,12}
#include "libys.h"

response* handler(request* req, response* res) {
  char *id = req_get_parameter(req, "id");
  if (strcmp(id, "12") == 0) { //... }
  // ...
  return res;
}

int main () {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/:id[^\\d+$]", handler, METHOD_GET, NULL);

  // ...
}
```

Here, `handler` will be invoked for any `GET` request at `/`, followed by a digit e.g. `/12`. The syntax is `:<parameter_name>[<regex>]`. In the above example, we've named our route parameter `id`. In the corresponding route handler, we retrieve the `id` using `req_get_parameter`.

## Retrieving Request Query Data

Similar to request parameters, Ys will automatically parse URL queries and make them available in the route handler.

For example, if we receive a request `/api?username=the_user&password=the_password`, we can retrieve the query data using `req_get_query`. Because URL query keys may have multiple values, the result of `req_get_query` will be a `char*` array of values, or `NULL` if no values matched for the query key.

You can use `req_has_query` or `req_num_queries` to determine whether there was a result and the number of values for the query key, respectively.

```c{2,4}
response *handler(request *req, response *res) {
  char **usernames = req_get_query(req, "username");

  if (req_has_query(usernames) && strcmp(usernames[0], "the_user") == 0) {
    // ...
  }
  // ...
}
```

::: warning NOTE
URL queries are not matched during routing. When matching a request path, anything subsequent to the first `?` is ignored, as it is not part of the request path. Thus, `/?key=value` and `/` will both match routes registered at `/`.
:::

## Registering Multiple Routes

```c{3-6,12}
#include "libys.h"

response* get_handler(request* req, response* res) {
  // ...
  return res;
}

response* post_handler(request* req, response* res) {
  // ...
  return res;
}

int main () {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET, NULL);
  router_register(router, "/", handler, METHOD_POST, NULL);

  // ...
}
```

Here, we've registered two separate route handlers at `/` - one for `GET` requests, and one for `POST` requests.

## Registering a Custom Fallback Handlers

```c{3-6,12}
#include "libys.h"

response* handler(request* req, response* res) {
  // ...
  return res;
}

response* not_found_handler(request* req, response* res) {
  // Do stuff...
  set_status(res, STATUS_NOT_FOUND);
  return res;
}

int main () {
  router_attr *attr = router_attr_init();
  router_register_404_handler(attr, not_found_handler);

  http_router *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET, NULL);
  // ...
}
```

Using `router_register_404_handler`, we can set a custom 404 handler that will be invoked any time we receive a request that does not match a route path. 404 handling supersedes 405 handling, so a request `POST /hello` in this example will trigger 404, not 405.

Similarly, use `router_register_405_handler` for 405 handling and `router_register_500_handler` for 500 handling.

## Registering Middleware

There are two ways to register middleware on a router. The first way we'll look at collects multiple middlewares and registers them at once. Recall that middlewares will be executed in a LIFO fashion, followed finally by the route handler.

```c
// ...
response *handler(request *req, response *res) {
  printf("handler\n");
  return res;
}

response *middleware1(request *req, response *res) {
  printf("middleware 1\n");
  return res;
}

response *middleware2(request *req, response *res) {
  printf("middleware 2\n");
  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  middlewares(attr, middleware1, middleware2);
  http_router *router = router_init(attr);

  router_register(router, "/", handler, METHOD_GET, NULL);
  // ...
}
```

Making a `GET` request to `/` would result in the following stdout output:

```sh
middleware 2
middleware 1
handler
```

::: warning NOTE
Middlewares are specific to the `router_attr*` on which they've been registered. If you pass a different `router_attr*` object to a sub-router, it will not inherit any middlewares applied to the root router.
:::

## Using CORS

[See CORS](./cors.md).
