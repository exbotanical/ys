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
  memcpy(record->value, v, sizeof(v));

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

int main() {
  records = malloc(sizeof(db_record));

  router_attr *attr = router_attr_init();
  add_middleware(attr, set_global_headers);
  http_router *router = router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, record_path, handle_get, METHOD_GET, NULL);
  router_register(router, record_path, handle_delete, METHOD_DELETE, NULL);
  router_register(router, record_path, handle_put, METHOD_PUT, NULL);
  router_register(router, record_path, handle_post, METHOD_POST, NULL);

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

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
