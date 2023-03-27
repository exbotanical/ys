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

// Max line length for config file
static const int CONFIG_MAX_LINE_LEN = 256;

// Maximum number of queued connections allowed for server
static const short MAX_QUEUED_CONNECTIONS = 100;

// Default number of threads to use in the server thread pool
static const short DEFAULT_NUM_THREADS = 4;

// Default port number
static const int DEFAULT_PORT_NUM = 5000;

// Default log level
static const char DEFAULT_LOG_LEVEL[] = "info";

// Environment variable key for user-defined number of threads
static const char NUM_THREADS_KEY[12] = "NUM_THREADS";

// Environment variable key for user-defined port number
static const char SERVER_PORT_KEY[5] = "PORT";

// Environment variable key for user-defined log level
static const char LOG_LEVEL_KEY[10] = "LOG_LEVEL";

// Environment variable key for user-defined log file path
static const char LOG_FILE_KEY[9] = "LOG_FILE";

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
