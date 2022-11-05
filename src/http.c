#define _GNU_SOURCE

#include "http.h"

#include "array.h"
#include "path.h"
#include "router.h"
#include "util.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>

// uses sentinel variant
buffer_t *build_response(http_status_t status, char **headers, char *body) {
	buffer_t *response = buffer_init();
	buffer_append(
		response,
		fmt_str("HTTP/1.1 %d %s\n", status, http_status_names[status])
	);

  for (int i = 0; i < sizeof(headers[i]) / sizeof(char*); i++) {
    char *header = headers[i];
    buffer_append(response, header);
    buffer_append(response, "\n");
	}

  char *body = body;

	buffer_append(
		response,
		fmt_str("Content-Length: %d\n\n", body ? strlen(body) : 0)
	);

	buffer_append(response, body ? body : "");

	return response;
}

struct request build_request(char *buffer) {
	char *raw_request = strtok(buffer, "\n");
	char *method = strtok(raw_request, " ");
	char *url = strtok(NULL, " ");
	char *protocol = strtok(raw_request, " ");
	char *host = strtok(raw_request, "\n");

	printf("Raw Request: %s, Method: %s, URL: %s, Protocol: %s, Host: %s\n",
		raw_request,
		method,
		url,
		protocol,
		host
	);

	struct request request = {
		.method = method,
		.url = url
	};

	return request;
}

void* default_not_found_handler (void *arg) {
	printf(arg);
	// buffer_t *response = build_response( NULL,
	// 	"X-Powered-By: rest-c",
	// 	"Content-Type: text/plain",
	// 	NULL
	// );

	return NULL;
}

void* default_method_not_allowed_handler (void *arg) {
	printf(arg);
	return NULL;
}
