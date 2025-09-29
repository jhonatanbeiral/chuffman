CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -Iinclude
SRC = src/main.c src/huffman.c src/bitio.c
OBJ = $(SRC:.c=.o)
TARGET = huffman

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean

