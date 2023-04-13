#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>

#define YS_LOG_INFO 6
#define YS_LOG_DEBUG 7
#define YS_LOG_VERBOSE 10

/**
 * Logs a user-facing error to stderr and exits with return code `rc`.
 */
#define DIE(...) printlogf(LOG_INFO, __VA_ARGS__), exit(EXIT_FAILURE)

extern const char* log_header;
extern const char* log_levels[];
extern short log_level;

/**
 * setup_logging configures logging based on the user config (e.g. stderr or
 * file)
 */
void setup_logging(void);

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
