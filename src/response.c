#include "response.h"

#include <stdlib.h>  // for EXIT_FAILURE
#include <string.h>  // for strdup

#include "logger.h"  // for DIE

res_t *response_init() {
  res_t *res = malloc(sizeof(res_t));
  if (!res) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate res_t");
  }

  res->headers = array_init();
  if (!res->headers) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate array_t");
  }

  return res;
}

bool set_header(res_t *response, char *header) {
  return array_push(response->headers, header);
}

void set_body(res_t *response, const char *body) {
  response->body = strdup(body);
}

void set_status(res_t *response, http_status_t status) {
  response->status = status;
}
