CC=gcc 
OBJFILES=$(wildcard ./*.c)
INTERMEDIARY=main
TARGET=demo

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -I../../src -I../../deps $(OBJFILES) -o $(INTERMEDIARY) -L../../ -lthread

clean:
	rm $(TARGET)
