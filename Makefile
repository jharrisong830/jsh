#
#  AUTHOR:  John Graham
#  
#  FILE: 	Makefile
#


CC = gcc
CFLAGS = -c -g


all: jshell clean

jshell: jshell.o
	$(CC) jshell.o -o jshell

jshell.o: src/jshell.c
	$(CC) $(CFLAGS) src/jshell.c

clean:
	rm *.o