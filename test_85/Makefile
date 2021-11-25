# Soubor: Makefile
#
# Předmět: IJC - Projekt 2
# Datum poslední změny:	25. 11. 2021 03:37:55
# Autor: Přemek Janda - xjanda28, VUT FIT
# Popis: Program MAKEFILE pro překlad všech potřebných souborů

##### proměnné #####
CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -g -O2
OBJ = compiler.o lexikon.o parser.o table.o stack.o tree.o semantic.o symtable.o #mystring.o hashtable.o code-generator.o

all: compiler

##### programy #####
# semantic
compiler: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@


##### objektové soubory #####
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: run test zip clean
run:
	@ ./compiler < program.txt > _out.txt
	@ echo "Make run done"

test:
	@ $(MAKE) clean --no-print-directory
	@ $(MAKE) --no-print-directory
	@ valgrind --leak-check=full --show-leak-kinds=all ./compiler < program.txt >_out.txt 2> _val.valgrind
	@ echo "Make test done"
	
zip:
	zip compiler.zip *.c *.h Makefile

clean:
	@ rm -f *.o *.zip compiler _out.txt _val.valgrind vgcore.*
	@ echo "Files successfully removed"
