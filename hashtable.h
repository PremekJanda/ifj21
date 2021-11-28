#ifndef IAL_HASHTABLE_H
#define IAL_HASHTABLE_H

#include <stdbool.h>

#define MAX_HT_SIZE 101

/*
 * Veľkosť tabuľky s ktorou pracujú implementované funkcie.
 * Pre účely testovania je vhodné mať možnosť meniť veľkosť tabuľky.
 * Pre správne fungovanie musí byť veľkosť prvočíslom.
 */
extern int HT_SIZE;


// Prvek tabulky
typedef struct ht_item {
  char *key;            // klíč prvku
  int value;          // hodnota prvku
  struct ht_item *next; // ukazatel na další synonymum
} ht_item_t;

// Tabulka o reálné velikosti MAX_HT_SIZE
typedef ht_item_t *ht_table_t[MAX_HT_SIZE];

int get_hash(char *key);
void ht_init(ht_table_t *table);
ht_item_t *ht_search(ht_table_t *table, char *key);
void ht_insert(ht_table_t *table, char *key, int data);
int *ht_get(ht_table_t *table, char *key);
void ht_delete(ht_table_t *table, char *key);
void ht_delete_all(ht_table_t *table);

#endif
