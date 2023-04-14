OS := $(shell lsb_release -si)

DEPS_DIR := deps
SRC_DIR := src
LIB_DIR := lib
TEST_DIR := t
UNIT_DIR := $(TEST_DIR)/unit
INTEG_DIR := $(TEST_DIR)/integ

CC := gcc
CFLAGS := -g -fPIC -Wall -Wextra -pedantic -Wno-missing-braces -I$(DEPS_DIR) -lm -lpcre -pthread
LDFLAGS := -shared
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

ifeq ($(OS), Ubuntu)
	TFLAGS += -lm -lpcre
endif

ifdef USE_TLS
	CFLAGS += -lcrypto -lssl -DUSE_TLS=1
ifeq ($(OS), Ubuntu)
	CFLAGS += -I/usr/include/openssl/
	TFLAGS += $(CFLAGS)
else
	CFLAGS += -I/usr/lib/openssl-1.1/
endif
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
	$(info USE_TLS=$(USE_TLS) DEBUG=$(DEBUG))
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(LDFLAGS) -o $(SO_TARGET)

$(STATIC_TARGET): $(patsubst %.c,%.o,$(SRC)) $(patsubst %.c,%.o,$(DEPS))
	$(info USE_TLS=$(USE_TLS) DEBUG=$(DEBUG))
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
	$(CC) $(INTEG_DIR)/test_server.c $(wildcard $(INTEG_DIR)/deps/*/*.c) -I$(SRC_DIR) -I$(DEPS_DIR) -L. -lys $(TFLAGS) -o $(INTEG_BASE_BIN)
	./scripts/integ.bash
	$(MAKE) clean

lint:
	$(LINTER) -i $(SRC) $(TESTS)

.PHONY: all clean test unit_test integ_test lint
