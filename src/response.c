#include "response.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "header.h"
#include "libutil/libutil.h"
#include "logger.h"
#include "util.h"
#include "xmalloc.h"

#ifdef USE_TLS
#include <openssl/ssl.h>
#endif

#define REQ_BUFFER_SIZE 4096

static const char *CRLF = "\r\n";

static bool is_2xx_connect(request_internal *req, response_internal *res) {
  return (res->status >= 200 && res->status < 300) &&
         s_equals(req->method, http_method_names[METHOD_CONNECT]);
}

static bool is_informational(response_internal *res) {
  return res->status >= 100 && res->status < 200;
}

static bool is_nocontent(response_internal *res) {
  return res->status == STATUS_NO_CONTENT;
}

static bool should_set_content_len(request_internal *req,
                                   response_internal *res) {
  return !is_nocontent(res) && !is_informational(res) &&
         (req ? !is_2xx_connect(req, res) : true);
}

/**
 * response_serialize converts a user-defined response object into a buffer that
 * can be sent over the wire
 *
 * @param req
 * @param res
 * @return buffer_t*
 */
buffer_t *response_serialize(request_internal *req, response_internal *res) {
  buffer_t *buf = buffer_init(NULL);
  if (!buf) {
    DIE("[response::%s] could not allocate memory for buffer_t\n", __func__);
  }

  hash_table *headers = res->headers;
  const int status = res->status;
  const char *body = res->body;

  buffer_append(buf,
                fmt_str("HTTP/1.1 %d %s", status, http_status_names[status]));
  buffer_append(buf, CRLF);

  bool has_content_type;
  for (unsigned int i = 0; i < (unsigned int)headers->capacity; i++) {
    ht_record *header = headers->records[i];
    if (!header) continue;

    if (!has_content_type && s_casecmp(CONTENT_TYPE, header->key)) {
      has_content_type = true;
    }

    buffer_append(buf, fmt_str("%s: %s", header->key,
                               str_join((array_t *)header->value, ", ")));
    buffer_append(buf, CRLF);
  }

  // A server MUST NOT send a Content-Length header field in any response
  // with a status code of 1xx (Informational) or 204 (No Content).
  // A server MUST NOT send a Content-Length header field in any 2xx
  // (Successful) response to a METHOD_CONNECT request (Section 4.3.6 of
  // RFC7231).
  if (should_set_content_len(req, res)) {
    // Default to text/plain if we've a body and Content-Type not set by user
    if (body && !has_content_type) {
      buffer_append(buf, fmt_str("%s: %s", CONTENT_TYPE, MIME_TYPE_TXT));
      buffer_append(buf, CRLF);
    }

    buffer_append(buf, fmt_str("Content-Length: %d", body ? strlen(body) : 0));
    buffer_append(buf, CRLF);

    buffer_append(buf, CRLF);

    if (body) {
      buffer_append(buf, body);
    }
  } else {
    buffer_append(buf, CRLF);
  }

  return buf;
}

void response_send(client_context *ctx, buffer_t *buf) {
  ssize_t total_sent = 0;

  while (total_sent < buffer_size(buf)) {
    ssize_t sent;

#ifdef USE_TLS
    sent = SSL_write(ctx->ssl, buffer_state(buf) + total_sent,
                     buffer_size(buf) - total_sent);
#else
    sent = write(ctx->sockfd, buffer_state(buf) + total_sent,
                 buffer_size(buf) - total_sent);
#endif

    if (sent == -1) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
        // Try again later
        continue;
      }

      printlogf(LOG_INFO, "[response::%s] failed to send response on sockfd %d",
                __func__, ctx->sockfd);
      printlogf(LOG_DEBUG, "[response::%s] full response body: %s\n", __func__,
                buffer_state(buf));
      goto done;

    } else {
      total_sent += sent;
    }
  }

  goto done;

done:
  buffer_free(buf);

#ifdef USE_TLS
  SSL_shutdown(ctx->ssl);
  SSL_free(ctx->ssl);
#endif

  close(ctx->sockfd);

  return;
}

void response_send_error(client_context *ctx, parse_error err) {
  response_internal *res = response_init();

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

  response_send(ctx, response_serialize(NULL, res));

  free(ctx);
  free(res);
}

void response_send_protocol_error(int sockfd) {
  response_internal *res = response_init();

  res->status = STATUS_INTERNAL_SERVER_ERROR;
  res->body = "invalid protocol";

  buffer_t *resbuf = response_serialize(NULL, res);

  write(sockfd, buffer_state(resbuf), buffer_size(resbuf));
  close(sockfd);

  free(res);
  free(resbuf);
}

response_internal *response_init(void) {
  response_internal *res = xmalloc(sizeof(response_internal));

  res->headers = ht_init(0);
  res->body = NULL;
  res->status = STATUS_OK;  // Default
  res->done = false;

  // TODO: why must be here on macos (struct packing?)
  insert_header(res->headers, "X-Powered-By", "Ys", false);

  return res;
}

void set_body(response *res, const char *body) {
  ((response_internal *)res)->body = s_copy(body);
}

void set_status(response *res, http_status status) {
  ((response_internal *)res)->status = status;
}

bool get_done(response *res) { return ((response_internal *)res)->done; }

void set_done(response *res) { ((response_internal *)res)->done = true; }

char *get_header(response *res, const char *key) {
  return get_first_header(((response_internal *)res)->headers, key);
}
