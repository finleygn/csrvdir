BINARY_NAME=serve
SOURCE_DIR=src

CC=cc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Os

DEPS = src/logging.h src/request.h
OBJ = src/main.o src/logging.o

# Compile src files to individual object
$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Link
$(BINARY_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f ./$(SOURCE_DIR)/*.o
	rm -rf $(BINARY_NAME)
