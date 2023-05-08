CC := gcc
SRC := $(wildcard ./*.c)
TARGET := demo

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -I../../src -I../../deps -I../../include $(SRC) -o $(TARGET) -L../../ -lys

clean:
	rm $(TARGET)
