#
#  AUTHOR:  John Graham
#  
#  FILE: 	Makefile
#


CC = gcc
CFLAGS = -c -g


all: jshell clean

jshell: jshell.o parse.o directory.o
	$(CC) jshell.o parse.o directory.o -o jshell

jshell.o: src/jshell.c
	$(CC) $(CFLAGS) src/jshell.c

parse.o: src/parse.c
	$(CC) $(CFLAGS) src/parse.c

directory.o: src/directory.c
	$(CC) $(CFLAGS) src/directory.c

clean:
	rm *.o