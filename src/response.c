#include <string.h>  // for strdup

#include "libhttp.h"

bool set_header(res_t *response, char *header) {
  return array_push(response->headers, header);
}

void set_body(res_t *response, const char *body) {
  response->body = strdup(body);
}

void set_status(res_t *response, http_status_t status) {
  response->status = status;
}
