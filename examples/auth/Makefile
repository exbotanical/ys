CC := gcc
SRC := $(wildcard ./*.c) $(wildcard ./deps/*/*.c)
TARGET := auth

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) -I../../src -I../../deps -Ideps -L../../ -lhttp -luuid

clean:
	rm $(TARGET)
