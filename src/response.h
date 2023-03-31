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
 * res_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req The request, used to determine how to serialize the response per
 * edge cases
 * @param res
 * @return buffer_t*
 */
buffer_t *res_serialize(request *req, response *res);

/**
 * res_send writes the given response to the given socket
 *
 * @param ctx
 * @param resbuf
 */
void res_send(client_context *ctx, buffer_t *resbuf);

/**
 * res_preempterr pre-empts res_send with an error response
 *
 * @param ctx
 * @param err
 */
void res_preempterr(client_context *ctx, parse_error err);

#endif /* RESPONSE_H */
