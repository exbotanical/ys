# Response APIs

## ys_response*

A `ys_response*` holds the necessary metadata for the response that will be
sent to the client. Client handlers should use the helpers outlined below to set the desired properties e.g. status, body, and headers.

## ys_set_header

```c
bool ys_set_header(ys_response *res, const char *key, const char *value);
```

`ys_set_header` inserts the given key/value pair as a header on the response. Returns a `bool` indicating whether the header was successfully set. A `false` return value is unlikely and would indicate a catastrophic memory error.

## ys_set_body

```c
void ys_set_body(ys_response *res, const char *fmt, ...);
```

`ys_set_body` sets the given body on the response. You can pass a regular string or a format string plus *n* values to interpolate. `fmt` here uses the same format syntax as `printf`.

## ys_set_status

```c
void ys_set_status(ys_response *res, ys_http_status status);
```

`ys_set_status` sets the given status code on the response.

## ys_get_done

```c
bool ys_get_done(ys_response *res);
```

`ys_get_done` returns the `done` status of the response. The `done` flag is a response property that informs Ys to complete the request immediately after the handler has returned. Specifically, if any middlewares are scheduled to run, they will be skipped and the response will be sent to the client.


## ys_set_done

```c
void ys_set_done(ys_response *res);
```

`ys_set_done` sets the `done` status of the response to `true`. This operation is
idempotent.


## ys_from_file

```c
char *ys_from_file(const char *filename);
```

`ys_from_file` reads a file into a string buffer, which may then be passed
directly to `ys_set_body`.

For example

```c
ys_set_body(res, ys_from_file("./index.html"));
```

## ys_get_header

```c
char *ys_get_header(ys_response *res, const char *key);
```

`ys_get_header` retrieves the first header value for a given key on
the response or `NULL` if not found.
