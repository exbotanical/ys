#include "response.h"

#include <stdlib.h>  // for EXIT_FAILURE
#include <string.h>  // for strdup

#include "logger.h"  // for DIE

res_t *response_init() {
  res_t *res = malloc(sizeof(res_t));
  if (!res) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate res_t");
  }

  res->headers = array_init();
  if (!res->headers) {
    DIE(EXIT_FAILURE, "%s\n", "unable to allocate array_t");
  }

  return res;
}

bool set_header(res_t *response, char *header) {
  return array_push(response->headers, header);
}

void set_body(res_t *response, const char *body) {
  response->body = strdup(body);
}

void set_status(res_t *response, http_status_t status) {
  response->status = status;
}

/**
 * serialize_response converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param response
 * @return buffer_t*
 */
buffer_t *serialize_response(res_t *response) {
  buffer_t *rbuf = buffer_init(NULL);
  if (!rbuf) {
    // TODO: constant template str for malloc failures
    DIE(EXIT_FAILURE, "%s\n", "could not allocate memory for buffer_t");
  }

  // TODO: constants for response fields for brevity
  int status = response->status;
  array_t *headers = response->headers;
  char *body = response->body;

  buffer_append(rbuf,
                fmt_str("HTTP/1.1 %d %s\n", status, http_status_names[status]));

  for (unsigned int i = 0; i < array_size(headers); i++) {
    char *header = array_get(headers, i);

    buffer_append(rbuf, header);
    buffer_append(rbuf, "\n");
  }

  buffer_append(rbuf,
                fmt_str("Content-Length: %d\n\n", body ? strlen(body) : 0));
  buffer_append(rbuf, body ? body : "");

  return rbuf;
}

void send_response(int socket, res_t *response_data) {
  buffer_t *response = serialize_response(response_data);
  write(socket, buffer_state(response), buffer_size(response));
  buffer_free(response);
  close(socket);
}
