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

record_t records[] = {
    {.key = "1", .value = "value1"},
    {.key = "2", .value = "value2"},
    {.key = "3", .value = "value3"},
};

record_t *search_records(char *key) {
  for (size_t i = 0; i < 3; i++) {
    record_t *record = &records[i];
    if (!record->key) continue;
    if (strcmp(record->key, key) == 0) {
      return record;
    }
  }

  return NULL;
}

char *response_ok(char *data) { return fmt_str("{\"data\":\"%s\"}", data); }

char *response_err(char *errmsg) {
  return fmt_str("{\"message\":\"%s\"}", errmsg);
}

response_t *global_response() {
  response_t *response = get_response();
  set_header(response, "Content-Type: application/json");
  set_header(response, "X-Powered-By: demo");

  return response;
}

void *handle_get(void *arg) {
  route_context_t *context = arg;
  response_t *response = global_response();

  char *id = context_get_parameter(context, "id");
  if (!id) {
    set_body(response, response_err("must provide an id"));
    set_status(response, BAD_REQUEST);
    return response;
  }

  record_t *record = search_records(id);
  if (!record) {
    set_body(response, response_err("no matching record"));
    set_status(response, NOT_FOUND);
    return response;
  }

  set_body(response, response_ok(fmt_str("{\"key\":\"%s\",\"value\":\"%s\"}",
                                         record->key, record->value)));
  set_status(response, OK);
  return response;
}

void *handle_delete(void *arg) {
  route_context_t *context = arg;
  response_t *response = global_response();

  char *id = context_get_parameter(context, "id");
  if (!id) {
    set_body(response, response_err("must provide an id"));
    set_status(response, BAD_REQUEST);
    return response;
  }

  record_t *record = search_records(id);
  if (!record || !record->value) {
    set_body(response, response_err("no matching record"));
    set_status(response, NOT_FOUND);
    return response;
  }

  // Flag record as deleted
  record->key = NULL;
  record->value = NULL;
  set_status(response, OK);

  return response;
}

// TODO: accept param and update record
void *handle_put(void *arg) {
  route_context_t *context = arg;
  response_t *response = global_response();

  char *id = context_get_parameter(context, "id");
  if (!id) {
    set_body(response, response_err("must provide an id"));
    set_status(response, BAD_REQUEST);
    return response;
  }

  record_t *record = search_records(id);
  if (!record) {
    set_body(response, response_err("no matching record"));
    set_status(response, NOT_FOUND);
    return response;
  }

  record->value = id;

  set_status(response, OK);
  return response;
}

// TODO: fix
void *handle_post(void *arg) {
  route_context_t *context = arg;
  response_t *response = global_response();

  char *id = context_get_parameter(context, "id");
  if (!id) {
    set_body(response, response_err("must provide an id"));
    set_status(response, BAD_REQUEST);
    return response;
  }

  record_t *record = search_records(id);
  if (record) {
    set_body(response, response_err("record exists"));
    set_status(response, BAD_REQUEST);
    return response;
  }

  set_status(response, CREATED);

  return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  char *record_path = "/records/:id[^\\d+$]";

  router_register(router, record_path, handle_get, NULL, GET, NULL);
  router_register(router, record_path, handle_delete, NULL, DELETE, NULL);
  router_register(router, record_path, handle_put, NULL, PUT, NULL);
  router_register(router, record_path, handle_post, NULL, POST, NULL);

  server_t *server = server_init(router, PORT);
  if (!server_start(server)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
