/**
 *  Soubor: htab.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	21. 10. 2021 21:21:29
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje implementaci hashovací tabulky
 */

#include "htab.h"

// - - - - - - - - - - - - - - - - - //
// - - - - - - - Makra - - - - - - - //
// - - - - - - - - - - - - - - - - - //
/**
 * @brief Uvolní data z paměti
 * @param e element/item Prvek k uvolnění z paměti
 */
#define FREE_DATA(e) \
    free((void *)(e)->fce->key);  \
    free((void *)(e)->key); \
    free((e)->fce); \
    free((e));

/**
 * @brief Alokování dat na hromadě a prvotní inincializace dat
 * @param e element/item 
 * @param k key Řetězec podle kterého se hledá
 */
#define INIT_ELEMENT(e, k, t) \
    /* alokování bloků na hromadě */ \
    (e) = (htab_item_t *)malloc(sizeof(htab_item_t));       if ((e) == NULL) return NULL; \
    (e)->fce = (fce_item_t *)malloc(sizeof(fce_item_t));    if ((e->fce) == NULL) return NULL; \
    /* inicializace dat */ \
    (e)->key = (k); \
    (e)->next_h_item = NULL; \
    (e)->fce->key = NULL; \
    (e)->fce->next_f_item = NULL; \




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

stack_t * stack_resize(stack_t *s, size_t new_size) {
    // vytvoří nový 2x větší zásobník
    stack_t * new_stack = stack_init(new_size);

    // projde celý předešlý zásobník a nakopíruje ho do nového
    for (size_t i = 0; i <= s->size; i++) 
        new_stack->stack[i] = s->stack[i];
    
    // uvolní předešlý zásobník
    stack_free(s);

    return new_stack;
}

void stack_clear(stack_t *s) {
    // projde všechny prvky a vymaže je
    for (size_t i = 0; i < s->size; i++)
        htab_free(s->stack[i]);
    
}

void stack_free(stack_t *s) {
    // vymaže všechna data ze stacku
    stack_clear(s);
    // uvolní paměť tabulky
    free(s);
}

void stack_push(stack_t *s, htab_t *t) {
    // chyba při plné tabulce
    if (s->top == (int)(s->size - 1)) {
        warning_msg("Zásobník rámců tabulek symbolů je plný! Kapacita rozšířena z %ld na %ld.\n", s->size, s->size*2);
        // rozšíří zásobník
        stack_resize(s, s->size*2);
        return;
    }
    
    // zvýší ukazetel na vrchol zásobníku
    s->top++;
    // přidá hashtabulku do zásobníku rámců
    s->stack[s->top] = t;
}

void stack_pop(stack_t *s) {
    // chyba při prázdné tabulce
    if (s->top == -1) {
        warning_msg("Zásobník rámců tabulek symbolů je prázdný!\n");
        return;
    }

    // vymaže daný rámec na zásobníku
    htab_free(s->stack[s->top]);
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

htab_item_t * htab_find(htab_t * t, key_t key) {
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
        INIT_ELEMENT(element, key, t);
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
    INIT_ELEMENT(element, key, t);    
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

bool htab_erase_item(htab_t * t, key_t key) {
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
            FREE_DATA(element);
            
            t->ptr_arr[index_in_arr] = NULL;
            return true;
        }

    // III. | del | -> | ptr | -> ...
    // prvek je první v seznamu, MÁ následníka
    if (element->next_h_item != NULL) 
        if (strcmp(element->key, key) == 0) {
            t->ptr_arr[index_in_arr] = element->next_h_item;

            FREE_DATA(element);
            
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
            
            FREE_DATA(element);
            
            return true;
        }        
    }

    return false;
}

void htab_clear(htab_t * t) {
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

void htab_free(htab_t * t) {
    // volá funkci clear pro vymazání dat z tabulky
    htab_clear(t);
    
    // uvolní strukturu tabulky
    free(t);
}




// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //
int main () {
    // zavolání funkce pro inicializaci tabulky    
    htab_t *hash_table = htab_init(HASH_TABLE_DIMENSION);
    if (hash_table == NULL)
        return error_exit("Nepovedlo se alokovat tabulku do paměti!\n");

    // testování funkcionality tabulky na 200 číslech
    for (int i = 0; i < 200; i++) {
        char *new_word;
        new_word = malloc(MAX_WORD_LEN);
        sprintf(new_word, "%d", rand() % 50);

        bool free_word = false;
        // pokud bude identifikátor nalezen, bude později odstraněn
        if(htab_find(hash_table, new_word) != NULL) {
            free_word = true;        
        } else {
            // vytvoří nový záznam v tabulce
            if (htab_lookup_add(hash_table, new_word) == NULL)
                return error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word);
        }
        
        // alokovaný identifikátor již existuje v tabulce a může být odstraněn
        if (free_word) 
            free(new_word);  
    }

    // vytisknutí dat 'hash_table'
    htab_print(hash_table);

    // uvolnění tabulky z paměti
    htab_free(hash_table);  
    
    return 0;
}


// - - - - - - - - - - - - - - - - //
// - - - -  Pomocné funkce - - - - //
// - - - - - - - - - - - - - - - - //

void htab_print (const htab_t * t) {
    for (size_t i = 0; i < t->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        htab_item_t * element = t->ptr_arr[i];

        printf("Line %ld: ", i + 1);
        // projde postupně všechny prvky seznamu
        while(element != NULL) {
            printf("|%s|->", element->key); 
            element = element->next_h_item;
        }
        printf("NULL\n");
    }
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
