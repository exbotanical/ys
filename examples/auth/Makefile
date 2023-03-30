CC=gcc
OBJFILES=$(wildcard ./*.c)
TARGET=demo

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -I../../src -I../../deps $(OBJFILES) -o $(TARGET) -L../../ -lhttp -luuid

clean:
	rm $(TARGET)
