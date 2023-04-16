# Server Attributes

## tcp_server_attr*

A `tcp_server_attr*` is an object used for storing user configurations such as port and certs for a `tcp_server*`.

### server_attr_init

```c
tcp_server_attr* server_attr_init(http_router* router);
```

`server_attr_init` initializes a new server attributes object.


### server_attr_init_with

```c
tcp_server_attr* server_attr_init_with(http_router* router, int port,
                                       char* cert_path, char* key_path);
```

`server_attr_init_with` aggregates all of the `server_attr_*` methods into a
single convenience function.


### server_set_port

```c
void server_set_port(tcp_server_attr* attr, int port);
```

`server_set_port` sets a non-default port on the server.

### server_use_https

```c
void server_use_https(tcp_server_attr* attr, char* cert_path, char* key_path);
```

`server_use_https` instructs Ys to use SSL for the server, using the provided certificate and key filepaths.

### server_disable_https

```c
void server_disable_https(tcp_server_attr* attr);
```

`server_disable_https` disables HTTPs support. This convenience function is
typically used for testing, where cert files were set on the `tcp_server_attr*`
(meaning HTTPs is enabled), but you still do not want to use HTTPs.
