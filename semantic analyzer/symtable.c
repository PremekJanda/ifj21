/**
 *  Soubor: symtable.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	18. 11. 2021 20:36:23
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje implementaci tabulky symbolů
 */

#include "symtable.h"
#include "tree.h"

#include "test.h"

// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //

int symtable() {
    
    stack_t *stack = stack_init(STACK_SIZE);
    if (stack == NULL)
        return error_exit("Nepovedlo se alokovat zásobník do paměti!\n");

    // vytvoření hashovacích tabulek
    CREATE_HTAB(hash_table0)

    stack_push(&stack, hash_table0);

    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_return");
    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val_type");
    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val");
    
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_return");
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_val_type");
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_va");

    stack_print(stack);

    // uvolnění zásobníku z paměti
    stack_free(stack);  
    
    return 0;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích stránek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

stack_t * stack_init(size_t n) {
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

void stack_expand(stack_t **s, size_t new_size) {
    if (((*s)->top + 1) > (int)new_size) {
        warning_msg("Kapacita zásobníku tabulky symbolů nelze snížit z důvodu odstranění dat!\n");
        return;
    }

    // realokuje se prostor pro přidání dalších hashovacích tabulek
    *s = realloc(*s, sizeof(stack_t) + (sizeof(htab_t) * new_size));

    // projde celý předešlý zásobník a nakopíruje ho do nového
    for (size_t i = (*s)->size; i < new_size; i++) 
        (*s)->stack[i] = NULL;
    
    // aktualizuje se velikost zásobníku
    (*s)->size = new_size;
}

void stack_clear(stack_t *s) {
    // projde všechny prvky a vymaže je
    for (size_t i = 0; i < s->size; i++)
        if (s->stack[i] != NULL) 
            htab_free(s->stack[i]);
}

void stack_free(stack_t *s) {
    // vymaže všechna data ze stacku
    stack_clear(s);
    // uvolní paměť tabulky
    free(s);
}

void stack_push(stack_t **s, htab_t *t) {
    // chyba při plné tabulce
    if ((*s)->top == (int)((*s)->size - 1)) {
        warning_msg("Zásobník rámců tabulky symbolů je plný! Kapacita rozšířena z %ld na %ld.\n", (*s)->size, (*s)->size*2);
        // rozšíří zásobník
        stack_expand(&(*s), (*s)->size*2);
    }
    
    // zvýší ukazetel na vrchol zásobníku
    (*s)->top++;
    // přidá hashtabulku do zásobníku rámců
    (*s)->stack[(*s)->top] = t;
}

void stack_pop(stack_t *s) {
    // chyba při prázdné tabulce
    if (s->top == -1) {
        warning_msg("Zásobník rámců tabulky symbolů je prázdný!\n");
        return;
    }

    // vymaže daný rámec na zásobníku
    htab_free(s->stack[s->top]);
    s->stack[s->top] = NULL;
    // posune ukazatel níže
    s->top--;
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

htab_item_t * htab_lookup_add(htab_t *t, key_t key) {
    
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // inicializace dvou ukazaleů: prev_element bude ukazovat na element
    htab_item_t * element, * prev_element;
    element = prev_element = t->ptr_arr[index_in_arr];
    
    // v případě prvního elementu v řadě
    if (element == NULL) {
        // alokace a inicializace dat
        INIT_ELEMENT(element, key);
        t->ptr_arr[index_in_arr] = element;
        
        return element;
    }

    // hledání prvku iterováním přes lineárně vázaný seznam tabulky
    do {
        prev_element = element;
        
        // kontrola, jestli se se 'key' vyskytuje v tabulce
        if (strcmp(element->key, key) == 0) {
            // // element->data->value++;
            return element;
        }        
        element = element->next_h_item;
    } while (element != NULL);
    
    // prvek nebyl nalezen -> je vytvořen nový
    INIT_ELEMENT(element, key);    
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
            char *new_word;
            new_word = malloc(MAX_WORD_LEN + 1);
            strcpy(new_word, element->key);
            
            // přidá slovo do nové tabulky
            htab_item_t *new_element;
            if ((new_element = htab_lookup_add(new_hash_table, new_word)) == NULL) {
                error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word);
                exit(0);
            }

            // zkopírování četností klíčů do nové tabulky
            // // new_data->value = element->data->value;
                
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
// - - - - Funkce pro operace nad rámci funkcí - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - //

void fce_item_push(fce_item_t **i, key_t key) {
    // pokud se jedná o první prvek
    if ((*i) == NULL) {
        INIT_FCE((*i), key)
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
    if (item == NULL)
        return;
    
    do {
        // posun na další prvek,, aby nedošlo ke smazání ukazatele
        next_item = item->next_f_item;
        // uvolnění paměti
        free(item);
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

int def_table_add(char * name, def_table_t *deftable, bool data) {
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
            deftable->item[i].data = data;
            item_found = true;
            break;
        }
    }

    // pokud není nalezen, tak se vytvoří nový
    if (item_found == false) {
        deftable->item[deftable->size].name = (char *)malloc(strlen(name) + 1);
        ALLOC_CHECK(deftable->item[deftable->size].name)
        // zápis dat
        deftable->item[deftable->size].data = data;
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

void stack_print (const stack_t *s) {
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
    printf("\t\t%-3s ",i->key);
    
    if (i->fce == NULL) 
        printf("není fce\n");
    else
        fce_print(i->fce);
}

void fce_print(const fce_item_t *i) {
    size_t counter = 0;
    while(i != NULL) {
        if (counter == 0) {
            printf("Návratový typ: [%s] ", i->key);
            
        } else {
            if ((counter % 2) == 1) 
                printf("|typ:[%s],", i->key);
            else
                printf("hodnota:[%s]| -> ", i->key);
        }

        i = i->next_f_item;
        counter++;
    } 

    printf("-|\n");
}

void def_table_print(def_table_t deftable) {
    printf("\nTable of DEFINED or DECLARED FUNCTIONS");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    for (size_t i = 0; i < deftable.size; i++) 
        printf("%s %s\n", deftable.item[i].name, (deftable.item[i].data) ? "defined" : "declared");
}

int error_exit(const char *fmt, ...) {
    // ukazatel na blok argumentů
    va_list arguments;

    // začátek práce s argumenty
    va_start(arguments, fmt);

    // výpis CHYBA: na chybový výstup
    fprintf(stderr, "CHYBA: ");

    // výpis na chybový výstup
    vfprintf(stderr, fmt, arguments);

    // ukončení práce s argumenty
    va_end(arguments);   
    
    // ukončení programu
    return 0; 
}

void warning_msg(const char *fmt, ...) {
    // ukazatel na blok argumentů
    va_list arguments;

    // začátek práce s argumenty
    va_start(arguments, fmt);

    // výpis CHYBA: na chybový výstup
    fprintf(stderr, "VAROVÁNÍ: ");

    // výpis na chybový výstup
    vfprintf(stderr, fmt, arguments);

    // ukončení práce s argumenty
    va_end(arguments);  
}
