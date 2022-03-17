CC=gcc
INC=.

OBJ= conf.o
CFLAGS=-Wall -DDEBUG

lib:utils.a

utils.a:conf.o
	ar -crs utils.a $(OBJECTS)

conf.o:conf.c conf.h
	$(CC) -c conf.c
