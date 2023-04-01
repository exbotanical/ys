#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhttp.h"

#define PORT 6124

char *COOKIE_ID = "TestCookie";

bool eq(const char *s1, const char *s2) { return strcmp(s1, s2) == 0; }

response *data_handler(request *req, response *res) {
  set_body(res, "{ \"data\": \"Hello World!\" }");
  set_header(res, "Content-Type", "application/json");
  set_status(res, STATUS_OK);

  return res;
}

response *login_handler(request *req, response *res) {
  char *session_id = "200";

  cookie *c = cookie_init(COOKIE_ID, session_id);
  cookie_set_expires(c, n_minutes_from_now(1));
  cookie_set_path(c, "/");
  set_cookie(res, c);

  set_status(res, STATUS_OK);

  return res;
}

response *logout_handler(request *req, response *res) {
  cookie *c = get_cookie(req, COOKIE_ID);
  if (!c) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res, true);

    return res;
  }

  char *sid = cookie_get_value(c);
  if (!sid) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res, true);

    return res;
  }

  cookie_set_max_age(c, -1);
  set_cookie(res, c);

  return res;
}

response *auth_middleware(request *req, response *res) {
  set_header(res, "X-Authorized-By", "TheDemoApp");
  if (eq(request_get_path(req), "/login") ||
      eq(request_get_path(req), "/register")) {
    return res;
  }

  cookie *c = get_cookie(req, COOKIE_ID);

  if (!c) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res, true);

    return res;
  }

  cookie_set_expires(c, n_minutes_from_now(1));
  set_cookie(res, c);

  return res;
}

// TODO: cors
int main() {
  router_attr *attr = router_attr_init();
  add_middleware(attr, auth_middleware);

  http_router *router = router_init(attr);

  router_register(router, "/login", login_handler, METHOD_POST, NULL);
  router_register(router, "/logout", logout_handler, METHOD_POST, NULL);
  router_register(router, "/data", data_handler, METHOD_GET, NULL);

  tcp_server *server = server_init(router, PORT);
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}
