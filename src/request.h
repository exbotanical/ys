#ifndef REQUEST_H
#define REQUEST_H

#include "libhttp.h"

#define REQ_BUFFER_SIZE 4096

typedef enum { IO_ERR = 1, PARSE_ERR, REQ_TOO_LONG, DUP_HDR } read_error_t;

typedef struct {
  read_error_t code;
} req_err_t;

typedef union {
  req_err_t err;
  req_t* req;
} req_meta_t;

req_meta_t read_and_parse_request(int sock);
#endif /* REQUEST_H */
