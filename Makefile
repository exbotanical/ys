CC=gcc
CFLAGS=-g -fPIC -Ideps -lm -lpcre -Wall -Wextra -pedantic -pthread
SILENT_FLAGS=-Wno-unused-variable -Wno-discarded-qualifiers
LDFLAGS=-shared -o
DEBUG=-DDEBUG=1 # TODO: use opt

BIN=libhttp.so
TARGET=run

SRC=$(wildcard src/*.c)
DEPS=$(wildcard deps/*/*.c)

TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all:
	$(CC) $(DEBUG) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) $(BIN)

clean:
	rm -f $(SRC:.c=.o) $(TARGET) $(BIN) $(BIN).so main*

run:
	$(CC) $(CFLAGS) $(SILENT_FLAGS) -o $(TARGET) $(DEPS) $(SRC)
	./run

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
