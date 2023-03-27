#ifndef REQUEST_H
#define REQUEST_H

#include "libhttp.h"

#define REQ_BUFFER_SIZE 4096

typedef enum { IO_ERR = 1, PARSE_ERR, REQ_TOO_LONG, DUP_HDR } parse_error;

typedef struct {
  parse_error code;
} request_error;

typedef union {
  request_error err;
  request* req;
} maybe_request;

maybe_request req_read_and_parse(int sockfd);

#endif /* REQUEST_H */
