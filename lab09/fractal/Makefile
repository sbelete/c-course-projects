INCLUDE = -I/course/cs033/pub/vm/libgd/include
LIBDIR  = -L/course/cs033/pub/vm/libgd/lib

# GNU99 is necessary for the MAP_ANONYMOUS flag among others
# TODO: Add back in -m32 flag if necessary
COMPILERFLAGS = -Wall -g -std=gnu99
CC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
LIBRARIES = -lgd

COLORS = colors.h colors.c
IMAGE = image.h image.c
MAIN = fractal.h main.c

.PHONY: all clean

all: fractal fractal_forked

fractal: fractal.c
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(LIBRARIES) $(COLORS) $(IMAGE) $(MAIN) $^
	
fractal_forked: fractal_forked.c
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(LIBRARIES) $(COLORS) $(IMAGE) $(MAIN) $^

clean:
	rm -rf *.o fractal fractal_forked *~ *.png
