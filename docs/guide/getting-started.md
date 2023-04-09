# Getting Started

## Installation

1. Clone Ys

```sh [git]
git clone https://github.com/exbotanical/ys.git
```

2. For compilation, you have two options: you can compile Ys as a shared object, or a static library. We generally recommend going with the latter, so this guide will assume you're compiling against Ys as a static library. If you need a primer on shared objects vs static libraries, see [A Primer on Shared Objects and Static Libraries](./shared-vs-static.md).

Compile Ys as a static library:
```sh [gcc]
make ys.a
```

3. Include the Ys header and setup your server. In main.c:

```c
#include "libys.h"

#include <stdlib.h>

#define PORT 9000

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  tcp_server *server = server_init(router, PORT);
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}
```

4. Add a request handler

```c{7-14,20}
#include "libys.h"

#define PORT 9000

response *root_handler(request *req, response *res) {
  res_set_header(res, "Content-Type", "text/plain");

  res_set_body(res, "Hello World!");
  res_set_status(res, STATUS_OK);

  return res;
}

int main(int argc, char **argv) {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);

  router_register(router, "/", root_handler, METHOD_GET, NULL);

  tcp_server *server = server_init(router, PORT);
  server_start(server);

  return 0;
}
```

5. Compile and run!

```sh
gcc main.c ys.a -o main

./main

curl localhost:9000/ # Hello World!
```
