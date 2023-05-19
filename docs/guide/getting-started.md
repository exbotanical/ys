# Getting Started

## Installation

1. Clone Ys

```sh [git]
git clone https://github.com/exbotanical/ys.git
```

2. For compilation, you have two options: you can compile Ys as a shared object, or a static library. We generally recommend going with the latter, so the installation process will install Ys as a static library. If you need a primer on shared objects vs static libraries, see [A Primer on Shared Objects and Static Libraries](./shared-vs-static.md).

Compile Ys and install:
```sh
make install
```

3. Include the Ys header and setup your server. In main.c:

```c
#include "libys.h"

#include <stdlib.h>

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_server_attr* srv_attr = ys_server_attr_init(router);
  ys_server *server = ys_server_init(srv_attr);

  ys_server_start(server);

  return EXIT_SUCCESS;
}
```

4. Add a request handler

```c{3-10,16}
#include "libys.h"

ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main(int argc, char **argv) {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);

  ys_server_attr* srv_attr = ys_server_attr_init(router);
  ys_server *server = ys_server_init(srv_attr);

  ys_server_start(server);

  return 0;
}
```

5. Compile and run!


```sh [gcc]
gcc main.c -o main -lm -lpcre -lcrypto -lssl -lys
```

```sh
./main
```

```sh
curl localhost:5000/ # Hello World!
```
