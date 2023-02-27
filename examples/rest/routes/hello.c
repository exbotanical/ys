#include <stdio.h>

typedef struct ch_array {
  char** state;
  size_t size;
} ch_array_t;

typedef struct response {
  int status;
  ch_array_t* headers;
  char* body;
} response_t;

void* GET(void* ctx) {
  printf("GET\n");

  response_t* response = malloc(sizeof(response_t*));
  response->body = "hi";
  response->headers = malloc(sizeof(ch_array_t*));
  response->status = 200;

  return response;
}
