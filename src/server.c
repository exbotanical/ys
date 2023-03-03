
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "client.h"
#include "config.h"
#include "lib.thread/libthread.h"
#include "libhttp.h"
#include "logger.h"
#include "path.h"
#include "request.h"
#include "router.h"
#include "util.h"

static int get_num_threads() {
  char *num_threads = getenv(NUM_THREADS_KEY);
  if (num_threads == NULL) return DEFAULT_NUM_THREADS;

  int env_threads = atoi(num_threads);
  if (env_threads && env_threads > 0) return env_threads;

  return DEFAULT_NUM_THREADS;
}

/**
 * @brief Converts a user-defined response object into a buffer.
 * @internal
 *
 * @param status
 * @param headers
 * @param body
 * @return Buffer*
 */
static Buffer *build_response(Response *response_data) {
  Buffer *response = buffer_init(NULL);
  if (!response) {
    // TODO constant template str for malloc failures
    DIE(EXIT_FAILURE, "%s\n", "could not allocate memory for Buffer");
  }

  // TODO constants for response_data fields for brevity
  int status = response_data->status;
  ch_array_t *headers = response_data->headers;
  char *body = response_data->body;

  buffer_append(response,
                fmt_str("HTTP/1.1 %d %s\n", status, http_status_names[status]));

  for (int i = 0; i < (int)headers->size; i++) {
    char *header = headers->state[i];

    buffer_append(response, header);
    buffer_append(response, "\n");
  }

  buffer_append(response,
                fmt_str("Content-Length: %d\n\n", body ? strlen(body) : 0));

  buffer_append(response, body ? body : "");

  return response;
}

/**
 * @brief Extract and structure a client request.
 * @internal
 *
 * @param buffer
 * @return request_t*
 */
static request_t *build_request(char *buffer) {
  char *buffer_cp = strdup(buffer);
  request_t *request = malloc(sizeof(request_t));
  request->raw = buffer;

  // TODO: do something sensible here - this is placeholder logic
  char *n1 = strtok(buffer_cp, "\n");
  char *n2 = strtok(NULL, "\n");
  char *n3 = strtok(NULL, "\n");
  char *n4 = strtok(NULL, "\n");
  char *n5 = strtok(NULL, "\n");
  char *n6 = strtok(NULL, "\n");

  // get the body, if any
  Buffer *content = buffer_init(NULL);
  char *n7 = NULL;
  while ((n7 = strtok(NULL, "\n")) != NULL) {
    buffer_append(content, n7);
  }
  request->content = buffer_state(content);

  request->method = strtok(n1, " ");
  request->path = strtok(NULL, " ");
  request->protocol = strtok(NULL, " ");

  strtok(n2, " ");
  request->host = strtok(NULL, " ");

  strtok(n3, " ");
  request->user_agent = strtok(NULL, " ");

  strtok(n4, " ");
  request->accept = strtok(NULL, " ");

  strtok(n5, " ");
  request->content_len = strtok(NULL, " ");

  strtok(n6, " ");
  request->content_type = strtok(NULL, " ");

  return request;
}

/**
 * @brief Handles client connections and executes the user-defined router.
 * @internal
 *
 * @param args
 */
void *client_thread_handler(void *args) {
  client_context_t *c_ctx = args;
  char recv_buffer[1024];  // TODO: MAJOR todo - iterate content-length until
                           // entire request is consumed

  recvfrom(c_ctx->client_socket, (char *)recv_buffer, sizeof(recv_buffer), 0,
           c_ctx->address, c_ctx->addr_len);

  LOG("[server::client_thread_handler] client request received: %s\n",
      recv_buffer);

  router_run(c_ctx->router, c_ctx->client_socket, build_request(recv_buffer),
             NULL);

  return NULL;
}

server_t *server_init(router_t *router, int port) {
  server_t *server = malloc(sizeof(server_t));
  if (!server) {
    char *message = "failed to allocate server";
    LOG("%s\n", message);
    DIE(EXIT_FAILURE, "%s\n", message);
  }

  server->router = router;
  server->port = port;

  return server;
}

bool server_start(server_t *server) {
  int port = server->port;
  int master_socket;

  if ((master_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    char *message =
        fmt_str("failed to initialize server socket on port %d", port);
    LOG("[server::start] %s\n", message);
    perror("socket");
    return false;
  }

  int yes = 1;
  // lose the pesky "Address already in use" error message (thanks, beej!)
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ==
      -1) {
    char *message = "failed to set sock opt";
    LOG("[server::start] %s\n", message);
    perror("setsockopt");
    return false;
  }

  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);

  memset((char *)&address, NULL_TERM, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    char *message = fmt_str("failed to bind server socket on %s:%d",
                            address.sin_addr, port);
    LOG("[server::start] %s\n", message);
    perror("bind");
    return false;
  }

  if (listen(master_socket, MAX_CONN) < 0) {
    char *message =
        fmt_str("failed to listen on %s:%d", address.sin_addr, port);
    LOG("[server::start] %s\n", message);
    perror("listen");
    return false;
  }

  printf("Listening on port %d...\n", port);

  fd_set readfds;
  int client_socket;

  thread_pool_t *pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    char *message = "failed to initialized thread pool";
    LOG("%s\n", message);
    DIE(EXIT_FAILURE, "%s\n", message);
  }

  const int num_threads = get_num_threads();
  thread_pool_init(pool);
  for (int i = 0; i < num_threads; i++) {
    thread_t *client_thread = thread_init(0, "client thread");
    // Make this a detached thread
    thread_set_attr(client_thread, false);
    thread_pool_insert(pool, client_thread);
  }

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(master_socket, &readfds);

    select(master_socket + 1, &readfds, NULL, NULL, NULL);
    if (FD_ISSET(master_socket, &readfds)) {
      if ((client_socket = accept(master_socket, (struct sockaddr *)&address,
                                  &addr_len)) < 0) {
        char *message = fmt_str("failed to accept client socket on %s:%d",
                                address.sin_addr, port);
        LOG("[server::start] %s\n", message);
        perror("accept");
        return false;
      }

      LOG("[server::start] %s\n",
          "accepted connection from new client; spawning handler thread...");

      client_context_t *c_ctx = malloc(sizeof(client_context_t));
      c_ctx->address = (struct sockaddr *)&address;
      c_ctx->addr_len = &addr_len;
      c_ctx->client_socket = client_socket;
      c_ctx->router = server->router;

      if (!thread_pool_dispatch(pool, client_thread_handler, c_ctx, true)) {
        LOG("[server::start] %s\n", "failed to dispatch thread from pool");
        DIE(EXIT_FAILURE, "%s\n", "failed to dispatch thread from pool");
      }
    }
  }

  // TODO: interrupt cancel, kill sig
  return true;
}

Response *response_init() {
  Response *response = malloc(sizeof(Response));
  if (!response) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate Response");
  }

  response->headers = ch_array_init();

  return response;
}

void send_response(int socket, Response *response_data) {
  Buffer *response = build_response(response_data);
  write(socket, buffer_state(response), buffer_size(response));
  buffer_free(response);
  close(socket);
}
