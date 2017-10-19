CC = gcc
CFLAGS = -std=gnu99 -Wall -Werror -Wunused -O1
EXECS = poly

OBJS = main.o poly.o poly_naive.o

all: $(EXECS)

$(EXECS): $(OBJS)
	$(CC) $(CFLAGS) $^ poly_fast.o -o $@

main.o: main.c timer.h sec.h
	$(CC) $(CFLAGS) -c main.c -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	/bin/rm -f $(EXECS) *~ $(OBJS)

