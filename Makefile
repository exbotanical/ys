CC=gcc
INSTALL_DIR=/usr/local/bin
CFLAGS=-Ideps -Wall -Wextra -pedantic -lm -std=c17

SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*/*.c)
OBJFILES = $(SRC:.c=.o)

TARGET=rest-c

.PHONY:
all: $(TARGET)

.PHONY:
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES)

.PHONY:
%.o: %.c
	$(CC) $(DEP_FLAG) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

.PHONY:
clean:
	rm -f $(OBJFILES) $(TARGET)

.PHONY:
install: $(TARGET)
	cp -f $(TARGET) $(INSTALL_DIR)
	# install -m 0777 $(TARGET) $(DEST)/$(TARGET)

.PHONY:
uninstall: $(INSTALL_DIR)/$(TARGET)
	rm -f $(INSTALL_DIR)/$(TARGET)

debug: CFLAGS += -D debug
debug: $(TARGET)
