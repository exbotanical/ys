#ifndef REST_H
#define REST_H

#include "buffer.h"
#include "lib.hash/hash.h" /* for TODO: */

#define PORT 9000

int run();
buffer_t* build_response();
void process_headers (h_record* r);
#endif /* REST_H */
