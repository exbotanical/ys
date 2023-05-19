# Server APIs

## ys_server*

A `ys_server*` is a server configuration object that stores settings for the HTTP server.

## ys_server_init

```c
ys_server *ys_server_init(ys_server_attr *attr);
```

`ys_server_init` allocates the necessary memory for a `ys_server`.

## ys_server_start

```c
void ys_server_start(ys_server *server);
```

`ys_server_start` listens for client connections and executes routing.

## ys_server_free

```c
void ys_server_free(ys_server *server);
```

`ys_server_free`  deallocates memory for the provided `ys_server*` instance.
