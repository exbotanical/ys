#include "util.h"

#include <stdlib.h>
#include <stdio.h>

char *safe_itoa(int x) {
	int length = snprintf(NULL, 0, "%d", x);
	char *str = malloc(length + 1 );
	snprintf(str, length + 1, "%d", x);

	// TODO: return struct w/ callback to free
	// free(str);
	return str;
}

void iterate_h_table(h_table* ht, void (*cb)()) {
	for(int i = 0; i < ht->capacity; i++) {
		if (ht->records[i] != NULL) {
			(*cb)(ht->records[i]);
		}
	}
}
