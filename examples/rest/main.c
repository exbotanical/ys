#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libhttp.h"

// TODO: finalize example
#define PORT 9000

typedef struct record {
  char *key;
  char *value;
} record_t;

record_t *records = NULL;
int num_records = 0;

record_t *searcht_records(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    record_t *record = &records[i];
    if (strcmp(record->key, key) == 0) {
      return record;
    }
  }

  return NULL;
}

int searcht_records_idx(char *key) {
  for (size_t i = 0; i < num_records; i++) {
    record_t record = records[i];
    if (strcmp(record.key, key) == 0) {
      return i;
    }
  }

  return -1;
}

void add_record(char *v) {
  records = realloc(records, (num_records + 1) * sizeof(record_t));

  records[num_records].key = strdup(v);
  records[num_records].value = strdup(v);
  num_records++;
}

bool delete_record(char *id) {
  int idx = searcht_records_idx(id);
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

char *res_ok(char *data) { return fmt_str("{\"data\":\"%s\"}", data); }

char *res_err(char *errmsg) { return fmt_str("{\"message\":\"%s\"}", errmsg); }

res_t *handle_get(req_t *req, res_t *res) {
  set_header(res, "Content-Type: application/json");
  set_header(res, "X-Powered-By: demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, BAD_REQUEST);
    return res;
  }

  record_t *record = searcht_records(id);
  if (!record) {
    set_body(res, res_err("no matching record"));
    set_status(res, NOT_FOUND);
    return res;
  }

  set_body(res, res_ok(fmt_str("{\"key\":\"%s\",\"value\":\"%s\"}", record->key,
                               record->value)));
  set_status(res, OK);
  return res;
}

res_t *handle_delete(req_t *req, res_t *res) {
  set_header(res, "Content-Type: application/json");
  set_header(res, "X-Powered-By: demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, BAD_REQUEST);
    return res;
  }

  bool ok = delete_record(id);
  if (!ok) {
    set_body(res, res_err("no matching record"));
    set_status(res, NOT_FOUND);
    return res;
  }

  set_status(res, OK);

  return res;
}

// TODO: use body
res_t *handle_put(req_t *req, res_t *res) {}

res_t *handle_post(req_t *req, res_t *res) {
  set_header(res, "Content-Type: application/json");
  set_header(res, "X-Powered-By: demo");

  char *id = req_get_parameter(req, "id");
  if (!id) {
    set_body(res, res_err("must provide an id"));
    set_status(res, BAD_REQUEST);
    return res;
  }

  record_t *record = searcht_records(id);
  if (record) {
    set_body(res, res_err("record exists"));
    set_status(res, BAD_REQUEST);
    return res;
  }

  add_record(id);

  set_status(res, CREATED);

  return res;
}

int main() {
  records = malloc(sizeof(record_t));

  router_t *router = router_init(NULL, NULL);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, record_path, handle_get, NULL, GET, NULL);
  router_register(router, record_path, handle_delete, NULL, DELETE, NULL);
  // router_register(router, record_path, handle_put, NULL, PUT, NULL);
  router_register(router, record_path, handle_post, NULL, POST, NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
