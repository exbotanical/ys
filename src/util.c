#include "util.h"

#include "logger.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

char *safe_itoa(int x) {
	int length = snprintf(NULL, 0, "%d", x);
	char *str = malloc(length + 1 );
	snprintf(str, length + 1, "%d", x);

	return str;
}

char *fmt_str (char *fmt, ...) {
	va_list args;
 	va_start(args, fmt);

	// Pass length of zero first to determine number of bytes needed
	size_t n_bytes = snprintf(NULL, 0, fmt, args) + 1;
 	va_end(args);

	char *str = malloc(n_bytes);
	if (!str) {
		LOG("%s\n", "[util::fmt_str] failed to allocate char*");
		return NULL;
	}

	vsnprintf(str, n_bytes, fmt, args);

	return str;
}
