#include "path.h"
#include "buffer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

ch_array_t* expand_path(const char* path) {
	return split(path, PATH_DELIMITER);
}

ch_array_t* split(const char* str, const char* delimiter) {
	// @see https://wiki.sei.cmu.edu/confluence/display/c/STR06-C.+Do+not+assume+that+strtok%28%29+leaves+the+parse+string+unchanged
	char* input = strdup(str);
	ch_array_t* ca = ch_array_init(0);

	if (!strstr(input, delimiter)) {
		return ca;
	}

  char* token = strtok(input, delimiter);
	if (token == NULL) {
		return ca;
	}

	while (token != NULL) {
		ch_array_insert(ca, token);
		token = strtok(NULL, delimiter);
	}

	free(input);

	return ca;
}

int index_of (const char* str, const char* target) {
	char* needle = strstr(str, target);
	if (needle == NULL) {
		return -1;
	}

	return needle - str;
}

	// TODO: validate and test
char* substr(const char* str, int start, int end, bool inclusive) {
	// int len = strlen(str);
	end = inclusive ? end : end - 1;

	if (start > end) {
		return "";
	}

	// if (start < 0 || start > len) {
	// 	printf("Invalid \'start\' index\n");
	// 	return 1;
	// }

	// if(end > len) {
	// 	printf("Invalid \'end\' index\n");
	// 	return 1;
	// }

	char* ret = malloc(sizeof(char) * (end - start));

	int i = 0;
	int j = 0;
	for(i = start, j = 0; i <= end; i++, j++) {
		ret[j]=str[i];
	}

	ret[j]='\0';

	return ret;
}

char* derive_label_pattern(const char* label) {
	int start = index_of(label, PARAMETER_DELIMITER_START);
	int end = index_of(label, PARAMETER_DELIMITER_END);

	// If the label doesn't contain a pattern, default to the wildcard pattern.
	if (start == -1 || end == -1) {
		// TODO: validate
		return strdup(PATTERN_WILDCARD);
	}

	printf("HERE: %d %d\n", start+1, end);
	return substr(label, start + 1, end, false);
}

char* derive_parameter_key(const char* label);

char* regex_cache_get(char* pattern);
