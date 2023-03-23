#include "config.h"

#include <stdio.h>   // for FILE
#include <stdlib.h>  // for free
#include <string.h>  // for strcspn
#include <unistd.h>  // for access + F_OK

#include "logger.h"
#include "util.h"  // for s_equals, s_copy

// Default server config
server_config_t server_config = {.log_file = NULL,
                                 .log_level = DEFAULT_LOG_LEVEL,
                                 .num_threads = DEFAULT_NUM_THREADS,
                                 .port_num = DEFAULT_PORT_NUM};

bool parse_config(const char* filename) {
  bool ret = false;
  char line[CONFIG_MAX_LINE_LEN];

  if (access(filename, F_OK) == -1) {
    printlogf(LOG_DEBUG, "Config file %s does not exist. Using defaults\n",
              filename);
    return true;
  }

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printlogf(LOG_INFO, "Error opening config file '%s'\n", filename);

    return ret;
  }

  char* log_level = NULL;
  char* log_file = NULL;
  int port_num;
  int num_threads;

  while (fgets(line, sizeof(line), fp)) {
    char* name = strtok(line, "=");
    if (!name) {
      goto cleanup;
    }

    char* value = strtok(NULL, "=");
    if (!value) {
      goto cleanup;
    }

    value[strcspn(value, "\r\n")] = '\0';  // remove trailing newline

    if (s_equals(name, SERVER_PORT_KEY)) {
      port_num = atoi(value);
      if (port_num < 1024 || port_num > 65535) {
        printlogf(LOG_INFO, "Invalid port number\n");
        goto cleanup;
      }
    } else if (s_equals(name, NUM_THREADS_KEY)) {
      num_threads = atoi(value);
      if (num_threads < 0) {
        printlogf(LOG_INFO, "Invalid number of threads\n");
        goto cleanup;
      }
    } else if (s_equals(name, LOG_LEVEL_KEY)) {
      if (s_nullish(value)) {
        printlogf(LOG_INFO, "Invalid log level\n");
        goto cleanup;
      }
      log_level = s_copy(value);
    } else if (s_equals(name, LOG_FILE_KEY)) {
      if (s_nullish(value)) {
        printlogf(LOG_INFO, "Invalid log file\n");
        goto cleanup;
      }
      log_file = s_copy(value);
    } else {
      printlogf(LOG_INFO, "Unknown option '%s' in config file\n", name);
      goto cleanup;
    }
  }

  if (port_num) {
    server_config.port_num = port_num;
  }
  if (num_threads) {
    server_config.num_threads = num_threads;
  }

  if (!s_nullish(log_level)) {
    server_config.log_level = log_level;
  }

  if (!s_nullish(log_file)) {
    server_config.log_file = log_file;
  }

  ret = true;
  goto cleanup;

cleanup:
  fclose(fp);
  return ret;
}
