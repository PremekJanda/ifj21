# Soubor: Makefile
#
# Předmět: IJC - Projekt 2
# Datum poslední změny:17. 11. 2021 14:03:29
# Autor: Přemek Janda - xjanda28, VUT FIT
# Popis: Program MAKEFILE pro překlad všech potřebných souborů

##### proměnné #####
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g
OBJ = tree.o symtable.o semantic.o

all: semantic

# OBJ = $(wildcard *.o)

##### programy #####
# semantic
semantic: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@


##### objektové soubory #####
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: run test zip clean
run:
	@ $(MAKE) --no-print-directory
	@ ./semantic > _out.txt
	@ echo "Make run done"

test:
	@ $(MAKE) --no-print-directory
	@ valgrind --leak-check=full --show-leak-kinds=all ./semantic < ../tests/test_sem_1.tl >_out.txt 2> _val.valgrind
# multiple files at once
# @ valgrind ./semantic < cat(../test/test_sem_1 ../test/test_sem_2 ../test/test_sem_3) >_out.txt 2> _val.valgrind


# @ valgrind ./semantic >_out.txt 2> _val.valgrind

# @ valgrind ./semantic >/dev/null 2> _val.valgrind
# @ ./semantic > _out.txt

	@ echo "Make test done"
	
zip:
	zip semantic.zip *.c *.h Makefile

clean:
	@ rm -f *.o *.zip semantic _out.txt _val.valgrind vgcore.*
	@ echo "Files successfully removed"
	