#ifndef RESPONSE_H
#define RESPONSE_H

#include "libhttp.h"
#include "libutil/libutil.h"
#include "request.h"

/**
 * response_init initializes a new response object
 *
 * @return res_t*
 */
res_t *response_init();

/**
 * serialize_response converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req The request, used to determine how to serialize the response per
 * edge cases
 * @param res
 * @return buffer_t*
 */
buffer_t *serialize_response(req_t *req, res_t *res);

/**
 * send_response writes the given response to the given socket
 *
 * @param socket
 * @param response
 */
void send_response(int socket, buffer_t *response);

void send_preemptive_err(int socket, read_error_t err);

#endif /* RESPONSE_H */
