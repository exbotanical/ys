#ifndef RESPONSE_H
#define RESPONSE_H

#include "libhttp.h"
#include "libutil/libutil.h"
#include "request.h"

/**
 * response_init initializes a new response object
 *
 * @return response*
 */
response *response_init();

/**
 * response_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req The request, used to determine how to serialize the response per
 * edge cases
 * @param res
 * @return buffer_t*
 */
buffer_t *response_serialize(request *req, response *res);

/**
 * response_send writes the given response to the given socket
 *
 * @param ctx
 * @param resbuf
 */
void response_send(client_context *ctx, buffer_t *resbuf);

/**
 * response_send_error pre-empts response_send with an error response
 *
 * @param ctx
 * @param err
 */
void response_send_error(client_context *ctx, parse_error err);

void response_send_protocol_error(int sockfd);
#endif /* RESPONSE_H */
