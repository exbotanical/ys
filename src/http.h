#ifndef HTTP_H
#define HTTP_H

#include "lib.hash/hash.h" /* for TODO: */

typedef enum method {
	GET,
	HEAD,
	POST,
	PUT,
	PATCH, // RFC 5789
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE
} method_t;

#endif /* HTTP_H */
