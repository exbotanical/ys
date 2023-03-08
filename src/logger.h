#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

#ifndef fprintf
#include <stdio.h>
#endif /* fprintf */

// TODO: clean up logging and make it consistent
#ifdef DEBUG
/**
 * @brief Debug stderr logger
 */
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

#define LOG_BUFFER 2048  /* max size of log line */
#define SMALL_BUFFER 256 /* small buffer size */

#define LOG_IDENT "libhttp"  // TODO: configurable

// TODO: allow compiler config -DTIMESTAMP_FMT=whatever
#define TIMESTAMP_FMT "%b %e %H:%M:%S"
#define LOG_HEADER TIMESTAMP_FMT " %%s " LOG_IDENT ": "
#define LOG_HEADER TIMESTAMP_FMT " %%s " LOG_IDENT ": "
#define LOCALE_LOG_HEADER "%c %%s " LOG_IDENT ": "

#define LOG_LEVEL LOG_NOTICE

extern const char* log_header;
extern const char* log_levels[];
extern short log_level;

#endif /* LOGGER_H */
