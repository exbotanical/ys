#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

#ifndef fprintf
#include <stdio.h>
#endif /* fprintf */

/**
 * Logs a user-facing error to stderr and exits with return code `rc`.
 */
#define DIE(rc, fmt, ...) printlogf(LOG_INFO, fmt, __VA_ARGS__), exit(rc)

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

/**
 * setup_logging configures logging based on the user config (e.g. stderr or
 * file)
 */
void setup_logging();

/**
 * printlogf prints at the specified log level to the log file descriptor the
 * given data
 *
 * @param level The log level at which to write the log (i.e. may be ignored)
 * @param fmt
 * @param ...
 */
void printlogf(int level, const char* fmt, ...);

#endif /* LOGGER_H */
