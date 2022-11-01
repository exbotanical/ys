#include "tap.c/tap.h"

#include <string.h>
#include <stdio.h>

void test_X () {
	char* token;
	char* string = strdup("abc:def:ghi");

	if (string != NULL) {
		char* tofree = string;

		while ((token = strsep(&string, ":")) != NULL) {
			printf("%s\n", token);
		}

		free(tofree);
	}
}

int main (int argc, char* argv[]) {
	plan(0);
	test_X();
  done_testing();
}
