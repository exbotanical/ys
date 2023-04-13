CC := gcc
SRC := $(wildcard ./*.c) $(wildcard ./deps/*/*.c)
TARGET := demo
LINK := -lys -luuid -lpcre -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) -I../../src -I../../deps -Ideps -L../../ $(LINK)

clean:
	rm $(TARGET)
