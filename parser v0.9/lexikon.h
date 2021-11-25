/**
 *  Soubor: lexicon.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:17. 11. 2021 13:59:30
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

#define TOKEN_LENGTH 30
#define REALL_TOKEN_LEN 30

extern char states[11];
extern char keywords[15][10];
extern char c;
extern int i;



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

/**
 * @brief funkce pro dokončení tokenů ve start fázi
 * @param token struktura Token
 * @param i ukazatel na index atributu
 * @param string řetězec, který slouží na určení typu tokenu
 * @param c čtený charakter
 */
void STokenFinish(tToken *token, int *i, char *string, char c);

/**
 * @brief funkce pro tvorbu tokenů
 * @param token struktura Token
 * @return Vrací 0 pokud nenastane chyba, jiank 1
 */
int scanner(tToken *token);

tToken *token_init();

#endif // __LEXIKON__