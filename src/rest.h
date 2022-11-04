#ifndef REST_H
#define REST_H

#include "buffer.h"
#include "http.h"

#include "lib.hash/hash.h"

#define PORT 9000

typedef struct request {
	char *url;
	char *method;
} request_t;

buffer_t *build_response(http_status_t status, char *body, char *header, ...);

struct request build_request(char *buffer);

void* default_not_found_handler (void *arg);

void* default_method_not_allowed_handler (void *arg);

#endif /* REST_H */
