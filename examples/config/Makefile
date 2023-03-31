CC := gcc
SRC := $(wildcard ./*.c)
TARGET := config

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -I../../src -I../../deps $(SRC) -o $(TARGET) -L../../ -lhttp

clean:
	rm $(TARGET)
