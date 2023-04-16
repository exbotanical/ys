# HTTPs Support

Ys ships with support for SSL using openssl.
<!-- TODO: PRERELEASE FIX NEEDED openssl link -->
## Requirements

* A local installation of openssl
* SSL certificate and key files

## Code

Once compiled, modify your server code to include your SSL cert and key files.

```c
int main() {
  // ...
  tcp_server_attr* srv_attr = server_attr_init(router);
  server_use_https("./certs/yoursite.pem", "./certs/yoursite-key.pem");

  tcp_server *server = server_init(srv_attr);
  server_start(server);
}
```
