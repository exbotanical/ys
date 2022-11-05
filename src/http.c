#define _GNU_SOURCE

#include "http.h"

#include "array.h"
#include "context.h"
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
buffer_t *build_response(
	http_status_t status,
	char *content_type,
	char *body,
	...
) {
	buffer_t *headers = buffer_init();
	buffer_append(
		headers,
		fmt_str("HTTP/1.1 %d %s\n", status, http_status_names[status])
	);
	buffer_append(
		headers,
		fmt_str("Content-Type: %s\n", content_type)
	);

  va_list args;
	va_start(args, body);

	char *header;
	// Add user-defined headers
	while ((header = va_arg(args, const char *)) != NULL) {
		// Content-Length needs to be accurate; thus we ensure the framework is setting it
		if (!strcasestr(header, "Content-Length:")) {
			buffer_append(headers, header);
			buffer_append(headers, "\n");
		}
	}

	va_end(args);

	buffer_append(
		headers,
		fmt_str("Content-Length: %d\n\n", strlen(body))
	);

	buffer_append(headers, body);

	return headers;
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
