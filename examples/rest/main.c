#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json.h"
#include "libhttp.h"

#define PORT 9000

typedef struct record {
  char *key;
  char *value;
} record_t;

record_t *records = NULL;
int num_records = 0;

record_t *search_records(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    record_t *record = &records[i];
    if (strcmp(record->key, key) == 0) {
      return record;
    }
  }

  return NULL;
}

int search_records_idx(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    record_t record = records[i];
    if (strcmp(record.key, key) == 0) {
      return i;
    }
  }

  return -1;
}

char *res_ok(char *data) { return fmt_str("{\"data\":\"%s\"}", data); }

char *res_err(char *errmsg) { return fmt_str("{\"message\":\"%s\"}", errmsg); }

void add_record(const char *k, const char *v) {
  records = realloc(records, (num_records + 1) * sizeof(record_t));

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

res_t *handle_get(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "application/json");
  set_header(res, "X-Powered-By", "demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  record_t *record = search_records(id);
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

res_t *handle_delete(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "application/json");
  set_header(res, "X-Powered-By", "demo");

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

res_t *handle_put(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "application/json");
  set_header(res, "X-Powered-By", "demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  char *v = json_getstr(req->body, "v");
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

  record_t *record = &records[idx];
  memcpy(record->value, v, sizeof(v));

  set_status(res, STATUS_OK);
  return res;
}

res_t *handle_post(req_t *req, res_t *res) {
  set_header(res, "Content-Type", "application/json");
  set_header(res, "X-Powered-By", "demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  char *v = json_getstr(req->body, "v");
  if (!v) {
    set_body(res, res_err("must provide a value"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  record_t *record = search_records(id);
  if (record) {
    set_body(res, res_err("record exists"));
    set_status(res, STATUS_BAD_REQUEST);
    return res;
  }

  add_record(id, v);

  set_status(res, STATUS_CREATED);  // TODO: default to 200

  return res;
}

int main() {
  records = malloc(sizeof(record_t));

  router_attr_t *attr = router_attr_init();
  router_t *router = router_init(attr);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, record_path, handle_get, METHOD_GET, NULL);
  router_register(router, record_path, handle_delete, METHOD_DELETE, NULL);
  router_register(router, record_path, handle_put, METHOD_PUT, NULL);
  router_register(router, record_path, handle_post, METHOD_POST, NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*
GET record 2. It doesn't exist (yet):
curl localhost:9000/records/2 -v

Let's create it:
curl localhost:9000/records/2 -v -H 'Content-Type: application/json' -d '{"v":
"someval" }'

If we try to create this record again, we'll get a 400:
curl localhost:9000/records/2 -v -H 'Content-Type: application/json' -d '{"v":
"someval" }'

Let's GET the record we just created:
curl localhost:9000/records/2 -v

And now let's update it with PUT:
curl localhost:9000/records/2 -v -X PUT -H 'Content-Type: application/json' -d
'{"v": "updatedval"}'

Let's GET it to verify our updates:
curl localhost:9000/records/2 -v

Now let's DELETE the record:
curl localhost:9000/records/2 -v -X DELETE

And finally, verify it no longer exists:
curl localhost:9000/records/2 -v
*/
