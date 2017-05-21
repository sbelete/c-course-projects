cc = gcc
ccflags = -g -std=gnu99
LDflags = -pthread -Wall -Wextra

all:	server client

server: server.o comm.o db.o 
	$(cc) ${ccflags} ${LDflags} server.o comm.o db.o -o server

server.o:	server.c comm.h db.h
	$(cc) server.c -c ${ccflags}

comm.o: comm.c comm.h
	$(cc) comm.c -c ${ccflags}

db.o:	db.c db.h
	$(cc) db.c -c ${ccflags}

client: client.c
	$(cc) -o client client.c ${ccflags}

clean:
	/bin/rm -f *.o server client