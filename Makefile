CC=gcc
CFLAGS=-g -fPIC -Ideps -lm -Wall -Wextra -pedantic
LDFLAGS=-shared -o

BIN=librest.so
TARGET=run

OBJFILES=$(wildcard src/*.c)
DEPS=$(wildcard deps/*/*.c)

TESTS = $(patsubst %.c, %, $(wildcard t/*.c))

all:
	$(CC) $(CFLAGS) $(DEPS) $(OBJFILES) $(LDFLAGS) $(BIN)

clean:
	rm -f $(TARGET) $(BIN) $(BIN).so main*

run:
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

test:
	./scripts/test.bash
	$(MAKE) clean

.PHONY: test clean
