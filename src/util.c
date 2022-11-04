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
	va_list args, args_cp;
 	va_start(args, fmt);
	va_copy(args_cp, args);

	// Pass length of zero first to determine number of bytes needed
	int n = vsnprintf(NULL, 0, fmt, args) + 1;
	char *buf = malloc(n);
	if (!buf) {
		LOG("%s\n", "[util::fmt_str] failed to allocate char*");
		return NULL;
	}

	vsnprintf(buf, n, fmt, args_cp);

 	va_end(args);
 	va_end(args_cp);

	return buf;
}
