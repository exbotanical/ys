CC := gcc
CFLAGS := -g -fPIC -Wall -Wextra -pedantic -Wno-missing-braces -Ideps -lm -lpcre -pthread
LDFLAGS := -shared -o
DEBUG := -DDEBUG=1 # TODO: use opt

LINTER := clang-format

BIN := libhttp.so
INTEG_BIN := integ

SRC := $(wildcard src/*.c)
DEPS := $(wildcard deps/*/*.c)
TESTS := $(wildcard t/*/*.c)

ifndef USE_JSON
SRC := $(filter-out src/json.%, $(SRC))
DEPS := $(filter-out deps/yyjson%, $(DEPS))
endif

all:
	$(CC) $(DEBUG) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) $(BIN)

clean:
	rm -f $(SRC:.c=.o) $(BIN) main*

test:
	$(MAKE) unit_test
	$(MAKE) integ_test

unit_test:
	./scripts/test.bash
	$(MAKE) clean

integ_test: all
	$(CC) t/integ/server.c -Isrc -Ideps -o $(INTEG_BIN) -L. -lhttp
	shpec

lint:
	$(LINTER) -i $(SRC) $(TESTS)

.PHONY: all clean test unit_test integ_test lint
