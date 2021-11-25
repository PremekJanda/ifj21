PRJ=lexikon
#
PROGS=$(PRJ)
CC=gcc
CFLAGS=-std=c99 -g -Wall -Wextra -pedantic -lm
LDFLAG=-lm

all: $(PROGS)

$(PRJ): $(PRJ).c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c

clean: 
	rm -f *.o lexikon

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@
	
code-generator.o: code-generation/code-generator.c code-generation/code-generator.h
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@

mystring.o: code-generation/mystring.c code-generation/mystring.h
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@

code-generator: code-generator.o tree.o, mystring.o
	$(CC) $(CFLAGS) $^ $(LDFLAG) -o $@