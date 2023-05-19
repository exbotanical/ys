# Cookies APIs

## ys_cookie*

A `ys_cookie*` is an object that stores all necessary information and metadata for Cookies.

## ys_same_site_mode

```c
typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} ys_same_site_mode;
```

## ys_cookie_init

```c
ys_cookie *ys_cookie_init(const char *name, const char *value);
```

`ys_cookie_init` initializes a new Cookie with a given name and value. All other fields will be
initialized to `NULL` or field-equivalent values such that the field is
ignored. Use the `cookie_set_*` helpers to set the other attributes.

## ys_cookie_set_domain

```c
void ys_cookie_set_domain(ys_cookie *c, const char *domain);
```

`ys_cookie_set_domain` sets the given Cookie's domain attribute.

## ys_cookie_set_expires

```c
void ys_cookie_set_expires(ys_cookie *c, time_t when);
```

`ys_cookie_set_expires` sets the given Cookie's expires attribute.

## ys_cookie_set_http_only

```c
void ys_cookie_set_http_only(ys_cookie *c, bool http_only);
```

`ys_cookie_set_http_only` sets the given Cookie's `http_only` attribute.

## ys_cookie_set_max_age

```c
void ys_cookie_set_max_age(ys_cookie *c, int age);
```

`ys_cookie_set_max_age` sets the given Cookie's age attribute.

## ys_cookie_set_path

```c
void ys_cookie_set_path(ys_cookie *c, const char *path);
```

`ys_cookie_set_path` sets the given Cookie's path attribute.

## ys_cookie_set_same_site

```c
void ys_cookie_set_same_site(ys_cookie *c, ys_same_site_mode mode);
```

`ys_cookie_set_same_site` sets the given Cookie's `same_site` attribute.

## ys_cookie_set_secure

```c
void ys_cookie_set_secure(ys_cookie *c, bool secure);
```

`ys_cookie_set_secure` sets the the Cookie's secure attribute to `secure`.

## ys_get_cookie

```c
ys_cookie *ys_get_cookie(ys_request *req, const char *name);
```

`ys_get_cookie` returns the named cookie provided in the request. If multiple
cookies match the given name, only the first matched cookie will be returned.

## ys_set_cookie

```c
void ys_set_cookie(ys_response *res, ys_cookie *c);
```

`ys_set_cookie` adds a Set-Cookie header to the provided response's headers.
The provided cookie must have a valid Name. Invalid cookies may be silently
omitted.

## ys_cookie_get_name

```c
char *ys_cookie_get_name(ys_cookie *c);
```

## ys_cookie_get_value

```c
char *ys_cookie_get_value(ys_cookie *c);
```

## ys_cookie_get_domain

```c
char *ys_cookie_get_domain(ys_cookie *c);
```

## ys_cookie_get_expires

```c
time_t ys_cookie_get_expires(ys_cookie *c);
```

## ys_cookie_get_http_only

```c
bool ys_cookie_get_http_only(ys_cookie *c);
```

## ys_cookie_get_max_age

```c
int ys_cookie_get_max_age(ys_cookie *c);
```

## ys_cookie_get_path

```c
char *ys_cookie_get_path(ys_cookie *c);
```

## ys_cookie_get_same_site

```c
ys_same_site_mode ys_cookie_get_same_site(ys_cookie *c);
```

## ys_cookie_get_secure

```c
bool ys_cookie_get_secure(ys_cookie *c);
```

## ys_cookie_free

```c
void ys_cookie_free(ys_cookie *c);
```
