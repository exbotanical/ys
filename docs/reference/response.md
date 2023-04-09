# Response APIs

## response*

A `response*` holds the necessary metadata for the response that will be
sent to the client. Client handlers should use the helpers outlined below to set the desired properties e.g. status, body, and headers.

## res_set_header

```c
bool res_set_header(response *res, const char *key, const char *value);
```

`res_set_header` inserts the given key/value pair as a header on the response. Returns a `bool` indicating whether the header was successfully set. A `false` return value is unlikely and would indicate a catastrophic memory error.

## res_set_body

```c
void res_set_body(response *res, const char *body);
```

`res_set_body` sets the given body on the response.

## res_set_status

```c
void res_set_status(response *res, http_status status);
```

`res_set_status` sets the given status code on the response.

## res_get_done

```c
bool res_get_done(response *res);
```

`res_get_done` returns the `done` status of the response. The `done` flag is a response property that informs Ys to complete the request immediately after the handler that sets `done` to `true` has returned. Specifically, if any middlewares are scheduled to run, they will be skipped and the response will be sent to the client.


## res_set_done

```c
void res_set_done(response *res);
```

`res_set_done` sets the `done` status of the response to `true`. This operation is
idempotent.


## from_file

```c
char *from_file(const char *filename);
```

`from_file` reads a file into a string buffer, which may then be passed
directly to `res_set_body`.

For example,

```c
res_set_body(res, from_file("./index.html"));
```

## res_get_header

```c
char *res_get_header(response *res, const char *key);
```

`res_get_header` retrieves the first header value for a given key on
the response or `NULL` if not found.
