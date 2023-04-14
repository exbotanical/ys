# Basic Tutorial

Let's break down the simple server example as seen in the [Getting Started](./getting-started.md) guide.

First we include the Ys header.

```c
#include "libys.h"
```

This is the only header file you'll need to include in order to have access to the full Ys feature-set. All of the constants and APIs in the Ys header file are documented with doc comments so you can access documentation from your IDE.

First, we'll define a `PORT` macro — this will be the port number on which the Ys server will listen for client connections. The port number must be outside of the range of standard reserved ports i.e. it must be greater than 1024 and less than 65535.

```c{3}
#include "libys.h"

#define PORT 9000
```

Let's define our main function and initialize a new router.

```c{5-10}
#include "libys.h"

#define PORT 9000

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  return 0;
}
```

A root router is required for a Ys server. In Ys, a router is an object that handles routing incoming network requests to the appropriate handler function (also known as a multiplexer).

By "root", we mean that the root router matches requests beginning with the path `/`. Later, we'll look at how to register sub-routers that match on specific sub-paths.

On a router, we define routes. Each route corresponds to a specific path (which may be a regular expression), at least one (but potentially many) request methods, and a single handler function which is invoked whenever a request matches that route.

The `http_router` struct accepts as an argument a router attributes object `router_attr`. This attributes stores router-specific information such as middlewares, 404, 405, and 500 handlers, and CORS. You can find more information about how the router attributes object is used in the [Routing Documentation](../documentation/routing.md). For now, we'll initialize it with `router_attr_init` and pass it to the router init function.

Now that we have our root router, let's create our first request handler.

```c{5-7,13}
#include "libys.h"

#define PORT 9000

response *root_handler(request *req, response *res) {
  return res;
}

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  return 0;
}
```

Here, we've registered a request handler `root_handler` on our root router. The second argument to `router_register` is the request path on which the router will match `root_handler`. Any time a request is made to `/`, `root_handler` will be invoked.

The third and subsequent arguments to `router_register` specify the request methods on which to register the handler - here, we've specified `METHOD_GET`. Ys provides several enum constants for all possible HTTP methods and statuses, prefixed with `METHOD_` and `STATUS_`, respectively. Note we terminate the variadic list of methods with a `NULL` terminator, which instructs `router_register` our list has ended. *Failure to pass the `NULL` terminator will result in undefined behavior.*

If we were to run our server (after setting up the server, of course - which we do later in this tutorial) and make several requests with `curl`, here's what we'd see:

|Request|Response Status|
|-|-|
|GET /somepath|404|
|GET /|200|
|POST /|405|

Notice Ys handles 404 Not Found and 405 Method Not Allowed by default. You can learn how to set your own handlers for these situations in the [Routing Documentation](../documentation/routing.md). Because we only registered `root_handler` at `GET /`, sending a `POST` request to `/` will result in a 405. Similarly, un-registered paths yield 404.

Next, let's discuss the `root_handler` function and implement some server logic.

```c{5-7}
#include "libys.h"

#define PORT 9000

response *root_handler(request *req, response *res) {
  return res;
}

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  return 0;
}
```

Currently, `root_handler` does nothing special. All request handlers implement the `router_handler*` type, whose signature is

`response *route_handler(request *, response *)`

Ys uses opaque types and offers getter and setter APIs for reading and writing struct fields to prevent undefined behavior (you can see a full accounting of these APIs in the [API Reference](../reference/)).

Every handler receives the client request and a pre-initialized response object, which you may modify in order to craft the response that will be sent to the requesting client. The response object *must be returned from the handler*. By default, the response status will be set to `200 OK` unless specified otherwise.

Let's look at a few setters that will allow us to craft the HTTP response returned by `root_handler`.


```c{6,8-9}
#include "libys.h"

#define PORT 9000

response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  return 0;
}
```

First, we have the `set_header` function. `set_header` - as its name would suggest - sets a key/value pair on the response's headers. Under the hood, Ys will handle formatting the header and encompassing response into a RFC 7230-compliant format. All we need to do here is set the key and value.

```c{2}
response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}
```

Here, we're setting the `Content-Type` to `text/plain`.

Speaking of RFC 7230, most HTTP headers are allowed to have multiple values corresponding to the same key. Were we to call `set_header` twice with the same key but different values, Ys will use both values in the response.

For example

```c{2-3}
response *root_handler(request *req, response *res) {
  set_header(res, "X-Header", "value-1");
  set_header(res, "X-Header", "value-2");
  // ...
  return res; // X-Header: value-1, value-2
}
```

Next, we set the body of the response using the aptly-named `set_body` function. In Ys, you *do not need to set `Content-Length`* - Ys will compute this header for you when serializing the response by determining the correct size of the response body. If you do not set a body, `Content-Length` will be set to `0` ([see exceptions](../reference/)).

```c{4}
response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}
```

Next, we explicitly set the response status to `200 OK` using the `set_status` function and `STATUS_OK` enum. As aforementioned, the response is set to `200 OK` by default if you do not call `set_status`, but we're including it here for illustrative purposes.

```c{5}
response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}
```

Similarly, if you set a body but do not set a `Content-Type` header, Ys will default the `Content-Type` to `text/plain`.

Finally, we return the response object with our modifications so it can be serialized by Ys and sent back to the client.

```c{7}
response *root_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/plain");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}
```

Now that we've configured our root handler, let's finish setting up the server so we can run it locally. First, we create an attributes object for our server.

```c{7-8}
int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  tcp_server_attr* srv_attr = server_attr_init(router);
  server_set_port(PORT);

  return 0;
}
```

We've added two new lines. The first initializes a new `tcp_server_attr`, a required settings object for our server. It is on this object that we can configure a port (or not - and use the default port: 5000) and certs for HTTPs. The only argument for `server_attr_init` is the root router instance for the server.

We've also overridden the default port to 9000 using the `server_set_port` helper. You can read about the other `tcp_server_attr*` helper methods and convenience functions [here](../reference/server-attr.md).

Now let's add two more lines to create and start the server.

```c{10-11}
int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  tcp_server_attr* srv_attr = server_attr_init(router);
  server_set_port(PORT);

  tcp_server *server = server_init(srv_attr);
  server_start(server);

  return 0;
}
```

The first new line initializes a new `tcp_server` instance. By default, the server will not use SSL (see [HTTPs Support](../documentation/https-support.md)). To initialize a new server, we pass the attributes object we created in the previous step.

Once we have our `tcp_server` instance, we can pass it to `server_start`, which will start the server, configure signal handlers for `SIGINT` and `SIGSEGV` (for controlled shutdowns), and begin listening on port 9000.

Let's compile, run, and try sending a request to our new server!

```sh
# Compile Ys into a statically-linked library
make libys.a
# Compile our server program, linking it to Ys
gcc main.c libys.a -o main
# Run the server program
./main
# In a separate terminal, send a request to the server using curl
curl localhost:9000/
# Hello World!
```
