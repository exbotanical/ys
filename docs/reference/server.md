# Server APIs

## tcp_server*

A `tcp_server*` is a server configuration object that stores settings for the HTTP server.

## server_init

```c
tcp_server *server_init(tcp_server_attr *attr);
```

`server_init` allocates the necessary memory for a `tcp_server`.

## server_start

```c
void server_start(tcp_server *server);
```

`server_start` listens for client connections and executes routing.

## server_free

```c
void server_free(tcp_server *server);
```

`server_free`  deallocates memory for the provided `tcp_server*` instance.
