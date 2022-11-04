#include "tap.c/tap.h"

#include "buffer.h"

#include <string.h>
#include <stdio.h>

void test_buffer_init() {
	buffer_t *buf = buffer_init();

	is(buf->state, NULL, "newly initialized buffer's state is NULL");
	ok(buf->len == 0, "newly initialized buffer's length is 0");
}

void test_buffer_append() {
	buffer_t *buf = buffer_init();

	buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");
	buffer_append(buf, "12345678910");
	buffer_append(buf, "abcdefghijklmnopqrstuvwxyz");

	char *expected = "abcdefghijklmnopqrstuvwxyz12345678910abcdefghijklmnopqrstuvwxyz";

	is(buf->state, expected, "buffer holds all appended characters in order");
	ok(buf->len == strlen(expected), "newly initialized buffer's length is 0");

	buffer_free(buf);
}

void test_buffer_free() {
	buffer_t *buf = buffer_init();

	lives_ok({
		buffer_free(buf);
	}, "frees the buffer's heap memory");
}

int main (int argc, char *argv[]) {
	plan(5);

	test_buffer_init();
	test_buffer_append();
	test_buffer_free();

  done_testing();
}
