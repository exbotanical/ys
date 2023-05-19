#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "deps/jsob/jsob.h"
#include "libys.h"

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

ys_response *set_global_headers(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Powered-By", "demo");
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
  memcpy(record->value, v, sizeof(v));

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

int main() {
  records = malloc(sizeof(db_record));

  ys_router_attr *attr = ys_router_attr_init();
  ys_use_middleware(attr, set_global_headers);
  ys_router *router = ys_router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  ys_router_register(router, record_path, handle_get, YS_METHOD_GET);
  ys_router_register(router, record_path, handle_delete, YS_METHOD_DELETE);
  ys_router_register(router, record_path, handle_put, YS_METHOD_PUT);
  ys_router_register(router, record_path, handle_post, YS_METHOD_POST);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);

  return EXIT_SUCCESS;
}

/*
GET record 2. It doesn't exist (yet):
curl localhost:5000/records/2 -v

Let's create it:
curl localhost:5000/records/2 -v -H 'Content-Type: application/json' -d '{"v":
"someval" }'

If we try to create this record again, we'll get a 400:
curl localhost:5000/records/2 -v -H 'Content-Type: application/json' -d '{"v":
"someval" }'

Let's GET the record we just created:
curl localhost:5000/records/2 -v

And now let's update it with PUT:
curl localhost:5000/records/2 -v -X PUT -H 'Content-Type: application/json' -d
'{"v": "updatedval"}'

Let's GET it to verify our updates:
curl localhost:5000/records/2 -v

Now let's DELETE the record:
curl localhost:5000/records/2 -v -X DELETE

And finally, verify it no longer exists:
curl localhost:5000/records/2 -v
*/
