/**
 *  Soubor: lexicon.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	14. 10. 2021 00:45:24
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Rozhraní pro lexikon.c
 */



// následující 2 řádky zabrání násobnému vložení
#ifndef __LEXIKON__
#define __LEXIKON__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>



// - - - - - - - - - - - - - - - - - - - - //
// - - - - Datové typy a struktury - - - - //
// - - - - - - - - - - - - - - - - - - - - //

typedef struct Token {
    char *type;
    char *attribute;
    int line;
} tToken;

#define TOKEN_LENGTH 100
#define REALL_TOKEN_LEN 100
char keywords[15][10] = {"do", "else", "end", "function", "global", "if", "integer", "local", "nil", "number", "require", "return", "string", "then", "while"};



// - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro vytváření tokenů - - - - //
// - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Inicializace nového tokenu
 * @param token Struktura token
 */
void initToken(tToken *token);

/**
 * @brief Vymaže token
 * @param token Struktura token
 */
void deleteToken(tToken *token);

/**
 * @brief Testovací funkce pro vytisknutí atributů tokenu
 * @param token Struktura token
 */
void printToken(tToken *token);

/**
 * @brief Rozptylovací (hash) funkce
 * @param token Struktura token
 * @return Vrací TRUE, pokud je keyword, else FALSE
 */
bool IsKeyWord(tToken *token);

#endif // __LEXIKON__