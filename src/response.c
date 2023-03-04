#include <string.h>

#include "libhttp.h"

Response *get_response() { return response_init(); }

bool set_header(Response *response, char *header) {
  return array_push(response->headers, header);
}

void set_body(Response *response, const char *body) {
  response->body = strdup(body);
}

void set_status(Response *response, http_status_t status) {
  response->status = status;
}

bool has_params(route_context_t *ctx) {
  return ctx->parameters && array_size(ctx->parameters) > 0;
}

parameter_t *get_param(route_context_t *ctx, int idx) {
  return array_get(ctx->parameters, idx);
}
