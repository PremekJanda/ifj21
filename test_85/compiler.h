/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	25. 11. 2021 02:42:14
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: ORozhraní propojující jednotlivé komponenty
 */

// lexikální analýza
#include "lexikon.h"

// syntaktickáanalýza
#include "parser.h"

// sémantická analýza
#include "semantic.h"

// generování kódu
#include "code-generator.h"

// chybové návratové hodnoty
#define LEXICAL_ERROR 1
#define SYNTAX_ERROR 2
#define ALLOC_ERROR 99
