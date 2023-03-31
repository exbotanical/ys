CC := gcc
SRC := $(wildcard ./*.c) $(wildcard ./deps/*/*.c)
TARGET := rest

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) -I../../src -I../../deps -Ideps -L../../ -lhttp -luuid

clean:
	rm $(TARGET)
