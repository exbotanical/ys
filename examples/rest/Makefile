CC := gcc
LINK := -lys -lm -lpcre
SRC := $(wildcard ./*.c) $(wildcard ./deps/*/*.c)
TARGET := demo

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -I../../src -I../../deps -I../../include $(SRC) -o $(TARGET) -L../../ $(LINK)

clean:
	rm $(TARGET)
