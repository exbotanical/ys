#include "path.h"

#include "buffer.h"

#include <string.h>
#include <stdlib.h>

char* expand_path(const char* path) {
	buffer_t* buf = buffer_init();


	// for (int i = 0; i < )

}

buffer_t* split(const char* str, const char* delimiter) {
	buffer_t* buf = buffer_init();
	char* token;
	char* input = strdup(str);

	if (input != NULL) {
		while ((token = strsep(&input, delimiter)) != NULL) {
			buffer_extend(buf, token);
		}

		free(input);
	}

	return buf;
}


char* derive_label_pattern(const char* label);
char* derive_parameter_key(const char* label);

char* regex_cache_get(char* pattern);
