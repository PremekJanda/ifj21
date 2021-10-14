# Soubor: Makefile
#
# Předmět: IJC - Projekt 2
# Datum poslední změny:	14. 10. 2021 02:24:49
# Autor: Přemek Janda - xjanda28, VUT FIT
# Popis: Program MAKEFILE pro překlad všech potřebných souborů

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.

##### proměnné #####
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

all: htab


##### programy #####
# htab
htab: htab.o
	$(CC) $(CFLAGS) $^ -o $@


##### objektové soubory #####
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: run zip clean
run:
	./htab
	
zip:
	zip htab.zip *.c *.h Makefile

clean:
	@rm -f *.o *.zip htab
	@echo "Files successfully removed"