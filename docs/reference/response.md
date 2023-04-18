# Response APIs

## response*

A `response*` holds the necessary metadata for the response that will be
sent to the client. Client handlers should use the helpers outlined below to set the desired properties e.g. status, body, and headers.

## set_header

```c
bool set_header(response *res, const char *key, const char *value);
```

`set_header` inserts the given key/value pair as a header on the response. Returns a `bool` indicating whether the header was successfully set. A `false` return value is unlikely and would indicate a catastrophic memory error.

## set_body

```c
void set_body(response *res, const char *fmt, ...);
```

`set_body` sets the given body on the response. You can pass a regular string or a format string plus *n* values to interpolate. `fmt` here uses the same format syntax as `printf`.

## set_status

```c
void set_status(response *res, http_status status);
```

`set_status` sets the given status code on the response.

## get_done

```c
bool get_done(response *res);
```

`get_done` returns the `done` status of the response. The `done` flag is a response property that informs Ys to complete the request immediately after the handler has returned. Specifically, if any middlewares are scheduled to run, they will be skipped and the response will be sent to the client.


## set_done

```c
void set_done(response *res);
```

`set_done` sets the `done` status of the response to `true`. This operation is
idempotent.


## from_file

```c
char *from_file(const char *filename);
```

`from_file` reads a file into a string buffer, which may then be passed
directly to `set_body`.

For example

```c
set_body(res, from_file("./index.html"));
```

## get_header

```c
char *get_header(response *res, const char *key);
```

`get_header` retrieves the first header value for a given key on
the response or `NULL` if not found.
