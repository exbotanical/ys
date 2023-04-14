#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deps/jsob/jsob.h"
#include "libys.h"

#define PORT 6124

typedef struct record {
  char *key;
  char *value;
} db_record;

db_record *records = NULL;
int num_records = 0;

char *COOKIE_ID = "TestCookie";

db_record *search_records(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    db_record *record = &records[i];
    if (strcmp(record->key, key) == 0) {
      return record;
    }
  }

  return NULL;
}

int search_records_idx(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    db_record record = records[i];
    if (strcmp(record.key, key) == 0) {
      return i;
    }
  }

  return -1;
}

char *res_ok(char *data) { return fmt_str("{\"data\":\"%s\"}", data); }

char *res_err(char *errmsg) { return fmt_str("{\"message\":\"%s\"}", errmsg); }

void add_record(const char *k, const char *v) {
  records = realloc(records, (num_records + 1) * sizeof(db_record));

  records[num_records].key = strdup(k);
  records[num_records].value = strdup(v);
  num_records++;
}

bool delete_record(char *id) {
  int idx = search_records_idx(id);
  if (idx == -1) return false;

  free(records[idx].key);
  free(records[idx].value);

  // Shift the remaining records to the left by one position
  for (int i = idx; i < num_records - 1; i++) {
    records[i] = records[i + 1];
  }

  num_records--;
  return true;
}

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
    set_done(res);

    return res;
  }

  char *sid = cookie_get_value(c);
  if (!sid) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  cookie_set_max_age(c, -1);
  set_cookie(res, c);

  return res;
}

response *auth_middleware(request *req, response *res) {
  set_header(res, "X-Authorized-By", "TheDemoApp");

  cookie *c = get_cookie(req, COOKIE_ID);

  if (!c) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  cookie_set_expires(c, n_minutes_from_now(1));
  set_cookie(res, c);

  return res;
}

response *set_global_headers(request *req, response *res) {
  set_header(res, "X-Powered-By", "demo");
  set_header(res, "X-Middleware", "test");

  return res;
}

response *handle_api_root(request *req, response *res) {
  set_body(res, "api root");
  return res;
}

response *handle_api_demo(request *req, response *res) {
  set_body(res, req_get_body(req));
  return res;
}

response *handle_get(request *req, response *res) {
  set_header(res, "Content-Type", "application/json");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  db_record *record = search_records(id);
  if (!record) {
    set_body(res, res_err("no matching record"));
    set_status(res, STATUS_NOT_FOUND);
    return res;
  }

  set_body(res, res_ok(fmt_str("{\"key\":\"%s\",\"value\":\"%s\"}", record->key,
                               record->value)));
  set_status(res, STATUS_OK);
  return res;
}

response *handle_delete(request *req, response *res) {
  set_header(res, "Content-Type", "application/json");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  bool ok = delete_record(id);
  if (!ok) {
    set_body(res, res_err("no matching record"));
    set_status(res, STATUS_NOT_FOUND);
    return res;
  }

  set_status(res, STATUS_OK);

  return res;
}

response *handle_put(request *req, response *res) {
  set_header(res, "Content-Type", "application/json");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  char *v = jsob_getstr(req_get_body(req), "v");
  if (!v) {
    set_body(res, res_err("must provide a value"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  int idx = search_records_idx(id);
  if (idx == -1) {
    set_body(res, res_err("no matching record"));
    set_status(res, STATUS_NOT_FOUND);
    return res;
  }

  db_record *record = &records[idx];
  memcpy(record->value, v, strlen(v));

  set_status(res, STATUS_OK);
  return res;
}

response *handle_post(request *req, response *res) {
  set_header(res, "Content-Type", "application/json");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  char *v = jsob_getstr(req_get_body(req), "v");

  if (!v) {
    set_body(res, res_err("must provide a value"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  db_record *record = search_records(id);
  if (record) {
    set_body(res, res_err("record exists"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  add_record(id, v);

  set_status(res, STATUS_CREATED);

  return res;
}

response *meta_handler(request *req, response *res) {
  char **movies = req_get_query(req, "movies");

  buffer_t *buf = buffer_init(NULL);
  for (unsigned int i = 0; i < req_num_queries(req, "movies"); i++) {
    buffer_append(buf, movies[i]);
  }

  set_body(res, buffer_state(buf));

  // status defaults to 200 when not set explicitly
  return res;
}

response *root_handler(request *req, response *res) {
  set_header(res, "X-Powered-By", "integ-test");
  set_header(res, "X-Not-Exposed", "integ-test");
  set_header(res, "X-Root-Handler", "test");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

cors_opts *setup_cors(void) {
  cors_opts *opts = cors_opts_init();

  set_allowed_methods(opts, METHOD_GET, METHOD_DELETE);
  set_allowed_headers(opts, "X-Test-Header");
  set_expose_headers(opts, "X-Powered-By");
  set_allowed_origins(opts, "test.com");

  return opts;
}

int main() {
  records = malloc(sizeof(db_record));

  /* Root Router */
  router_attr *attr = router_attr_init();
  add_middleware_with_opts(attr, set_global_headers, "^/ignore");
  cors_opts *cors = setup_cors();
  use_cors(attr, cors);

  http_router *router = router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, "/", root_handler, METHOD_GET, NULL);
  router_register(router, "/metadata", meta_handler, METHOD_GET, NULL);

  router_register(router, record_path, handle_get, METHOD_GET, NULL);
  router_register(router, record_path, handle_delete, METHOD_DELETE, NULL);
  router_register(router, record_path, handle_put, METHOD_PUT, NULL);
  router_register(router, record_path, handle_post, METHOD_POST, NULL);

  /* API Router */
  http_router *api_router = router_register_sub(router, attr, "/api");
  router_register(api_router, "/", handle_api_root, METHOD_GET, NULL);
  router_register(api_router, "/demo", handle_api_demo, METHOD_POST, NULL);

  /* Auth Router */
  router_attr *auth_attr = router_attr_init();
  add_middleware_with_opts(auth_attr, auth_middleware, "^/auth/login$",
                           "^/auth/register$");
  use_cors(auth_attr, cors);

  http_router *auth_router = router_register_sub(router, auth_attr, "/auth");
  router_register(auth_router, "/login", login_handler, METHOD_POST, NULL);
  router_register(auth_router, "/logout", logout_handler, METHOD_POST, NULL);
  router_register(auth_router, "/data", data_handler, METHOD_GET, NULL);

  tcp_server_attr *srv_attr =
      server_attr_init_with(router, PORT, "./t/integ/certs/localhost.pem",
                            "./t/integ/certs/localhost-key.pem");

  server_disable_https(srv_attr);
  tcp_server *server = server_init(srv_attr);

  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}
