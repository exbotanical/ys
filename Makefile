CC=gcc
CFLAGS=-g -fPIC -Ideps -lm -lpcre -Wall -Wextra -pedantic
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
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
