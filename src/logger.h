#ifndef LOGGER_H
#define LOGGER_H

#ifndef fprintf
#include <stdio.h>
#endif /* fprintf */

#ifdef DEBUG

#define LOG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#else

/**
 * @brief Debug logger. No-ops unless `DEBUG` is defined.
 */
#define LOG(fmt, ...)

#endif /* DEBUG */

/**
 * @brief User-facing error logger; prints to stderr.
 */
#define STDERR(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

/**
 * @brief Logs a user-facing error to stderr and exits with return code `rc`.
 */
#define DIE(rc, fmt, ...) STDERR(fmt, __VA_ARGS__), exit(rc)

#endif /* LOGGER_H */
