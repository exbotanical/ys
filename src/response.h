#ifndef RESPONSE_H
#define RESPONSE_H

#include "libutil/libutil.h"
#include "libys.h"
#include "request.h"

typedef struct {
  /**
   * A flag used by middleware - setting this to true will stop the middleware
   * chain and prevent subsequent middlewares from being run
   */
  bool done;

  /**
   * HTTP status code - if not set, will default to 200 OK
   */
  http_status status;

  /**
   * HTTP headers - optional, but you should pass content-type if sending a body
   */
  hash_table *headers;

  /**
   * response body - optional; Content-length header will be set for you
   */
  char *body;
} response_internal;

/**
 * response_init initializes a new response object
 *
 * @return response_internal*
 */
response_internal *response_init(void);

/**
 * response_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req The request, used to determine how to serialize the response per
 * edge cases
 * @param res
 * @return buffer_t*
 */
buffer_t *response_serialize(request_internal *req, response_internal *res);

/**
 * response_send writes the given response to the given socket
 *
 * @param ctx
 * @param buf
 */
void response_send(client_context *ctx, buffer_t *buf);

/**
 * response_send_error pre-empts response_send with an error response
 *
 * @param ctx
 * @param err
 */
void response_send_error(client_context *ctx, parse_error err);

void response_send_protocol_error(int sockfd);
#endif /* RESPONSE_H */
