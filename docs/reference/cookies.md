# Cookies APIs

<!-- TODO: document -->
## cookie*

A `cookie*` is an object that stores all necessary information and metadata for Cookies.

## same_site_mode

```c
typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} same_site_mode;
```

## cookie_init

```c
cookie *cookie_init(const char *name, const char *value);
```

`cookie_init` initializes a new Cookie with a given name and value. All other fields will be
initialized to `NULL` or field-equivalent values such that the field is
ignored. Use the `cookie_set_*` helpers to set the other attributes.

## cookie_set_domain

```c
void cookie_set_domain(cookie *c, const char *domain);
```

`cookie_set_domain` sets the given Cookie's domain attribute.

## cookie_set_expires

```c
void cookie_set_expires(cookie *c, time_t when);
```

`cookie_set_expires` sets the given Cookie's expires attribute.

## cookie_set_http_only

```c
void cookie_set_http_only(cookie *c, bool http_only);
```

`cookie_set_http_only` sets the given Cookie's `http_only` attribute.

## cookie_set_max_age

```c
void cookie_set_max_age(cookie *c, int age);
```

`cookie_set_max_age` sets the given Cookie's age attribute.

## cookie_set_path

```c
void cookie_set_path(cookie *c, const char *path);
```

`cookie_set_path` sets the given Cookie's path attribute.

## cookie_set_same_site

```c
void cookie_set_same_site(cookie *c, same_site_mode mode);
```

`cookie_set_same_site` sets the given Cookie's `same_site` attribute.

## cookie_set_secure

```c
void cookie_set_secure(cookie *c, bool secure);
```

`cookie_set_secure` sets the the Cookie's secure attribute to `secure`.

## get_cookie

```c
cookie *get_cookie(request *req, const char *name);
```

`get_cookie` returns the named cookie provided in the request. If multiple
cookies match the given name, only the first matched cookie will be returned.

## set_cookie

```c
void set_cookie(response *res, cookie *c);
```

`set_cookie` adds a Set-Cookie header to the provided response's headers.
The provided cookie must have a valid Name. Invalid cookies may be silently
omitted.

## cookie_get_name

```c
char *cookie_get_name(cookie *c);
```

## cookie_get_value

```c
char *cookie_get_value(cookie *c);
```

## cookie_get_domain

```c
char *cookie_get_domain(cookie *c);
```

## cookie_get_expires

```c
time_t cookie_get_expires(cookie *c);
```

## cookie_get_http_only

```c
bool cookie_get_http_only(cookie *c);
```

## cookie_get_max_age

```c
int cookie_get_max_age(cookie *c);
```

## cookie_get_path

```c
char *cookie_get_path(cookie *c);
```

## cookie_get_same_site

```c
same_site_mode cookie_get_same_site(cookie *c);
```

## cookie_get_secure

```c
bool cookie_get_secure(cookie *c);
```

## cookie_free

```c
void cookie_free(cookie *c);
```
