# CORS APIs

## ys_cors_opts*

`ys_cors_opts*` stores CORS configuration options used to execute a CORS policy on applicable requests.


## ys_cors_opts_init

`ys_cors_opts_init` initializes a new CORS options object. Use the `cors_set_*` utilities to build a custom CORS policy.

## ys_cors_allow_all

```c
ys_cors_opts *ys_cors_allow_all(void);
```

`ys_cors_allow_all` initializes a new CORS options object with sensibly liberal defaults.

## ys_use_cors

```c
void ys_use_cors(ys_router_attr *attr, ys_cors_opts *opts);
```

`ys_use_cors` binds the CORS global middleware to the [router attributes](./router-attr.md) instance.

## ys_cors_allow_origins

```c
void ys_cors_allow_origins(ys_cors_opts *opts, char *origin, ...);
```

`ys_cors_allow_origins` sets allowed origins on given the `ys_cors_opts*`. You do not
need to pass a `NULL` sentinel to terminate the list; this macro will do it
for you.

## ys_cors_allow_methods

```c
void ys_cors_allow_methods(ys_cors_opts *opts, ys_http_method method, ...);
```

`ys_cors_allow_methods` sets the allowed methods on the given `ys_cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## ys_cors_allow_headers

```c
void ys_cors_allow_headers(ys_cors_opts *opts, char *origin, ...);
```

`ys_cors_allow_headers` sets the allowed headers on the given `ys_cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## ys_cors_expose_headers

```c
void ys_cors_expose_headers(ys_cors_opts *opts, char *header, ...);
```

`ys_cors_expose_headers` sets the expose headers on the given `ys_cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## ys_cors_allow_credentials

```c
void ys_cors_allow_credentials(ys_cors_opts *opts, bool allow);
```

`ys_cors_allow_credentials` sets the `ys_cors_opts*` `allowed_credentials` option.

## ys_cors_use_options_passthrough

```c
void ys_cors_use_options_passthrough(ys_cors_opts *opts, bool use);
```

`ys_cors_use_options_passthrough` sets the `ys_cors_opts*` `use_options_passthrough`
option  to `use`.

## ys_cors_set_max_age

```c
void ys_cors_set_max_age(ys_cors_opts *opts, int max_age);
```

`ys_cors_set_max_age` sets the `ys_cors_opts*` max age to `max_age` seconds.
