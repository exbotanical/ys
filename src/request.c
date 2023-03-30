
#include "request.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>  // for memset
#include <unistd.h>  // for read

#include "header.h"
#include "libhash/libhash.h"  // for hash sets
#include "libhttp.h"
#include "libutil/libutil.h"  // for s_casecmp, s_equals, fmt_str
#include "picohttpparser/picohttpparser.h"
#include "request.h"
#include "xmalloc.h"

static const char CACHE_CONTROL[14] = "Cache-Control";
static const char NO_CACHE[9] = "no-cache";
static const char PRAGMA[7] = "Pragma";
static const char CONTENT_TYPE[13] = "Content-Type";
static const char ACCEPT[7] = "Accept";
static const char USER_AGENT[11] = "User-Agent";

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
maybe_request req_read_and_parse(int sockfd) {
  char* method = NULL;
  char* path = NULL;

  char buf[REQ_BUFFER_SIZE];
  memset(buf, 0, REQ_BUFFER_SIZE);

  int pret, minor_version;
  struct phr_header headers[100];  // TODO: configure max headers
  size_t buflen = 0, prev_buflen = 0, method_len, path_len, num_headers;
  ssize_t bytes_read;

  while (true) {
    while ((bytes_read = read(sockfd, buf + buflen, sizeof(buf) - buflen)) ==
               -1 &&
           errno == EINTR)
      ;

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

  request* req = xmalloc(sizeof(request));
  req->raw = s_copy(buf);
  req->body = s_copy(buf + pret);
  req->content_length = pret;
  req->method = fmt_str("%.*s", (int)method_len, method);
  req->path = fmt_str("%.*s", (int)path_len, path);
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

    // Now we handle special metadata fields on the request that we expose to
    // the consumer for quick reference
    if (s_casecmp(header_val, CONTENT_TYPE)) {
      req->content_type = header_val;
    } else if (s_casecmp(header_val, ACCEPT)) {
      req->accept = header_val;
    } else if (s_casecmp(header_val, USER_AGENT)) {
      req->user_agent = header_val;
    }
  }

  fix_pragma_cache_control(req->headers);

  maybe_request meta = {.req = req};
  return meta;
}

char* req_get_parameter(request* req, const char* key) {
  if (!req->parameters) return NULL;
  return ht_get(req->parameters, key);
}

unsigned int req_num_parameters(request* req) {
  if (!req->parameters) return 0;
  return req->parameters->count;
}

bool req_has_parameters(request* req) {
  return req && req->parameters && req->parameters->count > 0;
}

char** req_get_query(request* req, const char* key) {
  array_t* arr = (array_t*)ht_get(req->queries, key);
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
  return req->queries && ht_search(req->queries, key);
}

unsigned int req_num_queries(request* req, const char* key) {
  if (req_has_query(req, key)) {
    return array_size((array_t*)ht_get(req->queries, key));
  }

  return 0;
}
