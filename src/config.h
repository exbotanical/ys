#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
  short num_threads;
  int port_num;
  char* log_level;
  char* log_file;
} server_config_t;

extern server_config_t server_config;

// Max line length for config file.
static const int CONFIG_MAX_LINE_LEN = 256;

// Maximum number of queued connections allowed for server.
static const short MAX_CONN = 100;

// Default number of threads to use in the server thread pool.
static const short DEFAULT_NUM_THREADS = 4;

static const int DEFAULT_PORT_NUM = 5000;
static const char DEFAULT_LOG_LEVEL[] = "info";

// Environment variable key for user-defined number of threads.
static const char NUM_THREADS_KEY[12] = "NUM_THREADS";

// Environment variable key for user-defined port number.
static const char SERVER_PORT_KEY[5] = "PORT";

static const char LOG_LEVEL_KEY[10] = "LOG_LEVEL";

static const char LOG_FILE_KEY[9] = "LOG_FILE";

/**
 * parse_config parses the given config file at `filename` and set
 * `server_config`
 *
 * @param filename
 * @return true
 * @return false
 */
bool parse_config(const char* filename);

#endif /* CONFIG_H */
