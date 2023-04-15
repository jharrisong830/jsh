#
#  AUTHOR:  John Graham
#  
#  FILE: 	Makefile
#


CC = gcc
CFLAGS = -c -g


all: jshell clean

jshell: jshell.o parse.o
	$(CC) jshell.o parse.o -o jshell

jshell.o: src/jshell.c
	$(CC) $(CFLAGS) src/jshell.c

parse.o: src/parse.c
	$(CC) $(CFLAGS) src/parse.c

clean:
	rm *.o