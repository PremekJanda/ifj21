/**
 *  Soubor: htab.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	14. 10. 2021 02:18:02
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Rozhraní pro hashovací tabulku
 */

// následující řádky zabrání násobnému vložení:
#ifndef __HTAB_H__
#define __HTAB_H__

#include <stdio.h>      // size_t
#include <string.h>     // string fce
#include <stdbool.h>    // bool
#include <stdlib.h>     // malloc
#include <stdint.h>     // uint32
#include <stdarg.h>     // pro va_list

// - - - - - - - - - - - - - - - - - - - - //
// - - - - Datové typy a struktury - - - - //
// - - - - - - - - - - - - - - - - - - - - //
// Tabulka
struct htab;                     // neúplná deklarace struktury - uživatel nevidí obsah
typedef struct htab htab_t;      // typedef podle zadání

// Typy
typedef const char * htab_key_t; // typ klíče
typedef int htab_value_t;        // typ hodnoty

// Dvojice dat v tabulce
typedef struct htab_pair {
    htab_key_t    key;           // klíč
    htab_value_t  value;         // asociovaná hodnota
} htab_pair_t;                   // typedef podle zadání

// Položka seznamu
typedef struct htab_item {
    htab_pair_t * data;
    struct htab_item * next;
} databox;

// Struktura tabulky
struct htab {
    size_t size;
    size_t arr_size;
    databox * ptr_arr[];
};

// Nastavitelný parametr počtu řádků tabulky
#define HASH_TABLE_DIMENSION 2
#define MAX_WORD_LEN 127


// - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro práci s tabulkou - - - - //
// - - - - - - - - - - - - - - - - - - - - - - //
/**
 * @brief Rozptylovací (hash) funkce
 * @param str Řetěrec, pro který se hledá index
 * @return Index prvku v tabulce
 */
size_t htab_hash_function(htab_key_t str);

/**
 * @brief Konstruktor tabulky
 * @param n Velikost 'arr_size' tabulky
 * @return Ukazatel na nově-vytvořenou tabulku
 */
htab_t *htab_init (size_t n);      
              
/**
 * @brief Přesun dat do nově-přesunuté tabulky
 * @param n Velikost'arr_size' přesunuté tabulky
 * @param from Odkaz na strukturu původní hashovací tabulky
 * @return Ukazatel na nově-přesunutou tabulku
 */
htab_t *htab_move (size_t n, htab_t *from);    
  
/**
 * @brief Získá počet záznamů v tabulce t->size
 * @param t Struktura tabulka
 * @return Vrátí počet prvků tabulky
 */
size_t htab_size (const htab_t * t);   
          
/**
 * @brief Získá ze struktury t->arr_size velikost pole
 * @param t Struktura hashovací tabulky
 * @return Vrátí velikost pole
 */
size_t htab_bucket_count (const htab_t * t);     

/**
 * @brief Funkce pro vyhledání prvku v tabulce podle zadaného 'string' klíče
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá
 * @return V případě nalezení ukazatel na datový pár (key,value), v opačném NULL
 */
htab_pair_t * htab_find (htab_t * t, htab_key_t key);  

/**
 * @brief Funkce vyhledá v hashovací tabulce dané slovo a inkrementuje jeho četnost výskytu, pokud nenajde založí nový záznam
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá
 * @return Ukazatel na datový pár (key,value)
 */
htab_pair_t * htab_lookup_add (htab_t * t, htab_key_t key);

/**
 * @brief Ruší zadaný záznam
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá prvek na odstranění
 * @return Úspěch->true, Neúspěch->false
 */
bool htab_erase (htab_t * t, htab_key_t key);

/**
 * @brief Projde všechny záznamy a zavolá na ně funkci f
 * @param t Struktura hashovací tabulky
 * @param f ukazatel na funkci s parameterm páru dat (key,value)
 */
void htab_for_each (const htab_t * t, void (*f)(htab_pair_t *data));

/**
 * @brief Ruší všechny záznamy
 * @param t Struktura hashovací tabulky
 */
void htab_clear (htab_t * t);
    
/**
 * @brief Destruktor tabulky
 * @param t Struktura hashovací tabulky
 */
void htab_free (htab_t * t);     

#endif // __HTAB_H__