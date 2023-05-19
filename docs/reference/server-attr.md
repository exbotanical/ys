# Server Attributes

## ys_server_attr*

A `ys_server_attr*` is an object used for storing user configurations such as port and certs for a `ys_server*`.

### ys_server_attr_init

```c
ys_server_attr* ys_server_attr_init(ys_router* router);
```

`ys_server_attr_init` initializes a new server attributes object.


### ys_server_attr_init_with

```c
ys_server_attr* ys_server_attr_init_with(ys_router* router, int port,
                                       char* cert_path, char* key_path);
```

`ys_server_attr_init_with` aggregates all of the `server_attr_*` methods into a
single convenience function.


### ys_server_set_port

```c
void ys_server_set_port(ys_server_attr* attr, int port);
```

`ys_server_set_port` sets a non-default port on the server.

### ys_server_use_https

```c
void ys_server_use_https(ys_server_attr* attr, char* cert_path, char* key_path);
```

`ys_server_use_https` instructs Ys to use SSL for the server, using the provided certificate and key filepaths.

### ys_server_disable_https

```c
void ys_server_disable_https(ys_server_attr* attr);
```

`ys_server_disable_https` disables HTTPs support. This convenience function is
typically used for testing, where cert files were set on the `ys_server_attr*`
(meaning HTTPs is enabled), but you still do not want to use HTTPs.
