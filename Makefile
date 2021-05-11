
CC=gcc
CFLAGS=-g -Wall -Werror
LIBS=-lexif -ljpeg
OBJECTS=main.o config.o deps/frozen/frozen.o jpeg_handler.o exif.o
TARGET=infoto

all: $(OBJECTS)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $(TARGET)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) 

clean:
	rm $(TARGET) $(OBJECTS)
