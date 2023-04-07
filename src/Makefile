#
#  AUTHOR:  John Graham
#  
#  FILE: 	Makefile
#


CC = gcc
CFLAGS = -c -g


all: jshell clean

jshell: jshell.o
	$(CC) jshell.o -o ../jshell

jshell.o: jshell.c
	$(CC) $(CFLAGS) jshell.c

clean:
	rm *.o