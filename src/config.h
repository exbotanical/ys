#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_t {
  short num_threads;
  int port_num;
  // TODO: const?
  char* log_level;
  char* log_file;
} ServerConfig;

extern ServerConfig server_config;

/**
 * @brief Maximum number of queued connections allowed for server.
 */
static const short MAX_CONN = 100;

/**
 * @brief Default number of threads to use in the server thread pool.
 */
static const short DEFAULT_NUM_THREADS = 4;

static const int DEFAULT_PORT_NUM = 5000;
static const char DEFAULT_LOG_LEVEL[] = "info";

/**
 * @brief Environment variable key for user-defined number of threads.
 */
static const char NUM_THREADS_KEY[12] = "NUM_THREADS";

/**
 * @brief Environment variable key for user-defined port number.
 * TODO: use this
 */
static const char SERVER_PORT_KEY[5] = "PORT";

static const char LOG_LEVEL_KEY[10] = "LOG_LEVEL";

static const char LOG_FILE_KEY[9] = "LOG_FILE";

void parse_config(const char* filename);

#endif /* CONFIG_H */
