#
#  AUTHOR:  John Graham
#  
#  FILE: 	Makefile
#


CC = gcc
CFLAGS = -c -g


all: jsh clean

jsh: jsh.o parse.o directory.o
	$(CC) jsh.o parse.o directory.o -o jsh

jsh.o: src/jsh.c
	$(CC) $(CFLAGS) src/jsh.c

parse.o: src/parse.c
	$(CC) $(CFLAGS) src/parse.c

directory.o: src/directory.c
	$(CC) $(CFLAGS) src/directory.c

clean:
	rm *.o