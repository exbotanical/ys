# HTTPs Support

Ys ships with support for SSL using [openssl](https://www.openssl.org/).

## Requirements

* A local [installation of openssl](https://github.com/openssl/openssl#build-and-install)
* SSL certificate and key files

## Code

Once compiled, modify your server code to include your SSL cert and key files.

```c
int main() {
  // ...
  ys_server_attr* srv_attr = ys_server_attr_init(router);
  ys_server_use_https(srv_attr, "./certs/yoursite.pem", "./certs/yoursite-key.pem");

  ys_server *server = ys_server_init(srv_attr);
  ys_server_start(server);
}
```
