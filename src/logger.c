#include "logger.h"

#include <errno.h>
#include <fcntl.h>   // for open and open flags
#include <stdarg.h>  // for variadic args functions
#include <stdlib.h>  // for exit, getenv
#include <string.h>  // for strlen, strncmp
#include <time.h>    // time_t
#include <unistd.h>  // for write

#include "config.h"

const char *log_header = LOG_HEADER;

// Standard syslog log levels
const char *log_levels[] = {"emerg",   "alert",  "crit", "err",
                            "warning", "notice", "info", "debug",
                            "panic",   "error",  "warn", NULL};

short log_level = LOG_LEVEL;

static void fdprintf(int fd, const char *ctl, ...) {
  va_list va;
  char buf[LOG_BUFFER];

  va_start(va, ctl);
  vsnprintf(buf, sizeof(buf), ctl, va);
  write(fd, buf, strlen(buf));
  va_end(va);
}

static void vlog(int level, int fd, const char *ctl, va_list va) {
  char buf[LOG_BUFFER];

  if (level <= log_level) {
    char host_name[SMALL_BUFFER];
    static short suppress_header = 0;

    time_t t = time(NULL);
    struct tm *tp = localtime(&t);
    int buflen, header_len = 0;
    buf[0] = 0;  // in case suppress_header or strftime fails

    if (!suppress_header) {
      char header[SMALL_BUFFER];
      // strftime returns strlen of result, provided that result plus a \0
      // fit into buf of size
      if (strftime(header, sizeof(header), log_header, tp)) {
        if (gethostname(host_name, sizeof(host_name)) == 0) {
          // gethostname successful
          // result will be \0-terminated except gethostname doesn't promise
          // to do so if it has to truncate
          host_name[sizeof(host_name) - 1] = 0;
        } else {
          host_name[0] = 0;  // gethostname() call failed
        }
        // snprintf null-terminates, even when truncating
        // retval >= size means result was truncated
        if ((header_len = snprintf(buf, sizeof(header), header, host_name)) >=
            sizeof(header)) {
          header_len = sizeof(header) - 1;
        }
      }
    }

    if ((buflen =
             vsnprintf(buf + header_len, sizeof(buf) - header_len, ctl, va) +
             header_len) >= sizeof(buf)) {
      buflen = sizeof(buf) - 1;
    }

    if (server_config.log_file) {
      write(fd, buf, buflen);
    }
    printf(buf);
    // if previous write wasn't \n-terminated, we suppress header on next
    // write
    suppress_header = (buf[buflen - 1] != '\n');
  }
}

static void setup_log_level() {
  char *ptr = server_config.log_level;
  int j;
  for (j = 0; log_levels[j]; ++j) {
    if (strncmp(ptr, log_levels[j], strlen(log_levels[j])) == 0) {
      break;
    }
  }

  switch (j) {
    case 0:
    case 8:
      /* #define	LOG_EMERG	0	[* system is unusable *] */
      log_level = LOG_EMERG;
      break;
    case 1:
      /* #define	LOG_ALERT	1	[* action must be taken
       * immediately *] */
      log_level = LOG_ALERT;
      break;
    case 2:
      /* #define	LOG_CRIT	2	[* critical conditions *] */
      log_level = LOG_CRIT;
      break;
    case 3:
    case 9:
      /* #define	LOG_ERR		3	[* error conditions *] */
      log_level = LOG_ERR;
      break;
    case 4:
    case 10:
      /* #define	LOG_WARNING	4	[* warning conditions *] */
      log_level = LOG_WARNING;
      break;
    case 5:
      /* #define	LOG_NOTICE	5	[* normal but significant
       * condition *] */
      log_level = LOG_NOTICE;
      break;
    case 6:
      /* #define	LOG_INFO	6	[* informational *] */
      log_level = LOG_INFO;
      break;
    case 7:
      /* #define	LOG_DEBUG	7	[* debug-level messages *] */
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
      exit(n);  // TODO: die
    }
  }
}

void printlogf(int level, const char *ctl, ...) {
  va_list va;
  va_start(va, ctl);
  vlog(level, 2, ctl, va);
  va_end(va);
}