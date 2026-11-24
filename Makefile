# Smart Library Management System Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
TARGET = library

all: $(TARGET)

$(TARGET): library.c
	$(CC) $(CFLAGS) library.c -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe *.tmp

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
