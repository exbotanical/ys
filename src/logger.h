#ifndef LOGGER_H
#define LOGGER_H

#ifndef fprintf
#include <stdio.h>
#endif /* fprintf */

#ifdef DEBUG

#define LOG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

#else

#define LOG(fmt, ...)


#endif /* DEBUG */

#define DIE(rc, fmt, ...) fprintf(stderr, fmt, __VA_ARGS__), exit(rc)

#endif /* LOGGER_H */
