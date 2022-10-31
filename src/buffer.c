#include "buffer.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

buffer_t* buffer_init() {
  buffer_t* buf = malloc(sizeof(buffer_t));
	// Memory insufficient
  if (NULL == buf) {
		goto buffer_new_fail;
	}

  // // allocate space for the string itself
  // buf->state = malloc(BUFFER_SIZE_INIT);
  // //quit and cleanup if this failed
  // if (NULL == buf->state) {
	// 	goto buffer_new_fail;
	// }


  // buf->len = BUFFER_SIZE_INIT;

  return buf;

buffer_new_fail:
	free(buf);
	return NULL;
}


/**
 * @brief Takes a string buffer and appends a next string, reallocating the required memory (caller must invoke `free`)
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 */
void buffer_extend(buffer_t* buf, const char* s) {
	int len = strlen(s); // TODO: null check
  // get mem sizeof current str + sizeof append str
  char* next = realloc(buf->state, buf->len + len);

  if (!next) {
		return;
	}

  memcpy(&next[buf->len], s, len);
  buf->state = next;
  buf->len += len;
}

/**
 * @brief Deallocate the dynamic memory used by an `buffer_t`
 *
 * @param buf the buffer pointer
 */
void buffer_free(buffer_t* buf) {
  free(buf->state);
}
