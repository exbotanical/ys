CC := gcc
SRC := $(wildcard ./*.c)
TARGET := cors

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -I../../src -I../../deps $(SRC) -o $(TARGET) -L../../ -lhttp

clean:
	rm $(TARGET)
