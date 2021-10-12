PRJ=lexikon
#
PROGS=$(PRJ)
CC=gcc
CFLAGS=-std=c99 -g -Wall -Wextra -pedantic -lm

all: $(PROGS)

$(PRJ): $(PRJ).c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c

clean: 
	rm -f *.o lexikon
