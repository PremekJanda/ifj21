# Soubor: Makefile
#
# Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
# Datum poslední změny:	08. 12. 2021 17:46:34
# Autor: Přemek Janda - xjanda28, VUT FIT
# Popis: Program MAKEFILE pro překlad všech potřebných souborů

##### proměnné #####
CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -g -O2
OBJ = compiler.o lexikon.o parser.o table.o stack.o tree.o semantic.o symtable.o mystring.o hashtable.o code_generator.o

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
	zip xkocma08.zip *.c *.h precedence.txt rules.txt tabulka.txt Makefile built_in.fc rozdeleni
	
zip_test:
	@ $(MAKE) clean --no-print-directory
	zip -r test_compiler.zip *

clean:
	@ rm -f *.o *.zip compiler _out.txt _val.valgrind vgcore.*
	@ echo "Files successfully removed"
