PRJ=lexikon
#
PROGS=$(PRJ)
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm

all: $(PROGS)

$(PRJ): $(PRJ).c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c
#