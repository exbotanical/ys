#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct config_t {
  short num_threads;
  int port_num;
  char* log_level;
  char* log_file;
  bool use_syslog;
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
static const char DEFAULT_LOG_FILE[] = "/tmp/libhttp.log";  // TODO: generate

/**
 * @brief Environment variable key for user-defined number of threads.
 */
static const char NUM_THREADS_KEY[8] = "THREADS";

/**
 * @brief Environment variable key for user-defined port number.
 * TODO: use this
 */
static const char SERVER_PORT_KEY[5] = "PORT";

static const char LOG_LEVEL_KEY[10] = "log_level";

static const char LOG_FILE_KEY[9] = "log_file";

static const char USE_SYSLOG_KEY[11] = "use_syslog";

#endif /* CONFIG_H */
