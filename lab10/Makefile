# Flags to pass to compiler 
CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb
CC = gcc

# TODO: Create rest of Makefile
.PHONY: all clean

all: client

client: networking.c
	$(CC) $(CFLAGS) -o client networking.c

clean:
	rm -f client