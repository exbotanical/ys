#ifndef SERVER_H
#define SERVER_H

#include "libhttp.h"

/**
 * send_response writes the given response to the given socket
 *
 * @param socket
 * @param response
 */
void send_response(int socket, res_t *response);

#endif /* SERVER_H */
