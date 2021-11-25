/**
 *  Soubor: symtable.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	25. 11. 2021 15:42:42
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Rozhraní pro tabulku smybolů
 */

// následující řádky zabrání násobnému vložení:
#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <stdio.h>                  // size_t
#include <string.h>                 // string fce
#include <stdbool.h>                // bool
#include <stdlib.h>                 // malloc
#include <stdint.h>                 // uint32
#include <stdarg.h>                 // pro va_list


// Nastavitelné parametry
#define HASH_TABLE_DIMENSION 5      // počet řádků hashovací tabulky
#define MAX_WORD_LEN 127            // maximální délka identifikátoru
#define STACK_SIZE 5                // velikost zásobníku pro tabulky symbolů
#define DEFTABLE_SIZE 10            // základní velikost tabulky funkcí
#define DECLARED 0                  // funkce již byla deklarována
#define DEFINED 1                   //                 definována
#define CALLED 2                    // funkce je volána


// - - - - - - - - - - - - - - - - - - - - //
// - - - - Datové typy a struktury - - - - //
// - - - - - - - - - - - - - - - - - - - - //
// Typy
typedef char * key_t;               // typ klíče
// // typedef const char * key_t;         // typ klíče
typedef int top_t;                  // typ hodnoty

// Prvním prvekem vázaného seznamu je návratová hodnota
// Další prvky jsou parametry funkce
typedef struct fce_item {
    key_t key;                      // parametry a datové typy
    struct fce_item *next_f_item;   // ukazatel na další prvek (parametr / dat. typ)
} fce_item_t;

// Položka seznamu
typedef struct htab_item {
    key_t key;                      // identifikátor
    key_t type;                     // datový typ výzaru
    key_t value;                    // hodnota (pokud je dána)
    bool local;                     // 1 - proměnná je lokální, 0 - není lokalní
    int ret_values;              // počet návratových nodnot
    fce_item_t *fce;                // ukazatel na strukturu funkce tabulky    
    struct htab_item *next_h_item;  // ukazatel na další prvek
} htab_item_t;

// Struktura hashovací tabulky
typedef struct htab {
    size_t arr_size;                // počet úvodních uzlů (řádků) tabulky
    htab_item_t *ptr_arr[];         // pole ukazatelů na úvodní prvky (uzly) v řadě
} htab_t;

// Struktura zásobníku rámců tabulek symbolů/identifikátorů
typedef struct symtable {
    size_t size;                    // velikost zásobníku
    top_t top;                      // ukazatel na vrchol zásobníku
    htab_t *stack[];                // pole hashovacích tabulek
} stack_t;

typedef struct def_table_item {
    bool called;                    // 0 => nebyla volána,  1 => byla volána funkce
    bool state;                     // 0 => deklarovaná,    1 => definovaná funkce
    char *name;
} def_table_item_t;

typedef struct def_table {
    size_t capacity;
    size_t size;
    def_table_item_t *item;
} def_table_t;


// - - - - - - - - - - - - - - - - - //
// - - - - - - - Makra - - - - - - - //
// - - - - - - - - - - - - - - - - - //

/**
 * @brief Uvolní data z paměti
 * @param e element/item Prvek k uvolnění z paměti
 */
#define FREE_ELEMENT(e) \
    if ((e)->type != NULL)  free((void *)(e)->type); \
    if ((e)->key != NULL)   free((void *)(e)->key); \
    if ((e)->value != NULL) free((void *)(e)->value); \
    free((e));

/**
 * @brief Uvolní data z paměti
 * @param e element/item Prvek k uvolnění z paměti
 */
#define FREE_FCE(i) \
    if ((i)->key != NULL) free((void *)(i)->key);  \
    free((i));

/**
 * @brief Alokování dat na hromadě a prvotní inincializace dat
 * @param e element/item 
 * @param k key Řetězec podle kterého se hledá
 */
#define INIT_ELEMENT(e, t, k, v, l, r) \
    /* alokování bloků na hromadě */ \
    (e) = (htab_item_t *)malloc(sizeof(htab_item_t)); \
        if ((e) == NULL) return NULL; \
    /* inicializace dat */ \
    (e)->type = (t); \
    (e)->key = (k); \
    (e)->value = (v); \
    (e)->local = (l); \
    (e)->ret_values = (r); \
    (e)->next_h_item = NULL; \
    (e)->fce = NULL;


/**
 * @brief Alokování dat na hromadě a prvotní inincializace dat
 * @param e element/item 
 * @param k key Řetězec podle kterého se hledá
 */
#define INIT_FCE(e, k) \
    (e) = (fce_item_t *)malloc(sizeof(fce_item_t)); \
    if ((e) == NULL) error_exit("Nepovedlo se alokovat položku vázaného seznamu parametrů funkce!\n"); \
    (e)->key = (k); \
    (e)->next_f_item = NULL;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích tabulek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Inicializuje zásobník
 * @param n Velikost zásobníku
 * @return Inicializovaný zásobník na hromadě
 */
stack_t * symtable_init (size_t n);

/**
 * @brief Rozšíří tabulku z původní velikosti na dvojnásobnou
 * @param s Struktura zásobníku tabulek symbolů
 * @param n Velikost zásobníku
 * @return Nově rozšířený zásobník
 */
