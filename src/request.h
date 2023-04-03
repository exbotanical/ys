#ifndef REQUEST_H
#define REQUEST_H

#include "client.h"
#include "libhttp.h"

typedef enum { IO_ERR = 1, PARSE_ERR, REQ_TOO_LONG, DUP_HDR } parse_error;

typedef struct {
  char *route_path;
  const char *path;
  const char *method;
  const char *body;
  const char *raw;
  const char *protocol;
  const char *version;
  hash_table *parameters;
  hash_table *queries;
  hash_table *headers;
} request_internal;

typedef struct {
  parse_error code;
} request_error;

typedef union {
  request_error err;
  request_internal *req;
} maybe_request;

maybe_request req_read_and_parse(client_context *ctx);

#endif /* REQUEST_H */
