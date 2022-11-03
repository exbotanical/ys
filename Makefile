CC=gcc
CFLAGS=-g -fPIC -Ideps -lm -lpcre -Wall -Wextra -pedantic
SILENT_FLAGS=-Wno-unused-variable -Wno-discarded-qualifiers
LDFLAGS=-shared -o

BIN=librest.so
TARGET=run

SRC=$(wildcard src/*.c)
DEPS=$(wildcard deps/*/*.c)

TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all:
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) $(BIN)

clean:
	rm -f $(SRC:.c=.o) $(TARGET) $(BIN) $(BIN).so main*

run:
	$(CC) $(CFLAGS) $(SILENT_FLAGS) -o $(TARGET) $(DEPS) $(SRC)
	./run

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