void symtable_expand (stack_t **s, size_t n);

/**
 * @brief Vymaže všechny prvky zásobníku
 * @param s Struktura zásobníku tabulek symbolů
 */
void symtable_clear (stack_t *s);

/**
 * @brief Destruktor zásobníku
 * @param s Struktura zásobníku tabulek symbolů
 */
void symtable_free (stack_t *s);

/**
 * @brief Rozptylovací (hash) funkce
 * @param s Struktura zásobníku tabulek symbolů
 * @param t Hashovací tabulka
 */
void symtable_push (stack_t **s, htab_t *t);

/**
 * @brief Rozptylovací (hash) funkce
 * @param s Struktura zásobníku tabulek symbolů
 */
void symtable_pop (stack_t *s);

htab_item_t * symtable_lookup_item(stack_t *s, key_t id);



// - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro práci s tabulkou - - - - //
// - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Rozptylovací (hash) funkce
 * @param str Řetěrec, pro který se hledá index
 * @return Index prvku v tabulce
 */
size_t htab_hash_function(key_t str);

/**
 * @brief Konstruktor tabulky
 * @param n Velikost 'arr_size' tabulky
 * @return Ukazatel na nově-vytvořenou tabulku
 */
htab_t * htab_init (size_t n);      
              
/**
 * @brief Přesun dat do nově-přesunuté tabulky
 * @param n Velikost 'arr_size' přesunuté tabulky
 * @param from Odkaz na strukturu původní hashovací tabulky
 * @return Ukazatel na nově-přesunutou tabulku
 */
htab_t * htab_resize (size_t n, htab_t *from);     

/**
 * @brief Funkce pro vyhledání prvku v tabulce podle zadaného 'string' klíče
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá
 * @return V případě nalezení ukazatel na datový pár (key,value), v opačném NULL
 */
htab_item_t * htab_find (htab_t *t, key_t key);  

// TODO dokumentace
/**
 * @brief Funkce vyhledá v hashovací tabulce dané slovo a inkrementuje jeho četnost výskytu, pokud nenajde založí nový záznam
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá
 * @return Ukazatel na datový pár (key,value)
 */
htab_item_t * htab_lookup_add (htab_t *t, key_t type, key_t key, key_t value, bool local, int ret_values);

/**
 * @brief Ruší zadaný záznam
 * @param t Struktura hashovací tabulky
 * @param key Řetězec, podle kterého se hledá prvek na odstranění
 * @return Úspěch->true, Neúspěch->false
 */
bool htab_erase_item (htab_t *t, key_t key);

/**
 * @brief Ruší všechny záznamy
 * @param t Struktura hashovací tabulky
 */
void htab_clear (htab_t *t);
    
/**
 * @brief Destruktor tabulky
 * @param t Struktura hashovací tabulky
 */
void htab_free (htab_t *t);



// - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro operace nad rámci funkcí - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Přidá nový prvek do seznamu
 * @param i Struktura prvku seznamu
 * @param key Řetězec, který se vloží do paměti
 */
void fce_item_push(fce_item_t **i, key_t key);

/**
 * @brief Vmaže celý vázaný seznam
 * @param i Struktura prvku seznamu
 */
void fce_free(fce_item_t *i);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Tabulka definovaných čí deklarovaných fcí - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Inicializuje tabulku
 */
def_table_t * def_table_init();

/**
 * @brief Přidá nový nebo nahradí stávající prvek do tabulky
 * @param deftable Struktura tabulky funkcí
 * @param name Řetězec, který se vloží do paměti
 * @param data 0 - Deklarovaná fce, 1 - Definovaná fce
 */
int def_table_add(char *name, def_table_t *deftable, bool data);

/**
 * @brief Destruktor tabulky
 * @param deftable Struktura tabulky funkcí
 */
void def_table_free(def_table_t *deftable);



// - - - - - - - - - - - - - - - - //
// - - - -  Pomocné funkce - - - - //
// - - - - - - - - - - - - - - - - //

int symtable();

/**
 * @brief Projde všechny záznamy a zavolá na ně funkci f
 * @param t Struktura hashovací tabulky
 */
void symtable_print (const stack_t *t);

/**
 * @brief Projde všechny záznamy a zavolá na ně funkci f
 * @param t Struktura hashovací tabulky
 */
void htab_print (const htab_t *t);

/**
 * @brief Projde všechny záznamy a zavolá na ně funkci f
 * @param t Struktura hashovací tabulky
 */
void item_print (const htab_item_t *i);

/**
 * @brief Projde všechny záznamy a zavolá na ně funkci f
 * @param t Struktura hashovací tabulky
 */
void fce_print (const fce_item_t *i, size_t return_values);

/**
 * @brief Vypíše tabulku
 * @param deftable Struktura tabulky funkcí
 */
void def_table_print(def_table_t deftable);

/**
 * @brief Funkce vypíše chybovou hlášku na stderr
 * @param fmt Text chybové hlášky
 */
int error_exit(const char *fmt, ...);

/**
 * @brief Funkce vypíše upozornění na stderr
 * @param fmt Text zprávy upozornění
 */
void warning_msg(const char *fmt, ...);

#endif // __SYMTABLE_H__