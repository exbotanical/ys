# Request APIs

## ys_request*

A `ys_request*` contains request data such as the path, method, body, route and query parameters, etc.

## ys_req_get_parameter

```c
char* ys_req_get_parameter(ys_request* req, const char* key);
```

`ys_req_get_parameter` returns the parameter value matching `key`, or `NULL`
if not extant. By parameter, we mean a route match value.

For example, the following route

```c
ys_router_register(router, "/:key[^\\d+$]", handler, YS_METHOD_GET);
```

will match any digit after `/`. For a request `/10`, the resulting `ys_request*` will contain the parameter `key=10`. Thus, `ys_req_get_parameter(req, "key");` will yield `"10"`.

## ys_req_num_parameters

```c
unsigned int ys_req_num_parameters(ys_request *req);
```

`ys_req_num_parameters` returns the number of parameters matched on the given
request. For example, given the route

<!-- TODO: test -->
```c
ys_router_register(router, "/:key[^\\d+$]/:fruit[(.+)]", handler, YS_METHOD_GET);
```

and a request

```sh
/12/banana
```

`ys_req_num_parameters` will return `2`.

## ys_req_has_parameters

```c
bool ys_req_has_parameters(ys_request *req);
```

`ys_req_has_parameters` returns a `bool` indicating whether the given request contains
any matched parameters.

## ys_req_get_query

```c
char **ys_req_get_query(ys_request *req, const char *key);
```

`ys_req_get_query` returns a `char*` array of all values corresponding to a given URL query key.
For example, if the request URL was `/some-url/?num=1&num=2&num=3`, `ys_req_get_query(req, "num")` will return `["1", "2", "3"]`.

Use `ys_req_num_queries` to determine the size of this list.

## ys_req_has_query

```c
bool ys_req_has_query(ys_request *req, const char *key);
```

`ys_req_has_query` returns a `bool` indicating whether the request contains a query match for `key`.

## ys_req_num_queries

```c
unsigned int ys_req_num_queries(ys_request *req, const char *key);
```

`ys_req_num_queries` returns the number of values associated with a given query key
`key`.

## ys_req_get_path

```c
char *ys_req_get_path(ys_request *req);
```

`ys_req_get_path` returns the full request path.

## ys_req_get_route_path

```c
char *ys_req_get_route_path(ys_request *req);
```

`ys_req_get_route_path` returns the request path segment that was matched on
the router. For sub-routers, this will be the sub-path. For example,

```sh
path = /api/demo
route_path = /demo
```

## ys_req_get_method

```c
char *ys_req_get_method(ys_request *req);
```

`ys_req_get_method` returns the request method.

## ys_req_get_body

```c
char *ys_req_get_body(ys_request *req);
```

`ys_req_get_body` returns the full request body.

## ys_req_get_raw

```c
char *ys_req_get_raw(ys_request *req);
```

`ys_req_get_raw` returns the entire, raw request as it was received by the
server.

## ys_req_get_version

```c
char *ys_req_get_version(ys_request *req);
```

`ys_req_get_version` returns the protocol version string included in the
request.

## ys_req_get_header

```c
char *ys_req_get_header(ys_request *req, const char *key);
```

`ys_req_get_header` retrieves the first value for a given header key on
the request or `NULL` if not found.
