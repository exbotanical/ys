#include <string.h>  // for strdup

#include "libhttp.h"

response_t *get_response() { return response_init(); }

bool set_header(response_t *response, char *header) {
  return array_push(response->headers, header);
}

void set_body(response_t *response, const char *body) {
  response->body = strdup(body);
}

void set_status(response_t *response, http_status_t status) {
  response->status = status;
}
