CC=gcc
CFLAGS=-Wall -Werror -fPIC
PFLAGS=-DINFOTO_VERSION='"$(shell git rev-parse HEAD)"'
INCLUDES=-I/usr/include/freetype2 -I/usr/include/libpng16
LIBS=-lexif -ljpeg -lfreetype
DEPS=deps/frozen/frozen.o
OBJ=obj
BIN=bin
# grab all top level c files
SOURCES=$(wildcard *.c)
# path sub to point to obj folder
OBJECTS=$(patsubst %.c, $(OBJ)/%.o, $(SOURCES))
# files to filter out for shared/archive files
FILTER_FILES=$(OBJ)/main.o $(OBJ)/json_parsing.o
ARCHIVE_FILES=$(filter-out $(FILTER_FILES), $(OBJECTS))
ARCHIVE_DIR=lib
ARCHIVE_NAME=libinfoto.a
SHARED_NAME=libinfoto.so
# executable, if used for cli
TARGET=infoto

.PHONY: all
# have to path sub the dependencies. maybe a better way?
all: $(OBJECTS) $(DEPS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -O2 $(PFLAGS) $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

.PHONY: debug
debug: $(OBJECTS) $(DEPS)
	$(CC) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) -g $(PFLAGS) $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

# rule for dependencies
# put .o files in obj directory
deps/%.o: deps/%.c
	gcc -c -o $(patsubst %.c, $(OBJ)/%.o, $(notdir $<)) $< $(PFLAGS) $(CFLAGS)

# rule for top level source files
$(OBJ)/%.o: %.c
	@mkdir -p $(OBJ)
	@mkdir -p $(BIN)
	gcc -c -o $@ $< $(PFLAGS) $(CFLAGS) $(INCLUDES)

.PHONY: archive
# rule to create shared object and archive files
archive: $(ARCHIVE_FILES) $(DEPS)
	mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $(ARCHIVE_DIR)/$(SHARED_NAME) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^)) $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_NAME) $(patsubst %.o, $(OBJ)/%.o, $(notdir $^))

.PHONY: clean
clean:
	rm $(BIN)/$(TARGET) $(OBJ)/*.o
