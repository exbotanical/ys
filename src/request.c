
#include "request.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>  // for read

#include "header.h"
#include "libhash/libhash.h"  // for hash sets
#include "libhttp.h"
#include "picohttpparser/picohttpparser.h"
#include "request.h"
#include "util.h"  // for safe_strcasecmp, str_equals
#include "xmalloc.h"

parameter_t* req_get_parameter_at(req_t* req, unsigned int idx) {
  return array_get(req->parameters, idx);
}

void* req_get_parameter(req_t* req, const char* key) {
  for (unsigned int i = 0; i < req_num_parameters(req); i++) {
    parameter_t* param = req_get_parameter_at(req, i);
    if (str_equals(param->key, key)) {
      return param->value;
    }
  }

  return NULL;
}

unsigned int req_num_parameters(req_t* req) {
  return array_size(req->parameters);
}

bool req_has_parameters(req_t* req) {
  return req && array_size(req->parameters) > 0;
}

// TODO: break up into two functions
req_meta_t read_and_parse_request(int sock) {
  const char *method, *path;
  char buf[REQ_BUFFER_SIZE];
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
  request->raw = buf;
  request->content_length = pret;
  request->method = fmt_str("%.*s", (int)method_len, method);
  request->path = fmt_str("%.*s", (int)path_len, path);
  request->version = fmt_str("1.%d\n", minor_version);

  request->headers = ht_init(0);
  // This is where we deal with the really quite complicated mess of HTTP
  // headers
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
    if (safe_strcasecmp(header_val, "Content-Type")) {
      request->content_type = header_val;
    } else if (safe_strcasecmp(header_val, "Accept")) {
      request->accept = header_val;
    } else if (safe_strcasecmp(header_val, "User-Agent")) {
      request->user_agent = header_val;
    }
  }

  req_meta_t meta = {.req = request};
  return meta;
}
