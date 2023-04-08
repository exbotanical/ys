# HTTPs Support

Ys ships with support for TLS from openssl. Requirements

* A local installation of openssl
* Compile Ys with the `USE_TLS` flag set to a non-zero value

```sh
make ys.a USE_TLS=1
```

Once compiled, modify your server code to include your TLS cert and key files.

```c
int main() {
  // ...
  tcp_server *server = server_init(router, PORT);
  server_set_cert(server, "./certs/yoursite.pem", "./certs/yoursite-key.pem");

  server_start(server);
}
```
