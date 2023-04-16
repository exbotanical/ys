#include <stdlib.h>

#include "libys.h"

const char *html =
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">"
    "<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">"
    "<title> HTML 5 Boilerplate</title>"
    "<link rel=\"stylesheet\" href=\"/style.css\">"
    "</head>"
    "<body>"
    "<h1>"
    "libys!"
    "</h1>"
    "</body>"
    "</html>";

response *handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/html");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, html);
  set_status(res, STATUS_OK);

  return res;
}

response *css_handler(request *req, response *res) {
  set_header(res, "Content-Type", "text/css");
  set_header(res, "X-Powered-By", "demo");

  set_body(res, from_file("./style.css"));
  set_status(res, STATUS_OK);

  return res;
}

int main() {
  router_attr *attr = router_attr_init();
  http_router *router = router_init(attr);
  router_register(router, "/", handler, METHOD_GET);
  router_register(router, "/style.css", css_handler, METHOD_GET);

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

  return EXIT_SUCCESS;
}

// curl localhost:5000
