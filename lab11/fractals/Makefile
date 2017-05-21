INCLUDE = -I/course/cs033/pub/vm/libgd/include
LIBDIR  = -L/course/cs033/pub/vm/libgd/lib

COMPILERFLAGS = -Wall -g -std=gnu99
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lgd

COLORS = colors.h colors.c
IMAGE = image.h image.c
MAIN = fractal.h main.c

all: fractal fractal_threaded

fractal: fractal.c $(COLORS) $(IMAGE) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(LIBRARIES) $^
	
fractal_threaded: fractal_threaded.c $(COLORS) $(IMAGE) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(LIBRARIES) -lpthread $^

clean:
	rm -rf *.o fractal fractal_forked fractal_threaded *~ *.png
