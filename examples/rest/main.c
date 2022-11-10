#include "array.h"
#include "path.h"
#include "router.h"
#include "http.h"
#include "server.h"

#include "lib.thread/libthread.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>
#include <sys/select.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>

#define PORT 9000

typedef struct record {
  char *key;
  char *value;
} record_t;

static const record_t records[] = {
  { .key = "1", .value = "value1" },
  { .key = "2", .value = "value2" },
  { .key = "3", .value = "value3" },
};

record_t *search_records(char *key) {
  for (size_t i = 0; i < 3; i++) {
    record_t *record = &records[i];
    if (strcmp(record->key, key) == 0) {
      return record;
    }
  }

  return NULL;
}

response_t *global_response() {
  response_t *response = response_init();
  if (!response) {
    exit(EXIT_FAILURE);
  }

  if (
    !ch_array_insert(response->headers, "Content-Type: text/plain")
    || !ch_array_insert(response->headers, "X-Powered-By: demo")
  ) {
    exit(EXIT_FAILURE);
  }

  return response;
}

void *handle_get (void *arg) {
  route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

  response_t *response = global_response();

  if (!context->parameters) {
    response->body = "must provide an id";
    response->status = BAD_REQUEST;
    return response;
  }

  parameter_t *param = context->parameters->state[0];
  record_t *record = search_records(param->value);
  if (!record) {
    response->body = "no matching record";
    response->status = NOT_FOUND;
    return response;
  }

  response->body = "no matching record";
  response->status = NOT_FOUND;
  return response;
}

void *handle_delete (void *arg) {
  route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

  response_t *response = global_response();

  if (!context->parameters) {
    response->body = "must provide an id";
    response->status = BAD_REQUEST;
    return response;
  }

  parameter_t *param = context->parameters->state[0];
  record_t *record = search_records(param->value);
  if (!record) {
    response->body = "no matching record";
    response->status = NOT_FOUND;
    return response;
  }

  // Flag record as deleted
  record->key = NULL;
  record->value = NULL;
  return response;
}

void *handle_put (void *arg) {
  route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

  response_t *response = global_response();

  if (!context->parameters) {
    response->body = "must provide an id";
    response->status = BAD_REQUEST;
    return response;
  }

  parameter_t *param = context->parameters->state[0];
  record_t *record = search_records(param->value);
  if (!record) {
    response->body = "no matching record";
    response->status = NOT_FOUND;
    return response;
  }

  record->value = param->value;
  return response;
}

void *handle_post (void *arg) {
  route_context_t *context = arg;
  if (!context) {
    exit(EXIT_FAILURE);
  }

  response_t *response = global_response();

  if (!context->parameters) {
    response->body = "must provide an id";
    response->status = BAD_REQUEST;
    return response;
  }

  parameter_t *param = context->parameters->state[0];
  record_t *record = search_records(param->value);
  if (record) {
    response->body = "record exists";
    response->status = BAD_REQUEST;
    return response;
  }

  response->status = OK;
  // TODO:
  printf("content --> %s\n", context->content);

  return response;
}

int main() {
  router_t *router = router_init(NULL, NULL);
  if (!router) {
    return EXIT_FAILURE;
  }

  char *record_path = "/records/:id[^\\d+$]";

	if (
    !router_register(router, collect_methods("GET", NULL), record_path, handle_get)
    || !router_register(router, collect_methods("DELETE", NULL), record_path, handle_delete)
    || !router_register(router, collect_methods("PUT", NULL), record_path, handle_put)
    || !router_register(router, collect_methods("POST", NULL), record_path, handle_post)
  ) {
    return EXIT_FAILURE;
  }

	server_t *server = server_init(router, PORT);
  server_start(server);

	return EXIT_SUCCESS;
}
