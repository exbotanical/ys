#include "logger.h"

#include <errno.h>
#include <fcntl.h>   // for open and open flags
#include <stdarg.h>  // for variadic args functions
#include <stdio.h>
#include <stdlib.h>  // for exit, getenv
#include <string.h>  // for strlen, strncmp
#include <time.h>    // time_t
#include <unistd.h>  // for write

#include "config.h"
#include "libutil/libutil.h"

#define LOG_BUFFER 2048   // max size of log line
#define SMALL_BUFFER 256  // small buffer size

#define LOG_IDENT "libhttp"
#define LOG_LEVEL LOG_NOTICE
#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"

#define EDWARD_THE_FOURTH 1461

// Standard syslog log levels
// TODO: use less levels
const char *log_levels[] = {"emerg",   "alert",  "crit", "err",
                            "warning", "notice", "info", "debug",
                            "panic",   "error",  "warn", NULL};

// Log level to use
short log_level = LOG_LEVEL;

char *gen_timestamp(void) {
  static char timestamp[sizeof("YYYY-MM-DD HH:MM:SS")];

  time_t now = time(NULL);
  if (now == (time_t)-1) {
    return NULL;  // Unable to get the current time
  }

  // So for some reason localtime, gmtime, and re-entrant variations thereof
  // aren't working (they cause some sort of segfault when the thread pool grabs
  // a mutex). TODO: fix this
  struct tm tm_utc;
  const int seconds_per_day = 24 * 60 * 60;
  int days_since_epoch = (int)(now / seconds_per_day);
  int seconds_today = (int)(now % seconds_per_day);
  int leap_years = (days_since_epoch - 4) / EDWARD_THE_FOURTH;
  int year = 1970 + 4 * leap_years;
  int day_of_year = days_since_epoch - leap_years * EDWARD_THE_FOURTH;

  if (day_of_year >= 366) {
    leap_years = (year - 1968) / 4;
    day_of_year -= 366;
    year += leap_years;
  }

  tm_utc.tm_year = year - 1900;
  tm_utc.tm_yday = day_of_year;
  tm_utc.tm_hour = seconds_today / 3600;
  tm_utc.tm_min = (seconds_today % 3600) / 60;
  tm_utc.tm_sec = seconds_today % 60;

  if (strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, &tm_utc) == 0) {
    return NULL;  // Unable to format the timestamp
  }

  return timestamp;
}

/**
 * fdprintf writes the data to a given file descriptor
 *
 * @param fd
 * @param fmt
 * @param ...
 */
static void fdprintf(int fd, const char *fmt, ...) {
  va_list va;
  char buf[LOG_BUFFER];

  va_start(va, fmt);
  vsnprintf(buf, sizeof(buf), fmt, va);
  write(fd, buf, strlen(buf));
  va_end(va);
}

/**
 * vlog writes the log message to the given file descriptor conditionally, based
 * on the specified log level
 *
 * @param level
 * @param fd
 * @param fmt
 * @param va
 */
static void vlog(int level, int fd, const char *fmt, va_list va) {
  if (level <= log_level) {
    char host_name[SMALL_BUFFER];

    // gethostname successful
    if (gethostname(host_name, sizeof(host_name)) == 0) {
      // result will be null-terminated unless gethostname had to truncate
      host_name[sizeof(host_name) - 1] = 0;
    } else {
      host_name[0] = 0;  // gethostname failed
    }

    char *header =
        fmt_str("%s %s %s\t\t", gen_timestamp(), host_name, LOG_IDENT);

    char buf[LOG_BUFFER];

    int buflen, header_len = 0;
    buf[0] = 0;  // in case strftime fails

    // snprintf null-terminates, even when truncating
    if ((header_len = snprintf(buf, strlen(header), header)) >=
        (int)strlen(header)) {
      header_len = strlen(header) - 1;
    }

    if ((buflen =
             vsnprintf(buf + header_len, sizeof(buf) - header_len, fmt, va) +
             header_len) >= (int)sizeof(buf)) {
      buflen = sizeof(buf) - 1;
    }

    if (server_conf.log_file) {
      write(fd, buf, buflen);
    } else {
      fprintf(stderr, buf);
    }
  }
}

/**
 * setup_log_level configures the log level global based on the level specified
 * in the server_conf (or default if none specified)
 */
static void setup_log_level() {
  const char *lvl = server_conf.log_level;

  unsigned int i;
  for (i = 0; log_levels[i]; i++) {
    if (strncmp(lvl, log_levels[i], strlen(log_levels[i])) == 0) {
      break;
    }
  }

  switch (i) {
    case 0:
    case 8:
      // system is unusable
      log_level = LOG_EMERG;
      break;
    case 1:
      // action must be taken immediately *]
      log_level = LOG_ALERT;
      break;
    case 2:
      // critical conditions *]
      log_level = LOG_CRIT;
      break;
    case 3:
    case 9:
      // error conditions
      log_level = LOG_ERR;
      break;
    case 4:
    case 10:
      // warning conditions
      log_level = LOG_WARNING;
      break;
    case 5:
      // normal but significant condition
      log_level = LOG_NOTICE;
      break;
    case 6:
      // informational
      log_level = LOG_INFO;
      break;
    case 7:
      // debug-level messages
      log_level = LOG_DEBUG;
      break;
  }
}

void setup_logging() {
  setup_log_level();

  if (server_conf.log_file) {
    int fd;
    if ((fd = open(server_conf.log_file, O_WRONLY | O_CREAT | O_APPEND,
                   0600)) >= 0) {
      // 2> log_file
      fclose(stderr);
      dup2(fd, 2);
    } else {
      int n = errno;
      fdprintf(2, "failed to open logfile '%s', reason: %s",
               server_conf.log_file, strerror(n));
      // TODO: ???
    }
  }
}

void printlogf(int level, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  vlog(level, 2, fmt, va);
  va_end(va);
}
