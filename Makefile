CC ?= gcc
AR ?= ar
LINTER ?= clang-format

PROG := ys

DYNAMIC_TARGET := lib$(PROG).so
STATIC_TARGET := lib$(PROG).a
UNIT_TARGET := unit_test
INTEG_TARGET := integ_test

PREFIX := /usr/local
INCDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib
SRCDIR := src
DEPSDIR := deps
TESTDIR := t
LINCDIR := include

SRC := $(wildcard $(SRCDIR)/*.c)
DEPS := $(wildcard $(DEPSDIR)/*/*.c)
OBJ := $(addprefix obj/, $(notdir $(SRC:.c=.o)) $(notdir $(DEPS:.c=.o)))

CFLAGS = -I$(LINCDIR) -I$(DEPSDIR) -g -ggdb -fPIC -Wall -Wextra -pedantic -Wno-missing-braces
LIBS := -lcrypto -lssl -lm -lpcre -lpthread

all: $(DYNAMIC_TARGET) $(STATIC_TARGET)

$(DYNAMIC_TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -shared $(LIBS) -o $(DYNAMIC_TARGET)

$(STATIC_TARGET): $(OBJ)
	$(AR) rcs $@ $(OBJ)

obj/%.o: $(SRCDIR)/%.c $(LINCDIR)/lib$(PROG).h | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: $(DEPSDIR)/*/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj:
	mkdir -p obj

install: $(STATIC_TARGET)
	mkdir -p ${LIBDIR} && cp -f ${STATIC_TARGET} ${LIBDIR}/$(STATIC_TARGET)
	mkdir -p ${INCDIR} && cp -r $(LINCDIR)/lib$(PROG).h ${INCDIR}

uninstall:
	rm -f ${LIBDIR}/$(STATIC_TARGET)
	rm -f ${INCDIR}/lib$(PROG).h

# Use make -s test 2>/dev/null to see only test results
test:
	$(MAKE) unit_test
	$(MAKE) integ_test

unit_test: $(STATIC_TARGET)
	$(CC) $(wildcard $(TESTDIR)/unit/*.c) $(STATIC_TARGET) -I$(LINCDIR) -I$(SRCDIR) -I$(DEPSDIR) $(LIBS) -o $(UNIT_TARGET)
	./$(UNIT_TARGET)
	$(MAKE) clean

integ_test: $(STATIC_TARGET)
	$(CC) $(TESTDIR)/integ/main.c $(wildcard $(TESTDIR)/integ/$(DEPSDIR)/*/*.c) $(STATIC_TARGET) -I$(LINCDIR) $(LIBS) -o $(INTEG_TARGET)
	./scripts/integ.bash
	$(MAKE) clean

clean:
	rm -f $(OBJ) $(STATIC_TARGET) $(DYNAMIC_TARGET) $(UNIT_TARGET) $(INTEG_TARGET) main

lint:
	$(LINTER) -i $(SRC) $(wildcard $(TESTDIR)/*/*.c) $(LINCDIR)/lib$(PROG).h

.PHONY: all obj install uninstall test unit_test integ_test clean lint
