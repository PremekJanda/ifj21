/**
 *  Soubor: symtable.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	07. 12. 2021 05:20:13
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje implementaci tabulky symbolů
 */

#include "symtable.h"
#include "tree.h"



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích stránek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

stack_t * symtable_init(size_t n) {
    // alokace paměti na hromadě
    stack_t *stack = malloc(sizeof(stack_t) + (sizeof(htab_t) * n));
    // v případě chyby alokace se ukončí program
    if (stack == NULL) 
        return NULL;

    // inicializace vstupních hodnot tabulky
    stack->size = n;
    stack->top = -1;
    
    // nastavení kořenových ukazatelů na NULL
    for (size_t i = 0; i < n; i++)
        stack->stack[i] = NULL;
    
    return stack;
}

void symtable_expand(stack_t **s, htab_t *t, size_t new_size) {
    if (((*s)->top + 1) > (int)new_size) {
        fprintf(stderr, "Capacity can not be reduced\n");
        return;
    }

    // realokuje se prostor pro přidání dalších hashovacích tabulek
    stack_t *new_symtable = realloc(*s, sizeof(stack_t) + (sizeof(htab_t) * new_size));
    *s = new_symtable;
    new_symtable = NULL;
    
    // projde celý předešlý zásobník a nakopíruje ho do nového
    for (size_t i = (*s)->size; i < new_size; i++) 
        (*s)->stack[i] = NULL;

    // aktualizuje se velikost zásobníku
    (*s)->size = new_size;

    // zvýší ukazetel na vrchol zásobníku
    (*s)->top++;
    // přidá hashtabulku do zásobníku rámců
    (*s)->stack[(*s)->top] = t;
}

void symtable_clear(stack_t *s) {
    // projde všechny prvky a vymaže je
    for (size_t i = 0; i < s->size; i++)
        if (s->stack[i] != NULL) 
            htab_free(s->stack[i]);
}

void symtable_free(stack_t *s) {
    // vymaže všechna data ze stacku
    symtable_clear(s);
    // uvolní paměť tabulky
    free(s);
}

void symtable_push(stack_t **s, htab_t *t) {
    // rozšíření plné tabulky symbolů
    if ((*s)->top == (int)((*s)->size - 1)) {
        // rozšíří zásobník
        symtable_expand(s, t, (*s)->size*2);
    } else {
        // zvýší ukazetel na vrchol zásobníku
        (*s)->top++;
        // přidá hashtabulku do zásobníku rámců
        (*s)->stack[(*s)->top] = t;
    }
}

void symtable_pop(stack_t *s) {
    // chyba při prázdné tabulce
    if (s->top == -1) 
        return;

    // vymaže daný rámec na zásobníku
    htab_free(s->stack[s->top]);
    s->stack[s->top] = NULL;
    // posune ukazatel níže
    s->top--;
}

htab_item_t * symtable_lookup_item(stack_t *s, key_t id) {
    // hledaný item
    htab_item_t *lookup_item;
    // hledání v tabulce symbolů
    for (top_t i = s->top; i >= 0; i--) 
        if ((lookup_item = htab_find(s->stack[i], id)) != NULL)
            return lookup_item;

    // prvek nebyl nalezen
    return NULL;
}



// - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro práci s tabulkou - - - - //
// - - - - - - - - - - - - - - - - - - - - - - //

size_t htab_hash_function(key_t str) {
    uint32_t hash = 0;     // 32 bitů
    const unsigned char *p;
    for(p = (const unsigned char *)str; *p != '\0'; p++)
        hash = (65599 * hash) + *p;

    return hash;
}

htab_t * htab_init(size_t n) {
    // alokace paměti na hromadě
    htab_t *hash_table = malloc(sizeof(htab_t) + (n * sizeof(htab_item_t)));
    // v případě chyby alokace se ukončí program
    if (hash_table == NULL) 
        return NULL;

    // inicializace vstupních hodnot zásobníku
    hash_table->arr_size = n;
    
    // nastavení kořenových ukazatelů na NULL
    for (size_t i = 0; i < n; i++) 
        hash_table->ptr_arr[i] = NULL;
    
    return hash_table;
}

htab_item_t * htab_find(htab_t *t, key_t key) {
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // nastavení ukazatele na první prvek
    htab_item_t * element = t->ptr_arr[index_in_arr];

    // iterace, dokud se nenarazí na NULL ptr
    while (element != NULL) {
        // porovnání, pokud se záznam vyskytuje -> vráti se
        if (strcmp(element->key, key) == 0)
            return element;

        element = element->next_h_item;
    }
    
    // pro případ nenalezení prvku
    return NULL;    
}

