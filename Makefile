
CC=gcc
CFLAGS=-Wall
LIBS=-lexif -ljpeg
OBJECTS=jpeg_handler.o exif.o
TARGET=infoto

all: $(OBJECTS)
	$(CC) main.c $(CFLAGS) $(OBJECTS) $(LIBS) -o $(TARGET)

jpeg_handler.o:
	gcc -c $(CFLAGS) jpeg_handler.c

exif.o:
	gcc -c $(CFLAGS) exif.c

clean:
	rm $(TARGET) $(OBJECTS)
