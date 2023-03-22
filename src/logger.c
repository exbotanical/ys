#include "logger.h"

#include <errno.h>
#include <fcntl.h>   // for open and open flags
#include <stdarg.h>  // for variadic args functions
#include <stdlib.h>  // for exit, getenv
#include <string.h>  // for strlen, strncmp
#include <time.h>    // time_t
#include <unistd.h>  // for write

#include "config.h"
#include "libutil/libutil.h"

const char *log_header = LOG_HEADER;

// Standard syslog log levels
const char *log_levels[] = {"emerg",   "alert",  "crit", "err",
                            "warning", "notice", "info", "debug",
                            "panic",   "error",  "warn", NULL};

// Log level to use
short log_level = LOG_LEVEL;
#define TIMESTAMP_FORMAT "%04d-%02d-%02dT%02d:%02d:%02dZ"
#define TIMESTAMP_SIZE sizeof("YYYY-MM-DDTHH:MM:SSZ")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTAMP_FORMAT "%04d-%02d-%02dT%02d:%02d:%02dZ"
#define TIMESTAMP_SIZE sizeof("YYYY-MM-DDTHH:MM:SSZ")

char *gen_timestamp(void) {
  time_t utc_time = time(NULL);
  if (utc_time == ((time_t)-1)) {
    return NULL;  // Error: Unable to get UTC time
  }

  struct tm utc_tm;
  memset(&utc_tm, 0, sizeof(struct tm));
  int leap_years = 0;
  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  int seconds = (int)utc_time;
  utc_tm.tm_sec = seconds % 60;
  utc_tm.tm_min = (seconds / 60) % 60;
  utc_tm.tm_hour = (seconds / 3600) % 24;

  int days = seconds / 86400;
  int year = 1970;
  int month = 0;
  int day = 0;
  while (days >= 365 + leap_years) {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
      leap_years++;
    }
    days -= 365 + leap_years;
    year++;
  }
  utc_tm.tm_year = year - 1900;

  while (month < 12 &&
         days >= days_in_month[month] +
                     ((month == 1 && (year % 4 == 0 && year % 100 != 0) ||
                       year % 400 == 0))) {
    days -=
        days_in_month[month] +
        ((month == 1 && (year % 4 == 0 && year % 100 != 0) || year % 400 == 0));
    month++;
  }
  utc_tm.tm_mon = month;
  utc_tm.tm_mday = days + 1;

  char *timestamp = malloc(TIMESTAMP_SIZE);
  if (!timestamp) {
    return NULL;  // Error: Unable to allocate memory for timestamp string
  }

  int len = snprintf(timestamp, TIMESTAMP_SIZE, TIMESTAMP_FORMAT,
                     utc_tm.tm_year + 1900, utc_tm.tm_mon + 1, utc_tm.tm_mday,
                     utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec);
  if (len < 0 || len >= TIMESTAMP_SIZE) {
    free(timestamp);
    return NULL;  // Error: Unable to create timestamp string
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

    if (gethostname(host_name, sizeof(host_name)) == 0) {
      // gethostname successful
      // result will be \0-terminated except gethostname doesn't promise
      // to do so if it has to truncate
      host_name[sizeof(host_name) - 1] = 0;
    } else {
      host_name[0] = 0;  // gethostname() call failed
    }

    char *header = fmt_str("%s %s %s\t", gen_timestamp(), host_name, LOG_IDENT);

    char buf[LOG_BUFFER];

    int buflen, header_len = 0;
    buf[0] = 0;  // in case suppress_header or strftime fails

    // snprintf null-terminates, even when truncating
    // retval >= size means result was truncated
    if ((header_len = snprintf(buf, strlen(header), header)) >=
        (int)strlen(header)) {
      header_len = strlen(header) - 1;
    }

    if ((buflen =
             vsnprintf(buf + header_len, sizeof(buf) - header_len, fmt, va) +
             header_len) >= (int)sizeof(buf)) {
      buflen = sizeof(buf) - 1;
    }

    if (server_config.log_file) {
      write(fd, buf, buflen);
    } else {
      fprintf(stderr, buf);
    }
  }
}

/**
 * setup_log_level configures the log level global based on the level specified
 * in the server_config (or default if none specified)
 */
static void setup_log_level() {
  const char *ptr = server_config.log_level;
  int j;
  for (j = 0; log_levels[j]; ++j) {
    if (strncmp(ptr, log_levels[j], strlen(log_levels[j])) == 0) {
      break;
    }
  }

  switch (j) {
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

  if (getenv("LC_TIME") != NULL) {
    log_header = LOCALE_LOG_HEADER;
  }

  // TODO: stdout opt
  if (server_config.log_file) {
    int fd;
    if ((fd = open(server_config.log_file, O_WRONLY | O_CREAT | O_APPEND,
                   0600)) >= 0) {
      /* 2> log_file */
      fclose(stderr);
      dup2(fd, 2);
    } else {
      int n = errno;
      fdprintf(2, "failed to open logfile '%s', reason: %s",
               server_config.log_file, strerror(n));
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
