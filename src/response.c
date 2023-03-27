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

static bool is_2xx_connect(request *req, response *res) {
  return (res->status >= 200 && res->status < 300) &&
         s_equals(req->method, http_method_names[METHOD_CONNECT]);
}

static bool is_informational(response *res) {
  return res->status >= 100 && res->status < 200;
}

static bool is_nocontent(response *res) {
  return res->status == STATUS_NO_CONTENT;
}

static bool should_set_content_len(request *req, response *res) {
  return !is_nocontent(res) && !is_informational(res) &&
         !is_2xx_connect(req, res);
}

/**
 * res_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req
 * @param res
 * @return buffer_t*
 *
 * TODO: test
 */
buffer_t *res_serialize(request *req, response *res) {
  buffer_t *buf = buffer_init(NULL);
  if (!buf) {
    DIE(EXIT_FAILURE, "[response::%s] could not allocate memory for buffer_t\n",
        __func__);
  }

  array_t *headers = res->headers;
  const int status = res->status;
  const char *body = res->body;

  buffer_append(buf,
                fmt_str("HTTP/1.1 %d %s", status, http_status_names[status]));
  buffer_append(buf, CRLF);

  foreach (headers, i) {
    header_pair *header = array_get(headers, i);

    buffer_append(buf, fmt_str("%s: %s", header->key, header->value));
    buffer_append(buf, CRLF);
  }

  // A server MUST NOT send a Content-Length header field in any response
  // with a status code of 1xx (Informational) or 204 (No Content).  A
  // server MUST NOT send a Content-Length header field in any 2xx
  // (Successful) response to a METHOD_CONNECT request (Section 4.3.6 of
  // [RFC7231]).

  // TODO: test
  if (req && should_set_content_len(req, res)) {
    buffer_append(buf, fmt_str("Content-Length: %d", body ? strlen(body) : 0));
    buffer_append(buf, CRLF);
  }

  buffer_append(buf, CRLF);

  if (body) {
    buffer_append(buf, body);
  }

  return buf;
}

void res_send(int sockfd, buffer_t *resbuf) {
  ssize_t total_sent = 0;

  while (total_sent < buffer_size(resbuf)) {
    ssize_t sent = write(sockfd, buffer_state(resbuf) + total_sent,
                         buffer_size(resbuf) - total_sent);
    if (sent == -1) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        // Try again later
        continue;
      }

      printlogf(LOG_INFO, "[response::%s] failed to send response on sockfd %d",
                __func__, sockfd);
      printlogf(LOG_DEBUG, "[response::%s] full response body: %s\n", __func__,
                buffer_state(resbuf));
      goto done;

    } else {
      total_sent += sent;
    }
  }

  goto done;

done:
  buffer_free(resbuf);
  close(sockfd);

  return;
}

void res_preempterr(int sockfd, parse_error err) {
  response *res = res_init();

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

  res_send(sockfd, res_serialize(NULL, res));
}

response *res_init() {
  response *res = xmalloc(sizeof(response));

  res->headers = array_init();
  res->body = NULL;
  res->status = STATUS_OK;  // Default
  res->done = false;

  return res;
}

void set_body(response *response, const char *body) {
  response->body = s_copy(body);
}

void set_status(response *response, http_status status) {
  response->status = status;
}
