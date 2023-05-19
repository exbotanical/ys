# Cookies

Ys offers full support for spec-compliant HTTP Cookies.

## Creating a Cookie

To create a Cookie, we need to initialize a new `ys_cookie*` object.

```c
ys_cookie *c = ys_cookie_init("cookie_name", "cookie_value");
```

We initialize a `ys_cookie*` with the only required fields — the name and value. `ys_cookie_init` initializes all other Cookie fields to their `NULL` equivalents; `max_age` is set to zero, for example, which means `Max-Age` shall be omitted entirely from the resulting Cookie.

But of course, you will typically want to configure the Cookie. Ys offers several `cookie_set_*` APIs to that end. A full accounting of these APIs can be found in the [Cookies API Reference](../reference/cookies.md).

::: warning NOTE
From the Ys user's perspective, setting a Cookie's `max_age` to -1 with `ys_cookie_set_max_age` means the actual Max-Age in the serialized Cookie will be set to 0. Setting the `max_age` to 0 with `ys_cookie_set_max_age` means the actual Max-Age will be omitted entirely.
:::

## Deleting a Cookie

To instruct a client to delete a Cookie, we set the Max-Age to 0 by calling `ys_cookie_set_max_age(cookie, -1)`.

## Setting a Cookie

To set a Cookie on a `ys_response*`, we use the `ys_set_cookie` function.


::: warning NOTE
Once `ys_set_cookie` has been invoked, the original `ys_cookie*` object will be `free`d and completely invalidated.
:::

By calling `ys_set_cookie`, we instruct Ys to serialize the `ys_cookie*` into a `Set-Cookie` header.

For example:

```c{5-8}
ys_response *login_handler(ys_request *req, ys_response *res) {
  // ...
  char *session_id = create_sid();

  ys_cookie *c = ys_cookie_init(COOKIE_ID, session_id);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  ys_cookie_set_path(c, "/");
  ys_set_cookie(res, c);

  ys_set_status(res, YS_STATUS_OK);

  return res;
}
```

## Retrieving a Cookie

To retrieve a Cookie from a request, we call `ys_get_cookie`. `ys_get_cookie` looks at the request headers to find the `Cookie` matching the specified value, then parses the Cookie into a `ys_cookie*`. If no Cookie was found, `ys_get_cookie` returns `NULL`.

We can also use `ys_cookie_get_value` to retrieve the `value` of the Cookie:

```c{2,10}
ys_response *authorize_handler(ys_request *req, ys_response *res) {
  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);
  if (!c) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  char *sid = ys_cookie_get_value(c);

  // ...
  return res;
}
