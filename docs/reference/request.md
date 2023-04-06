# Request APIs

## request*

A `request*` contains request data such as the path, method, body, route and query parameters, etc.

## req_get_parameter

```c
char* req_get_parameter(request* req, const char* key);
```

`req_get_parameter` returns the parameter value matching `key`, or NULL
if not extant. By parameter, we mean a route match value.

For example, the following route

```c
router_register(router, "/:key[^\\d+$]", handler, METHOD_GET, NULL);
```

will match any digit after `/`. For a request `/10`, the resulting `request*` will contain the parameter `key=10`. Thus, `req_get_parameter(req, "key");` will yield `"10"`.

## req_num_parameters

```c
unsigned int req_num_parameters(request *req);
```

`req_num_parameters` returns the number of parameters matched on the given
request. For example, given the route

<!-- TODO: test -->
```c
router_register(router, "/:key[^\\d+$]/:orange[(.+)]", handler, METHOD_GET, NULL);
```

and a request

```sh
/12/banana
```

`req_num_parameters` will return `2`.

## req_has_parameters

```c
bool req_has_parameters(request *req);
```

`req_has_parameters` returns a `bool` indicating whether the given request contains
any matched parameters.

## req_get_query

```c
char **req_get_query(request *req, const char *key);
```

`req_get_query` returns a char* array of all values corresponding to a given URL query key.
For example, if the request URL was `/some-url/?key=1&key=2&key=3`, `req_get_query` will return `["1", "2", "3"]`.

Use `req_num_queries` to determine the size of this list.

## req_has_query

```c
bool req_has_query(request *req, const char *key);
```

`req_has_query` returns a `bool` indicating whether the request contains a query match for `key`.

## req_num_queries

```c
unsigned int req_num_queries(request *req, const char *key);
```

`req_num_queries` returns the number of values associated with a given query key
`key`.

## request_get_path

```c
char *request_get_path(request *req);
```

`request_get_path` returns the full request path.

## request_get_route_path

```c
char *request_get_route_path(request *req);
```

`request_get_route_path` returns the request path segment that was matched on
the router. For sub-routers, this will be the sub-path. For example,

```sh
path = /api/demo
route_path = /demo
```

## request_get_method

```c
char *request_get_method(request *req);
```

`request_get_method` returns the request method.

## request_get_body

```c
char *request_get_body(request *req);
```

`request_get_body` returns the full request body.

## request_get_raw

```c
char *request_get_raw(request *req);
```

`request_get_raw` returns the entire, raw request as it was received by the
server.

## request_get_protocol

```c
char *request_get_version(request *req);
```

`request_get_version` returns the protocol version string included in the
request.

## request_get_header

```c
char *request_get_header(request *req, const char *key);
```

`request_get_header` retrieves the first value for a given header key on
the request or `NULL` if not found.