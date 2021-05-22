
CC=gcc
CFLAGS=-g -Wall -Werror
LIBS=-lexif -ljpeg
DEPS=deps
OBJ=obj
BIN=bin
SOURCES=$(join $(wildcard *.c), $(shell find $(DEPS) -name '*.c'))
OBJECTS=$(patsubst %.c, $(OBJ)/%.o, $(SOURCES))
TARGET=infoto

all: $(OBJECTS)
	$(CC) $^ $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) 

clean:
	rm $(BIN)/$(TARGET) $(OBJ)/**/*.o
