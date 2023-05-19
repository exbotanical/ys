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

ys_response *data_handler(ys_request *req, ys_response *res) {
  ys_set_body(res, "{ \"data\": \"Hello World!\" }");
  ys_set_header(res, "Content-Type", "application/json");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *login_handler(ys_request *req, ys_response *res) {
  char *session_id = "200";

  ys_cookie *c = ys_cookie_init(COOKIE_ID, session_id);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(1));
  ys_cookie_set_path(c, "/");
  ys_set_cookie(res, c);

  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *logout_handler(ys_request *req, ys_response *res) {
  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);
  if (!c) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  char *sid = ys_cookie_get_value(c);
  if (!sid) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  ys_cookie_set_max_age(c, -1);
  ys_set_cookie(res, c);

  return res;
}

ys_response *auth_middleware(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Authorized-By", "TheDemoApp");

  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);

  if (!c) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  ys_cookie_set_expires(c, ys_n_minutes_from_now(1));
  ys_set_cookie(res, c);

  return res;
}

ys_response *set_global_headers(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Powered-By", "demo");
  ys_set_header(res, "X-Middleware", "test");

  return res;
}

ys_response *handle_api_root(ys_request *req, ys_response *res) {
  ys_set_body(res, "api root");
  return res;
}

ys_response *handle_api_demo(ys_request *req, ys_response *res) {
  ys_set_body(res, ys_req_get_body(req));
  return res;
}

ys_response *handle_get(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "application/json");

  char *id = ys_req_get_parameter(req, "id");
  if (!id) {
    ys_set_body(res, res_err("must provide an id"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  db_record *record = search_records(id);
  if (!record) {
    ys_set_body(res, res_err("no matching record"));
    ys_set_status(res, YS_STATUS_NOT_FOUND);
    return res;
  }

  ys_set_body(res, res_ok(fmt_str("{\"key\":\"%s\",\"value\":\"%s\"}",
                                  record->key, record->value)));
  ys_set_status(res, YS_STATUS_OK);
  return res;
}

ys_response *handle_delete(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "application/json");

  char *id = ys_req_get_parameter(req, "id");
  if (!id) {
    ys_set_body(res, res_err("must provide an id"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  bool ok = delete_record(id);
  if (!ok) {
    ys_set_body(res, res_err("no matching record"));
    ys_set_status(res, YS_STATUS_NOT_FOUND);
    return res;
  }

  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *handle_put(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "application/json");

  char *id = ys_req_get_parameter(req, "id");
  if (!id) {
    ys_set_body(res, res_err("must provide an id"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  char *v = jsob_getstr(ys_req_get_body(req), "v");
  if (!v) {
    ys_set_body(res, res_err("must provide a value"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  int idx = search_records_idx(id);
  if (idx == -1) {
    ys_set_body(res, res_err("no matching record"));
    ys_set_status(res, YS_STATUS_NOT_FOUND);
    return res;
  }

  db_record *record = &records[idx];
  memcpy(record->value, v, strlen(v));

  ys_set_status(res, YS_STATUS_OK);
  return res;
}

ys_response *handle_post(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "application/json");

  char *id = ys_req_get_parameter(req, "id");
  if (!id) {
    ys_set_body(res, res_err("must provide an id"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  char *v = jsob_getstr(ys_req_get_body(req), "v");

  if (!v) {
    ys_set_body(res, res_err("must provide a value"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  db_record *record = search_records(id);
  if (record) {
    ys_set_body(res, res_err("record exists"));
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    return res;
  }

  add_record(id, v);

  ys_set_status(res, YS_STATUS_CREATED);

  return res;
}

ys_response *meta_handler(ys_request *req, ys_response *res) {
  char **movies = ys_req_get_query(req, "movies");

  buffer_t *buf = buffer_init(NULL);
  for (unsigned int i = 0; i < ys_req_num_queries(req, "movies"); i++) {
    buffer_append(buf, movies[i]);
  }

  ys_set_body(res, buffer_state(buf));

  // status defaults to 200 when not set explicitly
  return res;
}

ys_response *root_handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Powered-By", "integ-test");
  ys_set_header(res, "X-Not-Exposed", "integ-test");
  ys_set_header(res, "X-Root-Handler", "test");

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_cors_opts *setup_cors(void) {
  ys_cors_opts *opts = ys_cors_opts_init();

  ys_cors_allow_methods(opts, YS_METHOD_GET, YS_METHOD_DELETE);
  ys_cors_allow_headers(opts, "X-Test-Header");
  ys_cors_expose_headers(opts, "X-Powered-By");
  ys_cors_allow_origins(opts, "test.com");

  return opts;
}

int main(int argc, char **argv) {
  const char *use_ssl = argc > 0 && s_equals("USE_SSL", argv[1]);

  records = malloc(sizeof(db_record));

  /* Root Router */
  ys_router_attr *attr = ys_router_attr_init();
  ys_add_middleware_with_opts(attr, set_global_headers, "^/ignore");
  ys_cors_opts *cors = setup_cors();
  ys_use_cors(attr, cors);

  ys_router *router = ys_router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  ys_router_register(router, "/", root_handler, YS_METHOD_GET);
  ys_router_register(router, "/metadata", meta_handler, YS_METHOD_GET);

  ys_router_register(router, record_path, handle_get, YS_METHOD_GET);
  ys_router_register(router, record_path, handle_delete, YS_METHOD_DELETE);
  ys_router_register(router, record_path, handle_put, YS_METHOD_PUT);
  ys_router_register(router, record_path, handle_post, YS_METHOD_POST);

  /* API Router */
  ys_router *api_router = ys_router_register_sub(router, attr, "/api");
  ys_router_register(api_router, "/", handle_api_root, YS_METHOD_GET);
  ys_router_register(api_router, "/demo", handle_api_demo, YS_METHOD_POST);

  /* Auth Router */
  ys_router_attr *auth_attr = ys_router_attr_init();
  ys_add_middleware_with_opts(auth_attr, auth_middleware, "^/auth/login$",
                              "^/auth/register$");
  ys_use_cors(auth_attr, cors);

  ys_router *auth_router = ys_router_register_sub(router, auth_attr, "/auth");
  ys_router_register(auth_router, "/login", login_handler, YS_METHOD_POST);
  ys_router_register(auth_router, "/logout", logout_handler, YS_METHOD_POST);
  ys_router_register(auth_router, "/data", data_handler, YS_METHOD_GET);

  ys_server_attr *srv_attr =
      ys_server_attr_init_with(router, PORT, "./t/integ/certs/localhost.pem",
                               "./t/integ/certs/localhost-key.pem");

  if (!use_ssl) {
    ys_server_disable_https(srv_attr);
  }
  ys_server *server = ys_server_init(srv_attr);

  ys_server_start(server);
  ys_server_free(server);

  return EXIT_SUCCESS;
}
