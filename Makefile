OS := $(shell lsb_release -si)

DEPS_DIR := deps
SRC_DIR := src
LIB_DIR := lib
TEST_DIR := t
UNIT_DIR := $(TEST_DIR)/unit
INTEG_DIR := $(TEST_DIR)/integ
LIBSSL_DIR := /usr/include/openssl/

CC := gcc
CFLAGS := -g -fPIC -Wall -Wextra -pedantic -Wno-missing-braces -I$(DEPS_DIR) -I$(LIBSSL_DIR)
LDFLAGS := -shared -lcrypto -lssl -lm -lpcre -pthread
TFLAGS :=

LINTER := clang-format
INTEG_RUNNER := shpec

SO_TARGET := libys.so
STATIC_TARGET := libys.a

INTEG_BASE_BIN := test_server_bin
INTEG_AUTH_BIN := auth_bin

SRC := $(wildcard $(SRC_DIR)/*.c)
DEPS := $(wildcard $(DEPS_DIR)/*/*.c)
LIB := $(wildcard $(LIB_DIR)/*.c)
TESTS := $(wildcard $(TEST_DIR)/*/*.c)
INTEG_DEPS := $(wildcard $(INTEG_DIR)/deps/*/*.c)

ifeq ($(OS), Ubuntu)
	TFLAGS += -lm -lpcre
endif

ifdef DEBUG
	SRC += $(LIB_DIR)/debug.h
	CFLAGS += -I$(LIB_DIR)
endif

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: $(DEP_DIR)/%/%.c
	$(CC) $(CFLAGS) -c $< -o $@

all:
	$(info OS=$(OS) DEBUG=$(DEBUG))
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) -o $(SO_TARGET)

$(STATIC_TARGET): $(patsubst %.c,%.o,$(SRC)) $(patsubst %.c,%.o,$(DEPS))
	$(info OS=$(OS) DEBUG=$(DEBUG))
	ar rcs $@ $^

clean:
	rm -f $(filter-out %.h, $(SRC:.c=.o)) $(filter-out %.h, $(DEPS:.c=.o)) $(SO_TARGET) $(INTEG_AUTH_BIN) $(INTEG_BASE_BIN) main*

# make -s test 2>/dev/null
test:
	$(MAKE) unit_test
	$(MAKE) integ_test

# make -s unit_test 2>/dev/null
unit_test:
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(LIB) $(LDFLAGS) -o $(SO_TARGET)
	./scripts/test.bash
	$(MAKE) clean

# make -s integ_test 2>/dev/null
integ_test: all
	$(CC) $(INTEG_DIR)/test_server.c $(INTEG_DEPS) -I$(SRC_DIR) -I$(DEPS_DIR) -L. -lys $(TFLAGS) -o $(INTEG_BASE_BIN)
	./scripts/integ.bash
	$(MAKE) clean

lint:
	$(LINTER) -i $(SRC) $(TESTS)

.PHONY: all clean test unit_test integ_test lint
