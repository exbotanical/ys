# Cookies

Ys offers full support for spec-compliant HTTP Cookies.

## Creating a Cookie

To create a Cookie, we need to initialize a new `cookie*` object.

```c
cookie *c = cookie_init("cookie_name", "cookie_value");
```

We initialize a `cookie*` with the only required values, its name and value. `cookie_init` initializes all other Cookie fields to their `NULL` equivalents; `max_age` is set to zero, for example, which means `Max-Age` shall be omitted entirely from the resulting Cookie.

But of course, you will typically want to configure the Cookie. Ys offers several `cookie_set_*` APIs. A full accounting of these APIs can be found in the [Cookies API Reference](../reference/cookies.md).

::: warning NOTE
From the Ys user's perspective, setting a Cookie's Max-Age to -1 with `cookie_set_max_age` means the actual Max-Age in the serialized Cookie will be set to 0. Setting the Max-Age to 0 with `cookie_set_max_age` means the actual Max-Age will be omitted entirely.
:::

## Deleting a Cookie

To instruct a client to delete a Cookie, we set the Max-Age to 0 by calling `cookie_set_max_age(cookie, -1)`.

## Setting a Cookie

To set a Cookie on a `response*`, we use the `set_cookie` function.


::: warning NOTE
Once `set_cookie` has been invoked, the original `cookie*` object will be `free`d and completely invalidated.
:::

By calling `set_cookie`, we instruct Ys to serialize the `cookie*` into a `Set-Cookie` header.

For example:

```c
response *login_handler(request *req, response *res) {
  // ...
  char *session_id = create_sid();

  cookie *c = cookie_init(COOKIE_ID, session_id);
  cookie_set_expires(c, n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  cookie_set_path(c, "/");
  set_cookie(res, c);

  res_set_status(res, STATUS_OK);

  return res;
}
```

## Retrieving a Cookie

To retrieve a Cookie from a request, we call `get_cookie`. `get_cookie` looks at the request headers, finds the `Cookie` header, and parses the value into a `cookie*`. If no Cookie was found, `get_cookie` returns `NULL`.

We can also use `cookie_get_value` to retrieve the `value` of the Cookie:

```c{2,10}
response *authorize_handler(request *req, response *res) {
  cookie *c = get_cookie(req, COOKIE_ID);
  if (!c) {
    res_set_status(res, STATUS_UNAUTHORIZED);
    res_set_done(res);

    return res;
  }

  char *sid = cookie_get_value(c);

  // ...
  return res;
}
