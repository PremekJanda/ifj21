/**
 * @file hashtable.c
 * @authors Radomír Bábek - xbabek02, VUT FIT
 * @brief Mé osobní vypracování implementace funkcí hashovací tabulky z IAL úlohy
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	08. 12. 2021 00:43:23
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptilovací funkce
 */
int get_hash(char *key)
{
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++)
  {
    result += key[i];
  }
  return (result % HT_SIZE);
}

// inicializace hashovací tabulky
void ht_init(ht_table_t *table)
{
  for (int i = 0; i < MAX_HT_SIZE; i++)
  {
    (*table)[i] = NULL;
  }
}

/*
 * Vyhledávání uvnitř hashovací tabulky, v případě nalezení vrací ukazatel
 * na nalezený prvek, jinak vrací NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key)
{
  for (ht_item_t *var = (*table)[get_hash(key)]; var != NULL; var = var->next)
  {
    if (strcmp(key, var->key) == 0)
    {
      return var;
    }
  }
  return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvok s daným klíčem už v tabulce existuje, nahradí jeho hodnotu
 */
void ht_insert(ht_table_t *table, char *key, int value)
{
  ht_item_t *item;
  if ((item = ht_search(table, key)) != NULL)
  {
    item->value = value;
  }
  else
  {
    if ((item = malloc(sizeof(ht_item_t))) != NULL)
    { // funkce vrací void, nemam jak osetrit
      if ((item->key = malloc(strlen(key) + 1)) != NULL)
      {
        item->value = value;
        strcpy(item->key, key);

        ht_item_t *var = (*table)[get_hash(key)];
        (*table)[get_hash(key)] = item;
        item->next = var;
      }
    }
  }
}

/*
 * Smazání veškerých prvků hashovací tabulky
 */
void ht_delete_all(ht_table_t *table)
{
  ht_item_t *var = NULL;
  for (int i = 0; i < MAX_HT_SIZE; i++)
  {
    while ((*table)[i] != NULL)
    {
      var = (*table)[i];
      (*table)[i] = var->next;
      free(var->key);
      free(var);
    }

    (*table)[i] = NULL;
  }
}
