#include "request.h"

#include <stdio.h>

#include "libhttp.h"

int main(int argc, char const *argv[]) {
  char request_str[] =
      "POST /api/users HTTP/1.1\r\n"
      "Host: example.com\r\n"
      "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:59.0) "
      "Gecko/20100101 Firefox/59.0\r\n"
      "Content-Type: application/json\r\n"
      "Content-Length: 29\r\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;"
      "Accept-Language: en-US,en;q=0.5\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Connection: keep-alive\r\n"
      "Upgrade-Insecure-Requests: 1\r\n"
      "\r\n"
      "{\"username\":\"alice\",\"password\":\"secret\"}";

  // req_t *r = parse_request(request_str);

  // printf("accept: %s\n", r->accept);
  // printf("body: %s\n", r->body);
  // printf("content_length: %d\n", r->content_length);
  // printf("content_type: %s\n", r->content_type);
  // printf("host: %s\n", r->host);
  // printf("method: %s\n", r->method);
  // printf("path: %s\n", r->path);
  // printf("protocol: %s\n", r->protocol);
  // printf("version: %s\n", r->version);

  // printf("raw: %s\n", r->raw);
}
