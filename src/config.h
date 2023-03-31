#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/**
 * server_config is a globally-used configurations object containing user server
 * settings
 */
typedef struct {
  /**
   * The number of threads to use for handling client connections in the
   * server's thread pool
   */
  short threads;

  /**
   * The port number on which the server will listen
   */
  int port;

  /**
   * The logging level
   */
  char* log_level;

  /**
   * The log file path. If not extant, logs will be written to stderr
   */
  char* log_file;
} server_config;

extern server_config server_conf;

// Maximum number of queued connections allowed for server
extern const short MAX_QUEUED_CONNECTIONS;

/**
 * parse_config parses the given config file at `filename` and set
 * `server_conf`
 *
 * @param filename
 * @return true if the config was valid
 * @return false if the config was invalid
 *
 * @internal
 */
bool parse_config(const char* filename);

#endif /* CONFIG_H */
