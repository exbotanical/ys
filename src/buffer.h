#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE_INIT 16

typedef struct buffer {
  char* state;
  int len;
} buffer_t;

buffer_t *buffer_init();

void buffer_append(buffer_t *buf, const char* s);

void buffer_free(buffer_t *buf);

#endif
