CC ?= gcc
AR ?= ar
LINTER := clang-format

DYNAMIC_TARGET := libys.so
STATIC_TARGET := libys.a
INTEG_TARGET := test_server_bin

PREFIX := /usr/local
INCDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib

SRC := $(wildcard src/*.c)
DEPS := $(wildcard deps/*/*.c)
OBJ := $(addprefix obj/, $(notdir $(SRC:.c=.o)) $(notdir $(DEPS:.c=.o)))

CFLAGS = -Iinclude -Ideps -g -ggdb -fPIC -Wall -Wextra -pedantic -Wno-missing-braces
LIBS := -lcrypto -lssl -lm -lpcre -pthread

all: $(DYNAMIC_TARGET) $(STATIC_TARGET)

$(DYNAMIC_TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -shared $(LIBS) -o $(DYNAMIC_TARGET)

$(STATIC_TARGET): $(OBJ)
	$(AR) rcs $@ $(OBJ)

obj/%.o: src/%.c include/libys.h | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: deps/*/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj:
	mkdir -p obj

install: $(STATIC_TARGET)
	mkdir -p ${LIBDIR} && cp -f ${STATIC_TARGET} ${LIBDIR}/$(STATIC_TARGET)
	mkdir -p ${INCDIR} && cp -r include/libys.h ${INCDIR}

uninstall:
	rm -f ${LIBDIR}/$(STATIC_TARGET)
	rm -f ${INCDIR}/libys.h

test:
	$(MAKE) unit_test
	$(MAKE) integ_test

unit_test: OBJ += lib/test_util.c
unit_test: $(DYNAMIC_TARGET)
	./scripts/test.bash
	$(MAKE) clean

integ_test: $(STATIC_TARGET)
	$(CC) t/integ/test_server.c $(wildcard t/integ/deps/*/*.c) $(LIBS) $(STATIC_TARGET) -o $(INTEG_TARGET)
	./scripts/integ.bash
	$(MAKE) clean

clean:
	rm -f $(OBJ) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(INTEG_TARGET) main

lint:
	$(LINTER) -i $(SRC) $(TESTS)

.PHONY: all obj install uninstall test unit_test integ_test clean lint
