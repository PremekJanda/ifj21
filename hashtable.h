/**
 * @file hashtable.h
 * @authors Radomír Bábek - xbabek02, VUT FIT
 * @brief Mé osobní vypracování implementace funkcí hashovací tabulky z IAL úlohy
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	08. 12. 2021 01:08:22
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef IAL_HASHTABLE_H
#define IAL_HASHTABLE_H

#include <stdbool.h>

#define MAX_HT_SIZE 101

// Velikost tabulky
extern int HT_SIZE;

// Prvek tabulky
typedef struct ht_item
{
  char *key;            // klíč prvku
  int value;            // hodnota prvku
  struct ht_item *next; // ukazatel na další synonymum
} ht_item_t;

// Tabulka o reálné velikosti MAX_HT_SIZE
typedef ht_item_t *ht_table_t[MAX_HT_SIZE];

int get_hash(char *key);
void ht_init(ht_table_t *table);
ht_item_t *ht_search(ht_table_t *table, char *key);
void ht_insert(ht_table_t *table, char *key, int data);
void ht_delete_all(ht_table_t *table);

#endif
