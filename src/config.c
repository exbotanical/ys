#include "config.h"

#include <stdio.h>   // for FILE, stderr
#include <stdlib.h>  // for free
#include <string.h>  // for strcspn, strdup

#include "util.h"  // for str_equals

// Default server config
server_config_t server_config = {.log_file = NULL,
                                 .log_level = DEFAULT_LOG_LEVEL,
                                 .num_threads = DEFAULT_NUM_THREADS,
                                 .port_num = DEFAULT_PORT_NUM};

void parse_config(const char* filename) {
  char line[CONFIG_MAX_LINE_LEN];

  // TODO: existence and permissions check
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening config file '%s'\n", filename);
    return;
  }

  while (fgets(line, sizeof(line), fp)) {
    char* name = strtok(line, "=");
    char* value = strtok(NULL, "=");

    if (name && value) {
      value[strcspn(value, "\r\n")] = '\0';  // remove trailing newline

      if (str_equals(name, SERVER_PORT_KEY)) {
        server_config.port_num = atoi(value);
      } else if (str_equals(name, NUM_THREADS_KEY)) {
        server_config.num_threads = atoi(value);
      } else if (str_equals(name, LOG_LEVEL_KEY)) {
        server_config.log_level = strdup(value);
      } else if (str_equals(name, LOG_FILE_KEY)) {
        server_config.log_file = strdup(value);
      } else {
        fprintf(stderr, "Unknown option '%s' in config file\n", name);
      }
    }
  }

  fclose(fp);
}
