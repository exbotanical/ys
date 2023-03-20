#include "response.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>  // for EXIT_FAILURE
#include <string.h>  // for strdup
#include <unistd.h>  // for ssize_t

#include "logger.h"  // for DIE
#include "util.h"    // for str_equals
#include "xmalloc.h"

static const char MESSAGE_BODY_SEP[3] = "\n\n";

static bool is_2xx_connect(req_t *req, res_t *res) {
  return (res->status >= 200 && res->status < 300) &&
         str_equals(req->method, http_method_names[METHOD_CONNECT]);
}

static bool is_informational(res_t *res) {
  return res->status >= 100 && res->status < 200;
}

static bool is_nocontent(res_t *res) { return res->status == NO_CONTENT; }

static bool should_set_content_len(req_t *req, res_t *res) {
  return !is_nocontent(res) && !is_informational(res) &&
         !is_2xx_connect(req, res);
}

/**
 * serialize_response converts a user-defined response object into a buffer that
 * can be sent over the wire
 * TODO: test
 * @param response
 * @return buffer_t*
 */
buffer_t *serialize_response(req_t *req, res_t *res) {
  buffer_t *rbuf = buffer_init(NULL);
  if (!rbuf) {
    // TODO: constant template str for malloc failures
    DIE(EXIT_FAILURE, "%s\n", "could not allocate memory for buffer_t");
  }

  // TODO: constants for res fields for brevity
  int status = res->status;
  array_t *headers = res->headers;
  char *body = res->body;

  buffer_append(rbuf,
                fmt_str("HTTP/1.1 %d %s\n", status, http_status_names[status]));

  for (unsigned int i = 0; i < array_size(headers); i++) {
    char *header = array_get(headers, i);

    buffer_append(rbuf, header);
    buffer_append(rbuf, "\n");
  }

  // TODO: test
  // A server MUST NOT send a Content-Length header field in any response
  // with a status code of 1xx (Informational) or 204 (No Content).  A
  // server MUST NOT send a Content-Length header field in any 2xx
  // (Successful) response to a METHOD_CONNECT request (Section 4.3.6 of
  // [RFC7231]).
  if (should_set_content_len(req, res)) {
    buffer_append(rbuf, fmt_str("Content-Length: %d", body ? strlen(body) : 0));
  }

  buffer_append(rbuf, MESSAGE_BODY_SEP);

  if (body) {
    buffer_append(rbuf, body);
  }

  return rbuf;
}

void send_response(int socket, buffer_t *response) {
  ssize_t total_sent = 0;

  while (total_sent < buffer_size(response)) {
    ssize_t sent = write(socket, buffer_state(response) + total_sent,
                         buffer_size(response) - total_sent);
    if (sent == -1) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        // Try again later
        continue;
      } else {
        printlogf(
            LOG_INFO,
            "[response::send_response] failed to send response on socket %d",
            socket);
        printlogf(LOG_DEBUG,
                  "[response::send_response] full response body: %s\n",
                  buffer_state(response));
        goto done;
        return;
      }
    } else {
      total_sent += sent;
    }
  }

  goto done;

done:
  buffer_free(response);
  close(socket);
}

res_t *response_init() {
  res_t *res = xmalloc(sizeof(res_t));

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
