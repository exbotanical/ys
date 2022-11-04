#include "buffer.h"

#include "util.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

buffer_t *buffer_init() {
  buffer_t *buf = malloc(sizeof(buffer_t));
  if (buf == NULL) {
		free(buf);

		LOG("[buffer::buffer_init] failed to allocate buffer_t\n");

		return NULL;
	}

  return buf;
}

bool buffer_append(buffer_t *buf, const char *s) {
	int len = strlen(s);

  // get mem sizeof current str + sizeof append str
  char *next = realloc(buf->state, buf->len + len);
  if (!next) {
		free(next);
		LOG("[buffer::buffer_append] failed to reallocate buf->state\n");

		return false;
	}

  memcpy(&next[buf->len], s, len);
  buf->state = next;
  buf->len += len;

	return true;
}

void buffer_free(buffer_t *buf) {
	// Because buffer_t's state member is initialized lazily, we need only dealloc
	// if it was actually allocated to begin with
	if (buf->len) {
		free(buf->state);
		buf->state = NULL;
	}

	free(buf);
}
