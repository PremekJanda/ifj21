CC=gcc
CFLAGS=-std=c99 -g -Wall -Wextra -pedantic -lm
LDFLAG=-lm

all: code-generator

tree.o: tree.c tree.h
code-generator.o: code-generator.c code-generator.h 

mystring: mystring.c mystring.h
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@