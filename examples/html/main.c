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

ys_response *handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/html");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, html);
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *css_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/css");
  ys_set_header(res, "X-Powered-By", "demo");

  ys_set_body(res, ys_from_file("./style.css"));
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_router *router = ys_router_init(attr);
  ys_router_register(router, "/", handler, YS_METHOD_GET);
  ys_router_register(router, "/style.css", css_handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);

  return EXIT_SUCCESS;
}

// curl localhost:5000
