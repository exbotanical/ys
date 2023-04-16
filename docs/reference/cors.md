# CORS APIs

## cors_opts*

`cors_opts*` stores CORS configuration options used to execute a CORS policy on applicable requests.


## cors_opts_init

`cors_opts_init` initializes a new CORS options object. Use the `cors_set_*` utilities to build a custom CORS policy.

## cors_allow_all

```c
cors_opts *cors_allow_all(void);
```

`cors_allow_all` initializes a new CORS options object with sensibly liberal defaults.

## use_cors

```c
void use_cors(router_attr *attr, cors_opts *opts);
```

`use_cors` binds the CORS global middleware to the [router attributes](./router-attr.md) instance.

## cors_allow_origins

```c
void cors_allow_origins(cors_opts *opts, char *origin, ...);
```

`cors_allow_origins` sets allowed origins on given the `cors_opts*`. You do not
need to pass a `NULL` sentinel to terminate the list; this macro will do it
for you.

## cors_allow_methods

```c
void cors_allow_methods(cors_opts *opts, http_method method, ...);
```

`cors_allow_methods` sets the allowed methods on the given `cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## cors_allow_headers

```c
void cors_allow_headers(cors_opts *opts, char *origin, ...);
```

`cors_allow_headers` sets the allowed headers on the given `cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## cors_expose_headers

```c
void cors_expose_headers(cors_opts *opts, char *header, ...);
```
<!-- TODO: explain what these are -->
`cors_expose_headers` sets the expose headers on the given `cors_opts*`. You do
not need to pass a `NULL` sentinel to terminate the list; this macro will do
it for you.

## cors_allow_credentials

```c
void cors_allow_credentials(cors_opts *opts, bool allow);
```

`cors_allow_credentials` sets the `cors_opts*` `allowed_credentials` option.

## cors_use_options_passthrough

```c
void cors_use_options_passthrough(cors_opts *opts, bool use);
```

`cors_use_options_passthrough` sets the `cors_opts*` `use_options_passthrough`
option  to `use`.

## cors_set_max_age

```c
void cors_set_max_age(cors_opts *opts, int max_age);
```

`cors_set_max_age` sets the `cors_opts*` max age to `max_age` seconds.
