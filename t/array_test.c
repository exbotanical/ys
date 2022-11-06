#include "tap.c/tap.h"

#include "array.h"

#include <string.h>
#include <stdio.h>

void test_ch_array_init() {
	ch_array_t *ca;

	lives_ok({
		ca = ch_array_init();
	}, "initializes the char array");

	ok(ca->size == 0, "newly initialized array's size is 0");
	ok(strcmp(ca->state, "") == 0, "newly initialized array's state is empty");

	ch_array_free(ca);
}

void test_ch_array_insert() {
	ch_array_t *ca = ch_array_init();

	ch_array_insert(ca, "test");

	is(ca->state[0], "test", "array contains value at index position");
	is(ca->state[1], NULL, "array contains NULL at index position");

	ch_array_insert(ca, "str");
	is(ca->state[0], "test", "array contains value at index position");
	is(ca->state[1], "str", "array contains value at index position");

	ch_array_free(ca);
}

void test_ch_array_free() {
	ch_array_t *ca = ch_array_init();

	lives_ok({
		ch_array_free(ca);
	}, "frees the allocated char array");
}

int main (int argc, char *argv[]) {
	plan(8);

	test_ch_array_init();
	test_ch_array_insert();
	test_ch_array_free();

  done_testing();
}
