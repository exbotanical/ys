#include "request.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "header.h"
#include "libhash/libhash.h"
#include "libhttp.h"
#include "libutil/libutil.h"
#include "path.h"
#include "picohttpparser/picohttpparser.h"
#include "request.h"
#include "xmalloc.h"

#ifdef USE_TLS
#include <openssl/ssl.h>
#endif

#define REQ_BUFFER_SIZE 4096

/**
 * fix_pragma_cache_control implements RFC 7234, section 5.4:
 * Should treat Pragma: no-cache like Cache-Control: no-cache
 * @param header
 */
static void fix_pragma_cache_control(hash_table* headers) {
  if (s_equals(get_header(headers, PRAGMA), NO_CACHE)) {
    if (!ht_search(headers, CACHE_CONTROL)) {
      insert_header(headers, CACHE_CONTROL, NO_CACHE);
    }
  }
}

// TODO: break up into two functions
maybe_request req_read_and_parse(client_context* ctx) {
  char* method = NULL;
  char* path = NULL;

  char buf[REQ_BUFFER_SIZE];
  memset(buf, 0, REQ_BUFFER_SIZE);

  int pret, minor_version;
  struct phr_header headers[100];  // TODO: configure max headers
  size_t buflen = 0, prev_buflen = 0, method_len, path_len, num_headers;
  ssize_t bytes_read;

  while (true) {
#ifdef USE_TLS
    while ((bytes_read =
                SSL_read(ctx->ssl, buf + buflen, sizeof(buf) - buflen)) == -1 &&
           errno == EINTR)
      ;
#else
    while ((bytes_read =
                read(ctx->sockfd, buf + buflen, sizeof(buf) - buflen)) == -1 &&
           errno == EINTR)
      ;
#endif

    if (bytes_read <= 0) {
      maybe_request meta = {.err = IO_ERR};
      return meta;
    }

    prev_buflen = buflen;
    buflen += bytes_read;

    num_headers = sizeof(headers) / sizeof(headers[0]);
    pret =
        phr_parse_request(buf, buflen, &method, &method_len, &path, &path_len,
                          &minor_version, headers, &num_headers, prev_buflen);

    if (pret > 0) {
      break;  // Successfully parsed the request
    }

    if (pret == -1) {
      maybe_request meta = {.err = PARSE_ERR};
      return meta;
    }

    // Request is incomplete
    if (pret == -2) {
      continue;
    }

    if (buflen == sizeof(buf)) {
      maybe_request meta = {.err = REQ_TOO_LONG};
      return meta;
    }
  }

  request_internal* req = xmalloc(sizeof(request_internal));
  req->raw = s_copy(buf);
  req->body = s_copy(buf + pret);
  req->method = fmt_str("%.*s", (int)method_len, method);
  req->path = fmt_str("%.*s", (int)path_len, path);
  req->route_path = s_copy(req->path);
  req->pure_path = path_get_pure(req->path);
  req->version = fmt_str("1.%d\n", minor_version);

  // This is where we deal with the really quite complicated mess of HTTP
  // headers
  req->headers = ht_init(0);
  for (unsigned int i = 0; i != num_headers; ++i) {
    char* header_key =
        fmt_str("%.*s", (int)headers[i].name_len, headers[i].name);
    char* header_val =
        fmt_str("%.*s", (int)headers[i].value_len, headers[i].value);

    if (!insert_header(req->headers, header_key, header_val)) {
      maybe_request meta = {.err = DUP_HDR};
      return meta;
    }
  }

  fix_pragma_cache_control(req->headers);

  maybe_request meta = {.req = req};
  return meta;
}

char* req_get_parameter(request* req, const char* key) {
  request_internal* ri = (request_internal*)req;

  if (!ri->parameters) return NULL;
  return ht_get(ri->parameters, key);
}

unsigned int req_num_parameters(request* req) {
  request_internal* ri = (request_internal*)req;

  if (!ri->parameters) return 0;
  return ri->parameters->count;
}

bool req_has_parameters(request* req) {
  request_internal* ri = (request_internal*)req;

  return req && ri->parameters && ri->parameters->count > 0;
}

char** req_get_query(request* req, const char* key) {
  array_t* arr = (array_t*)ht_get(((request_internal*)req)->queries, key);
  if (arr) {
    char** values = xmalloc(array_size(arr));
    foreach (arr, i) {
      values[i] = array_get(arr, i);
    }
    return values;
  }

  return NULL;
}

bool req_has_query(request* req, const char* key) {
  request_internal* ri = (request_internal*)req;
  return ri->queries && ht_search(ri->queries, key);
}

unsigned int req_num_queries(request* req, const char* key) {
  if (req_has_query(req, key)) {
    return array_size((array_t*)ht_get(((request_internal*)req)->queries, key));
  }

  return 0;
}

char* request_get_path(request* req) {
  return s_copy(((request_internal*)req)->path);
}

char* request_get_route_path(request* req) {
  return s_copy(((request_internal*)req)->route_path);
}

char* request_get_method(request* req) {
  return s_copy(((request_internal*)req)->method);
}

char* request_get_body(request* req) {
  return s_copy(((request_internal*)req)->body);
}

char* request_get_raw(request* req) {
  return s_copy(((request_internal*)req)->raw);
}

char* request_get_protocol(request* req) {
  return s_copy(((request_internal*)req)->protocol);
}

char* request_get_version(request* req) {
  return s_copy(((request_internal*)req)->version);
}

char* request_get_header(request* req, const char* key) {
  return get_header(((request_internal*)req)->headers, key);
}
