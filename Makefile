
CC=gcc
CFLAGS=-g -Wall -Werror
INCLUDES=-I/usr/include/freetype2 -I/usr/include/libpng16
LIBS=-lexif -ljpeg -lfreetype
DEPS=deps/frozen/frozen.o
OBJ=obj
BIN=bin
# grab all top level c files
SOURCES=$(wildcard *.c)
# path sub to point to obj folder
OBJECTS=$(patsubst %.c, $(OBJ)/%.o, $(SOURCES))
TARGET=infoto

# have to path sub the dependencies. maybe a better way?
all: $(OBJECTS) $(DEPS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

# rule for dependencies
# put .o files in obj directory
deps/%.o: deps/%.c
	gcc -c -o $(patsubst %.c, $(OBJ)/%.o, $(notdir $<)) $< $(CFLAGS)

# rule for top level source files
$(OBJ)/%.o: %.c
	gcc -c -o $@ $< $(CFLAGS) $(INCLUDES)

clean:
	rm $(BIN)/$(TARGET) $(OBJ)/*.o
