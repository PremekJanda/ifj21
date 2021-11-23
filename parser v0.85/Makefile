CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Wextra -g -O2
LDFLAG=-lm

all: parser

stack.o: stack.c stack.h
table.o: table.c table.h
parser.o: parser.c table.h stack.h tree.h lexikon.h
tree.o: tree.c tree.h
lexikon.o: lexikon.c lexikon.h

parser: parser.o table.o stack.o tree.o lexikon.o
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@