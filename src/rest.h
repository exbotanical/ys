#ifndef REST_H
#define REST_H

#include "buffer.h"
#include "lib.hash/hash.h" /* for TODO: */

#define PORT 9000

typedef struct request {
	char *url;
	char *method;
} request_t;


int run();
buffer_t *build_response(char *body, char *header, ...);
struct request build_request(char *buffer);
void process_headers (h_record* r);
void *handler (void *arg);
#endif /* REST_H */
