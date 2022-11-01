#include "util.h"
#include "rest.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int run() {
	int server_fd;
	int new_socket;

	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Create socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	memset(address.sin_zero, NULL_TERM, sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 10) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Listening on port %i...\n", PORT);

  while(1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		buffer_t* response = build_response("Hello world!");
		write(new_socket, response->state, response->len);
		buffer_free(response);

		close(new_socket);
	}

	return EXIT_SUCCESS;
}

buffer_t* build_response(char* body) {
	buffer_t* response = buffer_init();
	buffer_append(response, "HTTP/1.1 200 OK\n");
	buffer_append(response, "Server: My Personal HTTP Server\n");
	buffer_append(response, "Content-Type: text/plain\n");
	buffer_append(response, "Content-Length: ");
	buffer_append(response, safe_itoa(strlen(body)));
	buffer_append(response, "\n\n");
	buffer_append(response, body);

	return response;
}

void process_headers (h_record* r) {
	printf("key: %s\n", r->key);
	printf("value: %s\n",r->value);
}
