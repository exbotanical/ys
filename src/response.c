#include "response.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>  // for EXIT_FAILURE
#include <string.h>  // for strlen
#include <unistd.h>  // for ssize_t

#include "header.h"
#include "libutil/libutil.h"  // for s_equals, s_copy
#include "logger.h"           // for DIE
#include "xmalloc.h"

static const char CRLF[3] = "\r\n";

static bool is_2xx_connect(req_t *req, res_t *res) {
  return (res->status >= 200 && res->status < 300) &&
         s_equals(req->method, http_method_names[METHOD_CONNECT]);
}

static bool is_informational(res_t *res) {
  return res->status >= 100 && res->status < 200;
}

static bool is_nocontent(res_t *res) {
  return res->status == STATUS_NO_CONTENT;
}

static bool should_set_content_len(req_t *req, res_t *res) {
  return !is_nocontent(res) && !is_informational(res) &&
         !is_2xx_connect(req, res);
}

/**
 * res_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 * TODO: test
 * @param response
 * @return buffer_t*
 */
buffer_t *res_serialize(req_t *req, res_t *res) {
  buffer_t *rbuf = buffer_init(NULL);
  if (!rbuf) {
    DIE(EXIT_FAILURE, "[response::%s] could not allocate memory for buffer_t\n",
        __func__);
  }

  array_t *headers = res->headers;
  const int status = res->status;

  const char *body = res->body;

  buffer_append(rbuf,
                fmt_str("HTTP/1.1 %d %s", status, http_status_names[status]));
  buffer_append(rbuf, CRLF);

  foreach (headers, i) {
    header_t *header = array_get(headers, i);

    buffer_append(rbuf, fmt_str("%s: %s", header->key, header->value));
    buffer_append(rbuf, CRLF);
  }

  // TODO: test
  // A server MUST NOT send a Content-Length header field in any response
  // with a status code of 1xx (Informational) or 204 (No Content).  A
  // server MUST NOT send a Content-Length header field in any 2xx
  // (Successful) response to a METHOD_CONNECT request (Section 4.3.6 of
  // [RFC7231]).
  if (req && should_set_content_len(req, res)) {
    buffer_append(rbuf, fmt_str("Content-Length: %d", body ? strlen(body) : 0));
    buffer_append(rbuf, CRLF);
  }

  buffer_append(rbuf, CRLF);

  if (body) {
    buffer_append(rbuf, body);
  }

  return rbuf;
}

void res_send(int socket, buffer_t *response) {
  ssize_t total_sent = 0;

  while (total_sent < buffer_size(response)) {
    ssize_t sent = write(socket, buffer_state(response) + total_sent,
                         buffer_size(response) - total_sent);
    if (sent == -1) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        // Try again later
        continue;
      } else {
        printlogf(LOG_INFO,
                  "[response::%s] failed to send response on socket %d",
                  __func__, socket);
        printlogf(LOG_DEBUG, "[response::%s] full response body: %s\n",
                  __func__, buffer_state(response));
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

void res_preempterr(int socket, read_error_t err) {
  res_t *res = xmalloc(sizeof(res_t));
  res->headers = array_init();

  switch (err) {
    case IO_ERR:
    case PARSE_ERR:
      res->status = STATUS_INTERNAL_SERVER_ERROR;
      break;
    case DUP_HDR:
      res->status = STATUS_BAD_REQUEST;
      break;
    case REQ_TOO_LONG:
      res->status = STATUS_REQUEST_ENTITY_TOO_LARGE;
      break;
    default:
      res->status = STATUS_INTERNAL_SERVER_ERROR;
  }

  res_send(socket, res_serialize(NULL, res));
}

res_t *res_init() {
  res_t *res = xmalloc(sizeof(res_t));

  res->headers = array_init();
  res->body = NULL;
  res->status = STATUS_OK;  // default
  res->done = false;

  return res;
}

void set_body(res_t *response, const char *body) {
  response->body = s_copy(body);
}

void set_status(res_t *response, http_status_t status) {
  response->status = status;
}