htab_item_t * htab_lookup_add(htab_t *t, key_t type, key_t key, key_t value, bool local, int ret_values) {
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // inicializace dvou ukazaleů: prev_element bude ukazovat na element
    htab_item_t * element, * prev_element;
    element = prev_element = t->ptr_arr[index_in_arr];
    
    // v případě prvního elementu v řadě
    if (element == NULL) {
        // alokace a inicializace dat
        INIT_ELEMENT(element, type, key, value, local, ret_values);
        t->ptr_arr[index_in_arr] = element;
        
        return element;
    }

    // hledání prvku iterováním přes lineárně vázaný seznam tabulky
    do {
        prev_element = element;
        
        // kontrola, jestli se se 'key' vyskytuje v tabulce
        if (strcmp(element->key, key) == 0) {
            element->value = value;
            return element;
        }        
        element = element->next_h_item;
    } while (element != NULL);
    
    // prvek nebyl nalezen -> je vytvořen nový
    INIT_ELEMENT(element, type, key, value, local, ret_values);    
    prev_element->next_h_item = element;

    return element;
}

htab_t * htab_resize(size_t n, htab_t *from) {
    htab_t * new_hash_table = htab_init(n);
    if (new_hash_table == NULL)
        return NULL;
    
    for (size_t i = 0; i < from->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        htab_item_t * element = from->ptr_arr[i];
        
        // projde postupně všechny prvky seznamu
        while(element != NULL) {
            // inicializace nového slova pro uložení do paměti
            char *type, *key, *value;
            type = malloc(MAX_WORD_LEN + 1);
            strcpy(type, element->type);
            key = malloc(MAX_WORD_LEN + 1);
            strcpy(key, element->key);
            value = malloc(MAX_WORD_LEN + 1);
            strcpy(value, element->value);
            
            // přidá slovo do nové tabulky
            htab_item_t *new_element;
            if ((new_element = htab_lookup_add(new_hash_table, type, key, value, element->local, element->ret_values)) == NULL) {
                free(type);
                free(key);
                free(value);
                return NULL;
            }
                
            element = element->next_h_item;
        }
    }

    htab_free(from);
    
    return new_hash_table;
}

bool htab_erase_item(htab_t *t, key_t key) {
    // získání indexu podle klíče 'key' v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // inicializace dvou ukazaleů: prev_element bude ukazovat na element
    htab_item_t * element, * prev_element;
    element = prev_element = t->ptr_arr[index_in_arr];
    
    // I. | NULL |
    // prázdný seznam
    if (element == NULL) 
        return false;

    // II. | del | -> | NULL |
    // prvek je první v seznamu, NEMÁ následníka
    if (element->next_h_item == NULL) 
        if (strcmp(element->key, key) == 0) {
            fce_free(element->fce);
            FREE_ELEMENT(element);
            
            t->ptr_arr[index_in_arr] = NULL;
            return true;
        }

    // III. | del | -> | ptr | -> ...
    // prvek je první v seznamu, MÁ následníka
    if (element->next_h_item != NULL) 
        if (strcmp(element->key, key) == 0) {
            t->ptr_arr[index_in_arr] = element->next_h_item;
            fce_free(element->fce);
            FREE_ELEMENT(element);
            
            return true;
        }    

    // IV. ... -> | ptr | -> | del | -> | NULL |
    //     ... -> | ptr | -> | del | -> | ptr  | -> ...
    // prvek je uprostřed nebo poslední v seznamu
    while (element->next_h_item != NULL) {
        prev_element = element;
        element = element->next_h_item;
        
        // kontrola, jestli se se 'key' vyskytuje v tabulce
        if (strcmp(element->key, key) == 0) {
            prev_element->next_h_item = element->next_h_item;
            fce_free(element->fce);
            FREE_ELEMENT(element);
            
            return true;
        }        
    }

    return false;
}

void htab_clear(htab_t *t) {  
    if (t == NULL)
        return;
    // projde pole ukazatelů na svázané listy
    for (size_t i = 0; i < t->arr_size; i++) {
        
        htab_item_t * del_data = t->ptr_arr[i];                 // ukazatel na první prvek v seznamu
        char del_str[MAX_WORD_LEN + 1];                         // buffer pro hledaný string
        
        // odstraňuje prvky dokud žádný v seznamu nezůstane
        while(del_data != NULL) {
            htab_item_t * new_del_data = del_data->next_h_item; // nastavení ukazatele na další prvek před vymazáním původního
            strcpy(del_str, del_data->key);                     // zkopírování hledaného textu do bufferu
            htab_erase_item(t, del_str);                        // uvolnění prvku z paměti
            del_data = new_del_data;                            // posun ukazatele na další prvek
        }
    }
}

