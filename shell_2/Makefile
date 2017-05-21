# Flags to pass to compiler
CFLAGS = -Wall -O2 -std=c99 -D_GNU_SOURCE -ggdb

cc = gcc
EXECS = shell_prompt shell_no_prompt

.PHONY: all clean

all: $(EXECS)

shell_prompt: sh.c jobs.c executes.c reap.c parse.c help.c
	${cc} ${CFLAGS} -o shell_prompt -DPROMPT $^

shell_no_prompt: sh.c jobs.c executes.c reap.c parse.c help.c
	${cc} ${CFLAGS} -o shell_no_prompt $^

clean:
	rm -f $(EXECS)
	rm -r -f shell_prompt.dSYM shell_no_prompt.dSYM
