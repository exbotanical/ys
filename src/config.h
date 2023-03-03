#ifndef CONFIG_H
#define CONFIG_H

/**
 * @brief Maximum number of queued connections allowed for server.
 */
static const int MAX_CONN = 100;

static const int DEFAULT_NUM_THREADS = 4;

static const char NUM_THREADS_KEY[8] = "THREADS";
static const char SERVER_PORT_KEY[5] = "PORT";

#endif /* CONFIG_H */
