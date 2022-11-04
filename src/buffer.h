#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

/**
 * @brief TODO: doc
 *
 */
#define BUFFER_SIZE_INIT 16

/**
 * @brief TODO: doc
 *
 */
typedef struct buffer {
  char *state;
  int len;
} buffer_t;

/**
 * @brief Allocates memory for a new buffer.
 *
 * @return buffer_t*
 */
buffer_t *buffer_init();

/**
 * @brief Appends a string to a given buffer `buf`, reallocating the required memory
 *
 * @param buf the buffer to which `s` will be appended
 * @param s char pointer to be appended to the buffer
 */
bool buffer_append(buffer_t *buf, const char *s);

/**
 * @brief Deallocate the dynamic memory used by an `buffer_t`
 *
 * @param buf the buffer pointer
 */
void buffer_free(buffer_t *buf);

#endif