void htab_free(htab_t *t) {
    if (t == NULL)
        return;
        
    // volá funkci clear pro vymazání dat z tabulky
    htab_clear(t);
    // uvolní strukturu tabulky
    free(t);
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro operace nad rámci funkcí - - ;- - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - //

void fce_item_push(fce_item_t **i, key_t key) {
    // pokud se jedná o první prvek
    if (*i == NULL) {
        INIT_FCE(*i, key)
        return;
    }
    
    // posun na poslední prvek
    fce_item_t *prev_item, *next_item;
    prev_item = next_item = *i;
    
    // postup na konec seznamu
    while ((next_item = next_item->next_f_item) != NULL) 
        prev_item = next_item;
    
    // inicializace dalšího prvku
    INIT_FCE(next_item, key)

    // nastavení ukazatele na další prvek
    prev_item->next_f_item = next_item;
}

void fce_free(fce_item_t *i) {
    fce_item_t *next_item, *item = i;
    
    // první prvek je NULL
    if (i == NULL)
        return;
    
    do {
        // posun na další prvek, aby nedošlo ke smazání ukazatele
        next_item = item->next_f_item;
        // uvolnění paměti
        FREE_FCE(item)
        // free(item);
    } while ((item = next_item) != NULL);
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Tabulka definovaných čí deklarovaných fcí - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

def_table_t * def_table_init() {
    def_table_t *deftable = malloc(sizeof(def_table_t));
    if (deftable == NULL)
        return NULL;
    
    // inicializace velikosti tabulky
    deftable->capacity = DEFTABLE_SIZE;
    deftable->size = 0;

    // alokace jednotlivých prvůů
    deftable->item = malloc(sizeof(def_table_item_t) * DEFTABLE_SIZE);
    if (deftable->item == NULL)
        return NULL;

    for (size_t i = 0; i < deftable->capacity; i++)
        deftable->item[i].name = NULL;

    return deftable;
}

int def_table_add(char *name, def_table_t *deftable, bool state) {
    if (deftable->size == deftable->capacity) {
        // rozšíření kapacity na dvojnásobek
        deftable->capacity *= 2;
        // realokace většího pole
        def_table_item_t * temp_deftable = realloc(deftable->item, sizeof(def_table_t) * deftable->capacity);
        ALLOC_CHECK(deftable->item)
        deftable->item = temp_deftable;
    }

    // pokud bude nalezen prvek, tak se přepíše hodnota
    bool item_found = false;
    for (size_t i = 0; i < deftable->size; i++) {
        if (strcmp(deftable->item[i].name, name) == 0) {
            deftable->item[i].state = state;
            deftable->item[i].called = 0;
            item_found = true;
            break;
        }
    }

    // pokud není nalezen, tak se vytvoří nový
    if (item_found == false) {
        deftable->item[deftable->size].name = (char *)malloc(strlen(name) + 1);
        ALLOC_CHECK(deftable->item[deftable->size].name)
        // zápis dat
        deftable->item[deftable->size].state = state;
        deftable->item[deftable->size].called = 0;
        // zkopírování nového názvu
        strcpy(deftable->item[deftable->size].name, name);
        // rozšíření pole
        deftable->size++;
    }

    return EXIT_SUCCESS;
}

void def_table_free(def_table_t *deftable) {
    
    for (size_t i = 0; i < deftable->size; i++)
        if (deftable->item[i].name != NULL)
            free(deftable->item[i].name);
    
    free(deftable->item);

    free(deftable);
}



// - - - - - - - - - - - - - - - - //
// - - - -  Pomocné funkce - - - - //
// - - - - - - - - - - - - - - - - //

void symtable_print (const stack_t *s) {
    printf("\nSTACK   size: %ld\n", s->size);
    for (top_t i = s->size - 1; i >= 0; i--) {
        printf("[%d]->%s\n", i, (s->stack[i] == NULL) ? "" : "h");
    }

    bool stack_empty = true;
    for (top_t i = s->size - 1; i >= 0; i--) {
        if (s->stack[i] == NULL) 
            continue;

        stack_empty = false;
        printf("\nHASHTABLE: stack[%d]\n", i);
        htab_print(s->stack[i]);
    }

    if (stack_empty)
        printf("\nStack is empty ¯\\_(ツ)_/¯\n");
}

void htab_print(const htab_t *t) {
    for (size_t i = 0; i < t->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        htab_item_t * element = t->ptr_arr[i];

        printf("\tLine %ld\n", i + 1);
        
        // projde postupně všechny prvky seznamu
        while(element != NULL) { 
            item_print(element);

            element = element->next_h_item;
        }
        printf("\n");
    }
}

void item_print(const htab_item_t *i) {
    printf("\t\ta: %-10s t: %-10s v: %-10s l: %-8s rv: %-4d \t|\t ", i->key, i->type, i->value, (i->local) ? "lok" : "glob", i->ret_values);
    
    if (i->fce == NULL && i->ret_values == -1) 
        printf("není fce\n");
    else
        fce_print(i->fce, i->ret_values);
}

void fce_print(const fce_item_t *i, size_t return_values) {
    size_t counter = 1;
    if (return_values == 0)
        printf("Žádné návratové hodnoty | ");

    while(i != NULL) {
        if (counter == return_values && return_values != 0)
            printf("[%s] | ", i->key);
        else 
            if (i->next_f_item != NULL)
                printf("[%s] -> ", i->key);
            else
                printf("[%s]", i->key);
              
        i = i->next_f_item;
        counter++;
    } 

    printf("\n");
}

void def_table_print(def_table_t deftable) {
    printf("\nTable of DEFINED or DECLARED FUNCTIONS");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    for (size_t i = 0; i < deftable.size; i++) 
        printf("%s %s %s\n", 
            deftable.item[i].name, 
            (deftable.item[i].state) ? "defined" : "declared", 
            (deftable.item[i].called) ? "called" : "not called"
        );
}
