CC = gcc
TARGET = landscape

all: landscape

$(TARGET): landscape.o
	$(CC) landscape.o -o $(TARGET) -lglut

landscape.o: landscape.c fRGBcolors.h
	$(CC) -c landscape.c

clean:
	rm -rf *o $(TARGET)
