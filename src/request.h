#ifndef REQUEST_H
#define REQUEST_H

#include "libhttp.h"

#define REQ_BUFFER_SIZE 4096

req_t* read_and_parse_request(int sock);
#endif /* REQUEST_H */
