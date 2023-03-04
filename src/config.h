#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Maximum number of queued connections allowed for server.
 */
static const int MAX_CONN = 100;

/**
 * @brief Default number of threads to use in the server thread pool.
 */
static const int DEFAULT_NUM_THREADS = 4;

/**
 * @brief Environment variable key for user-defined number of threads.
 */
static const char NUM_THREADS_KEY[8] = "THREADS";

/**
 * @brief Environment variable key for user-defined port number.
 * TODO: use this
 */
static const char SERVER_PORT_KEY[5] = "PORT";

#endif /* CONFIG_H */
