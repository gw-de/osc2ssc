CC		= gcc
CFLAGS	= -g -c -Wall
LFLAGS	= -lpthread
TARGET	= protocol-converter

# Directory of all source (*.c) files
SRC_DIR = src

# Directory where object files (artifacts) are stored
OBJ_DIR = obj

# Get all .c files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Substitute all ".c" file name strings to ".o" file name strings
OBJS = $(patsubst src/%.c, out/%.o, $(SRCS))
#OBJS = $(SRCS:%.c=%.o)

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

out/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(SRC_DIR)/*.o $(OBJ_DIR)/*.o *.o

.PHONY: all clean