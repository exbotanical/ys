
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

/**
 * fix_pragma_cache_control implements RFC 7234, section 5.4:
 * Should treat Pragma: no-cache like Cache-Control: no-cache
 * @param header
 */
static void fix_pragma_cache_control(hash_table* headers) {
  if (s_equals(req_header_get(headers, "Pragma"), "no-cache")) {
    if (!ht_search(headers, "Cache-Control")) {
      insert_header(headers, "Cache-Control", "no-cache");
    }
  }
}

// TODO: break up into two functions
req_meta_t req_read_and_parse(int sock) {
  char* method = NULL;
  char* path = NULL;
  char buf[REQ_BUFFER_SIZE];
  memset(buf, 0, REQ_BUFFER_SIZE);

  int pret, minor_version;
  struct phr_header headers[100];
  size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
  ssize_t rret;

  while (true) {
    while ((rret = read(sock, buf + buflen, sizeof(buf) - buflen)) == -1 &&
           errno == EINTR)
      ;

    if (rret <= 0) {
      req_meta_t meta = {.err = IO_ERR};
      return meta;
    }

    prevbuflen = buflen;
    buflen += rret;

    num_headers = sizeof(headers) / sizeof(headers[0]);
    pret =
        phr_parse_request(buf, buflen, &method, &method_len, &path, &path_len,
                          &minor_version, headers, &num_headers, prevbuflen);
    if (pret > 0) {
      break;  // successfully parsed the request
    }

    if (pret == -1) {
      req_meta_t meta = {.err = PARSE_ERR};
      return meta;
    }

    // request is incomplete
    if (pret == -2) {
      continue;
    }

    if (buflen == sizeof(buf)) {
      req_meta_t meta = {.err = REQ_TOO_LONG};
      return meta;
    }
  }

  req_t* request = xmalloc(sizeof(req_t));
  request->raw = s_copy(buf);
  request->body = s_copy(buf + pret);
  request->content_length = pret;
  request->method = fmt_str("%.*s", (int)method_len, method);
  request->path = fmt_str("%.*s", (int)path_len, path);
  request->version = fmt_str("1.%d\n", minor_version);

  // This is where we deal with the really quite complicated mess of HTTP
  // headers
  request->headers = ht_init(0);

  for (unsigned int i = 0; i != num_headers; ++i) {
    char* header_key =
        fmt_str("%.*s", (int)headers[i].name_len, headers[i].name);
    char* header_val =
        fmt_str("%.*s", (int)headers[i].value_len, headers[i].value);

    if (!insert_header(request->headers, header_key, header_val)) {
      req_meta_t meta = {.err = DUP_HDR};
      return meta;
    }

    // Now we handle special metadata fields on the request that we expose to
    // the consumer for quick reference
    if (s_casecmp(header_val, "Content-Type")) {
      request->content_type = header_val;
    } else if (s_casecmp(header_val, "Accept")) {
      request->accept = header_val;
    } else if (s_casecmp(header_val, "User-Agent")) {
      request->user_agent = header_val;
    }
  }

  fix_pragma_cache_control(request->headers);

  req_meta_t meta = {.req = request};
  return meta;
}

char* req_get_parameter(req_t* req, const char* key) {
  if (!req->parameters) return NULL;
  return ht_get(req->parameters, key);
}

unsigned int req_num_parameters(req_t* req) {
  if (!req->parameters) return 0;
  return req->parameters->count;
}

bool req_has_parameters(req_t* req) {
  return req && req->parameters && req->parameters->count > 0;
}

char** req_get_query(req_t* req, const char* key) {
  array_t* arr = ht_get(req->queries, key);
  if (arr) {
    char** values = malloc(array_size(arr));
    foreach (arr, i) {
      values[i] = array_get(arr, i);
    }
    return values;
  }

  return NULL;
}

bool req_has_query(req_t* req, const char* key) {
  return req->queries && ht_search(req->queries, key);
}

unsigned int req_num_queries(req_t* req, const char* key) {
  if (req_has_query(req, key)) {
    return array_size((array_t*)ht_get(req->queries, key));
  }

  return 0;
}
