#ifndef SERVER_H
#define SERVER_H

#include "libhttp.h"

/**
 * @brief TODO:
 *
 * @param socket
 * @param response_data
 */
void send_response(int socket, res_t *response_data);

#endif /* SERVER_H */
