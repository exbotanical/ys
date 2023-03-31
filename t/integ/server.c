#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "deps/jsob/jsob.h"
#include "libhttp.h"

#define PORT 6124

typedef struct record {
  char *key;
  char *value;
} db_record;

db_record *records = NULL;
int num_records = 0;

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

response *set_global_headers(request *req, response *res) {
  set_header(res, "X-Powered-By", "demo");
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

  char *v = jsob_getstr(req->body, "v");
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

  char *v = jsob_getstr(req->body, "v");

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
  set_header(res, "Content-Type", "text/plain");
  set_header(res, "X-Powered-By",
             "integ-test");  // TODO: duplicate headers 1, 2, 3
  set_header(res, "X-Not-Exposed", "integ-test");

  set_body(res, "Hello World!");
  set_status(res, STATUS_OK);

  return res;
}

cors_opts *setup_cors() {
  cors_opts *opts = cors_opts_init();

  set_allowed_methods(opts, "GET", "DELETE");
  set_allowed_headers(opts, "X-Test-Header");
  set_expose_headers(opts, "X-Powered-By");
  set_allowed_origins(opts, "test.com");

  return opts;
}

int main() {
  records = malloc(sizeof(db_record));

  router_attr *attr = router_attr_init();

  add_middleware(attr, set_global_headers);
  use_cors(attr, setup_cors());

  http_router *router = router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, "/", root_handler, METHOD_GET, NULL);
  router_register(router, "/metadata", meta_handler, METHOD_GET, NULL);

  router_register(router, record_path, handle_get, METHOD_GET, NULL);
  router_register(router, record_path, handle_delete, METHOD_DELETE, NULL);
  router_register(router, record_path, handle_put, METHOD_PUT, NULL);
  router_register(router, record_path, handle_post, METHOD_POST, NULL);

  tcp_server *server = server_init(router, PORT);
  server_start(server);
  server_free(server);

  return EXIT_SUCCESS;
}
