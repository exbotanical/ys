# Basic Tutorial

Let's break down the simple server example as seen in the [Getting Started](./getting-started.md) guide.

First we include the Ys header.

```c
#include "libys.h"
```

This is the only header file you'll need to include in order to have access to the full Ys feature-set. All of the constants and APIs in the Ys header file are documented with doc comments so you can access documentation from your IDE.

First, we'll define a `PORT` macro — this will be the port number on which the Ys server will listen for incoming connections. The port number must be outside of the range of standard reserved ports i.e. it must be greater than 1024 and less than 65535.

```c{3}
#include "libys.h"

#define PORT 9000
```

Let's define our main function and initialize a new router.

```c{5-10}
#include "libys.h"

#define PORT 9000

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  return 0;
}
```

A root router is required for a Ys server. In Ys, a router (also known as a multiplexer) is an object that routes incoming network requests to the appropriate handler function.

By *root*, we mean that the root router matches requests beginning with the path `/`. Later, we'll look at how to register sub-routers that match on specific sub-paths.

On a router, we define one or more routes. Each route corresponds to a specific path (which may be a regular expression), at least one request method, and a single handler function which is invoked whenever a request matches that route.

The `ys_router` struct accepts as an argument a router attributes object `ys_router_attr`. This object stores router-specific information such as middlewares, 404, 405, and 500 handlers, and CORS. You can find more information about how the router attributes object is used in the [Routing Documentation](../documentation/routing.md). For now, we'll initialize it with `ys_router_attr_init` and pass it to the `ys_router_init` function.

Now that we have our root router, let's create our first request handler.

```c{5-7,13}
#include "libys.h"

#define PORT 9000

ys_response *root_handler(ys_request *req, ys_response *res) {
  return res;
}

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  return 0;
}
```

Here, we've registered a request handler `root_handler` on our root router. The second argument to `ys_router_register` is the request path on which the router will match `root_handler`. Any time a request is made to `/`, `root_handler` will be invoked.

The third and subsequent arguments to `ys_router_register` specify the request methods on which to register the handler — here, we've specified `YS_METHOD_GET`. Note we do not need to terminate the variadic list of methods with a `NULL` sentinel terminator; the `ys_router_register` macro is written such that it is inserted automatically.

::: info TIP
Ys provides several enum constants for all possible HTTP methods and statuses, prefixed with `YS_METHOD_` and `YS_STATUS_`, respectively.
:::

If we were to run our server (after setting up the server, of course — which we do later in this tutorial) and make several requests with `curl`, here's what we'd see:

|Request|Response Status|
|-|-|
|GET /somepath|404|
|GET /|200|
|POST /|405|

Notice Ys handles `404 Not Found` and `405 Method Not Allowed` by default. You can learn how to set your own handlers for these situations in the [Routing Documentation](../documentation/routing.md). Because we only registered `root_handler` at `GET /`, sending a `POST` request to `/` will result in a 405. Similarly, requests to non-registered paths yield 404.

Next, let's discuss the `root_handler` function and implement some server logic.

```c{5-7}
#include "libys.h"

#define PORT 9000

ys_response *root_handler(ys_request *req, ys_response *res) {
  return res;
}

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  return 0;
}
```

Currently, `root_handler` does nothing special. All request handlers implement the `router_handler*` type, whose signature is

`ys_response *ys_route_handler(ys_request *, ys_response *)`

::: info TIP
Ys uses opaque types and offers getter and setter APIs for reading and writing struct fields to prevent undefined behavior (you can see a full accounting of these APIs in the [API Reference](../reference/)).
:::

Every handler receives the client request and a pre-initialized response object, which you may modify in order to customize the response that will be sent back to the client. The response object *must be returned from the handler*. By default, the response status will be set to `200 OK` unless specified otherwise.

Let's look at a few setters that will allow us to build the HTTP response returned by `root_handler`.


```c{6,8-9}
#include "libys.h"

#define PORT 9000

ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_TXT);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  return 0;
}
```

First, we have the `ys_set_header` function. `ys_set_header` — as its name would suggest — sets a key/value pair on the response headers. Under the hood, Ys will handle formatting the header and encompassing response into a RFC 7230-compliant format. All we need to do here is set the key and value.

```c{2}
ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_TXT);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}
```

Here, we're setting the `Content-Type` to `text/plain`.

::: info TIP
Notice the `YS_MIME_TYPE_TXT`. You can find a full list of the MIME type constants provided [here](../reference/constants.md#mime-type-constants).
:::

Speaking of RFC 7230, most HTTP headers are allowed to have multiple values corresponding to the same key. Were we to call `ys_set_header` twice with the same key but different values, Ys will use both values in the response.

For example

```c{2-3}
ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Header", "value-1");
  ys_set_header(res, "X-Header", "value-2");
  // ...
  return res; // X-Header: value-1, value-2
}
```

Next, we set the body of the response using the aptly-named `ys_set_body` function. In Ys, you *do not need to set `Content-Length`* — Ys will compute this header for you when serializing the response by determining the correct size of the response body. If you do not set a body, `Content-Length` will be set to `0` ([see exceptions](../reference/)).

```c{4}
ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_TXT);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}
```

Next, we explicitly set the response status to `200 OK` using the `ys_set_status` function and `YS_STATUS_OK` enum. As aforementioned, the response is set to `200 OK` by default if you do not call `ys_set_status`, but we're including it here for illustrative purposes.

```c{5}
ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_TXT);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}
```

Similarly, if you set a body but do not set a `Content-Type` header, Ys will default the `Content-Type` to `text/plain`.

Finally, we return the response object with our modifications so it can be serialized by Ys and sent back to the client.

```c{7}
ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_TXT);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}
```

Now that we've configured our root handler, let's finish setting up the server so we can run it locally. First, we create an attributes object for our server.

```c{7-8}
int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  ys_server_attr* srv_attr = ys_server_attr_init(router);
  ys_server_set_port(srv_attr, PORT);

  return 0;
}
```

We've added two new lines. The first initializes a new `ys_server_attr`, a required settings object for our server. It is on this object that we can configure a port (or not — and use the default port: 5000) and certs for HTTPs. The only argument for `ys_server_attr_init` is the root router instance for the server.

We've also overridden the default port to 9000 using the `ys_server_set_port` helper. You can read about the other `ys_server_attr*` convenience functions [here](../reference/server-attr.md).

Now let's add two more lines to create and start the server.

```c{10-11}
int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  ys_server_attr* srv_attr = ys_server_attr_init(router);
  ys_server_set_port(srv_attr, PORT);

  ys_server *server = ys_server_init(srv_attr);
  ys_server_start(server);

  return 0;
}
```

The first new line initializes a new `ys_server` instance. By default, the server will not use SSL (see [HTTPs Support](../documentation/https-support.md)). To initialize a new server, we pass the attributes object we created in the previous step.

Once we have our `ys_server` instance, we can pass it to `ys_server_start`, which will start the server, configure signal handlers for `SIGINT` and `SIGSEGV` (for graceful shutdowns), and begin listening on port 9000.

Let's compile, run, and try sending a request to our new server!

-->
```sh
# Compile Ys into a statically-linked library and install
make install
# Compile our server program, linking it to Ys - we'll likely need to link these other libs as well
gcc main.c -o main -lm -lpcre -lssl -lcrypto -lys
# Run the server program
./main
# In a separate terminal, send a request to the server using curl
curl localhost:9000/
# Hello World!
```
