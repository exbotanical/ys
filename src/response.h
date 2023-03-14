#ifndef RESPONSE_H
#define RESPONSE_H

#include "libhttp.h"

/**
 * response_init initializes a new response object
 *
 * @return res_t*
 */
res_t *response_init();

/**
 * send_response writes the given response to the given socket
 *
 * @param socket
 * @param response
 */
void send_response(int socket, res_t *response_data);

#endif /* RESPONSE_H */
