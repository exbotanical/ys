#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "util.h"

// Maximum number of queued connections allowed for server
const short MAX_QUEUED_CONNECTIONS = 100;

// Max line length for config file
static const int CONFIG_MAX_LINE_LEN = 256;

// Default number of threads to use in the server thread pool
static const short DEFAULT_NUM_THREADS = 4;

// Default port number
static const int DEFAULT_PORT_NUM = 5000;

// Default log level
static char DEFAULT_LOG_LEVEL[] = "info";

// Environment variable key for user-defined number of threads
static const char NUM_THREADS_KEY[] = "NUM_THREADS";

// Environment variable key for user-defined port number
static const char SERVER_PORT_KEY[] = "PORT";

// Environment variable key for user-defined log level
static const char LOG_LEVEL_KEY[] = "LOG_LEVEL";

// Environment variable key for user-defined log file path
static const char LOG_FILE_KEY[] = "LOG_FILE";

/**
 * Default server config
 */
server_config server_conf = {.log_file = NULL,
                             .log_level = DEFAULT_LOG_LEVEL,
                             .threads = DEFAULT_NUM_THREADS,
                             .port = DEFAULT_PORT_NUM};

bool parse_config(const char* filename) {
  bool ret = false;
  char line[CONFIG_MAX_LINE_LEN];

  if (access(filename, F_OK) == -1) {
    printlogf(YS_LOG_DEBUG, "Config file %s does not exist. Using defaults\n",
              filename);
    return true;
  }

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printlogf(YS_LOG_INFO, "Error opening config file '%s'\n", filename);

    return ret;
  }

  int port = 0;
  int threads = 0;
  char* log_level = NULL;
  char* log_file = NULL;

  while (fgets(line, sizeof(line), fp)) {
    char* name = strtok(line, "=");
    if (!name) {
      goto cleanup;
    }

    char* value = strtok(NULL, "=");
    if (!value) {
      goto cleanup;
    }

    value[strcspn(value, "\r\n")] = NULL_TERMINATOR;  // remove trailing newline

    if (s_equals(name, SERVER_PORT_KEY)) {
      port = atoi(value);

      if (!is_port_in_range(port)) {
        printlogf(YS_LOG_INFO, "[config::%s] Invalid port number\n", __func__);
        goto cleanup;
      }
    } else if (s_equals(name, NUM_THREADS_KEY)) {
      threads = atoi(value);

      if (threads < 0) {
        printlogf(YS_LOG_INFO, "[config::%s] Invalid number of threads\n",
                  __func__);
        goto cleanup;
      }
    } else if (s_equals(name, LOG_LEVEL_KEY)) {
      if (s_nullish(value)) {
        printlogf(YS_LOG_INFO, "[config::%s] Invalid log level\n", __func__);
        goto cleanup;
      }

      log_level = s_copy(value);
    } else if (s_equals(name, LOG_FILE_KEY)) {
      if (s_nullish(value)) {
        printlogf(YS_LOG_INFO, "[config::%s] Invalid log file\n", __func__);
        goto cleanup;
      }

      log_file = s_copy(value);
    } else {
      printlogf(YS_LOG_INFO,
                "[config::%s] Unknown option '%s' in config file\n", __func__,
                name);
      goto cleanup;
    }
  }

  if (port) {
    server_conf.port = port;
  }

  if (threads) {
    server_conf.threads = threads;
  }

  if (!s_nullish(log_level)) {
    server_conf.log_level = log_level;
  }

  if (!s_nullish(log_file)) {
    server_conf.log_file = log_file;
  }

  ret = true;
  goto cleanup;

cleanup:
  fclose(fp);
  return ret;
}
