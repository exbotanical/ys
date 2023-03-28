CC := gcc
CFLAGS := -g -fPIC -Wall -Wextra -pedantic -Wno-missing-braces -Ideps -lm -lpcre -pthread
LDFLAGS := -shared -o

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

ifdef DEBUG
	SRC += lib/debug.h
	CFLAGS += -Ilib
endif

all:
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) $(BIN)

clean:
	rm -f $(filter-out %.h, $(SRC:.c=.o)) $(BIN) main*

test:
	$(MAKE) unit_test
	$(MAKE) integ_test

unit_test:
	./scripts/test.bash
	$(MAKE) clean

# make -s integ_test 2>/dev/null
integ_test:
	./scripts/test.bash integ
	$(CC) t/integ/server.c -Isrc -Ideps -o $(INTEG_BIN) -L. -lhttp
	shpec
	rm integ

lint:
	$(LINTER) -i $(SRC) $(TESTS)

.PHONY: all clean test unit_test integ_test lint
