# Flags to pass to compiler 
CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb

# TODO: Create rest of Makefile
cc = gcc
EXECS = quiet_repl loud_repl

.PHONY: all clean test

all: $(EXECS)

quiet_repl: repl.c upper.c upper.h
	cp /gpfs/main/course/cs033/pub/labs/makefile_lab/lower.h . 
	cp /gpfs/main/course/cs033/pub/labs/makefile_lab/lower.c .
	${cc} ${CFLAGS} -o quiet_repl $^ lower.h lower.c

loud_repl: repl.c upper.c upper.h 
	cp /gpfs/main/course/cs033/pub/labs/makefile_lab/lower.h . 
	cp /gpfs/main/course/cs033/pub/labs/makefile_lab/lower.c .
	${cc} ${CFLAGS} -o loud_repl -DEXTRA_LOUD $^ lower.h lower.c

README.pdf: README.tex
	pdflatex $^ $@
	rm -f README.aux README.log

test: repltests
	./repltests quiet_repl loud_repl
	
clean: 
	rm -f $(EXECS) lower.h lower.c README.pdf 