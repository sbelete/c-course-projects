SRC_DIR = src
WORDS_SRC = words.h words.c file_io.h file_io.c ttable.h ttable.c
CC = gcc
CFLAGS = -I. -O1 -lprofiler -std=c99 -Wall -Werror -Wextra

.PHONY: all clean

all: words words_avg

words: $(WORDS_SRC) main.c
	${cc} -o words $(WORDS_SRC) main.c ${CFLAGS}

words_avg: $(WORDS_SRC) main_avg.c
	${cc} -o words_avg $(WORDS_SRC) main_avg.c ${CFLAGS}

clean:
	/bin/rm -f words words_avg
