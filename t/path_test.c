#include "tap.c/tap.h"

#include "path.h"

#include <string.h>
#include <stdio.h>

void test_split() {
	char* test_str = "a:b:c:d";
	buffer_t* buf = split(test_str, ":");
	printf("HERE: %s\n",buf->state);

	is(buf->state[0], "a", "first substring is captured");
	is(buf->state[1], "b", "second substring is captured");
	is(buf->state[2], "c", "third substring is captured");
	is(buf->state[3], "d", "fourth substring is captured");
}

int main (int argc, char* argv[]) {
	plan(1);
	test_split();
  done_testing();
}
