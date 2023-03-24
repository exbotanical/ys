#ifndef RESPONSE_H
#define RESPONSE_H

#include "libhttp.h"
#include "libutil/libutil.h"
#include "request.h"

/**
 * res_init initializes a new response object
 *
 * @return res_t*
 */
res_t *res_init();

/**
 * res_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req The request, used to determine how to serialize the response per
 * edge cases
 * @param res
 * @return buffer_t*
 */
buffer_t *res_serialize(req_t *req, res_t *res);

/**
 * res_send writes the given response to the given socket
 *
 * @param socket
 * @param response
 */
void res_send(int socket, buffer_t *response);

void res_preempterr(int socket, read_error_t err);

#endif /* RESPONSE_H */
