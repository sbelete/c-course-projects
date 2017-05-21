# Flags to pass to compiler 
CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb

cc = gcc
EXECS = 33sh 33noprompt

.PHONY: all clean

all: $(EXECS)

33sh: sh.c
	${cc} ${CFLAGS} -o shell_prompt -DPROMPT $^ 

33noprompt: sh.c
	${cc} ${CFLAGS} -o shell_no_prompt $^

clean: 
	rm -f $(EXECS) 

#CC = gcc

#OBJS = sh.c
#WPROMPT = shell_prompt
#NOPROMPT = shell_no_prompt
#CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
#CFLAGS += -Winline -Wfloat-equal -Wnested-externs
#CFLAGS += -pedantic -std=gnu99 -Werror
#PROMPT = -DPROMPT

#all: 33sh 33noprompt

#33sh: $(OBJS)
#	$(CC) $(CFLAGS) -o $(WPROMPT) $(PROMPT) $(OBJS)

#33noprompt: $(OBJS)
#	$(CC) $(CFLAGS) -o $(NOPROMPT) $(OBJS)

#clean:
#	rm -f $(WPROMPT) $(NOPROMPT)
