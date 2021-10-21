/**
 *  Soubor: htab.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	21. 10. 2021 12:07:14
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje implementaci hashovací tabulky
 */

#include "htab.h"


// - - - - - - - - - - - - - - - - //
// - - - -  Pomocné funkce - - - - //
// - - - - - - - - - - - - - - - - //
/**
 * @brief Vytiskne jeden prvek tabulky
 * @param data Pár dat (key,value) daného prvku
 */
void print_htab (htab_pair_t *data) {
    printf("|%s:%d|->", data->key, data->value);
}

/**
 * @brief Vytiskne celou tabulku zavoláním funkce 'htab_for_each'
 * @param t Hashovací tabulka
 */
void print_htab_all (htab_t *t) {    
    // procházení každého prvku
    htab_for_each(t, print_htab);
}

/**
 * @brief Funkce vypíše chybovou hlášku na stderr
 * @param fmt Text chybové hlášky
 */
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

/**
 * @brief Funkce vypíše upozornění na stderr
 * @param fmt Text zprávy upozornění
 */
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



// - - - - - - - - - - - - - - - - - //
// - - - - - - - Makra - - - - - - - //
// - - - - - - - - - - - - - - - - - //
/**
 * @brief Uvolní data z paměti
 * @param e element/item Prvek k uvolnění z paměti
 */
#define FREE_DATA(e) \
    free((void *)(e)->data->key); \
    free((e)->data); \
    free((e));

/**
 * @brief Alokování dat na hromadě a prvotní inincializace dat
 * @param e element/item 
 * @param k key Řetězec podle kterého se hledá
 */
#define INIT_ELEMENT(e, k, t) \
    /* alokování bloků na hromadě */ \
    (e) = (databox *)malloc(sizeof(databox));                if ((e) == NULL) return NULL; \
    (e)->data = (htab_pair_t *)malloc(sizeof(htab_pair_t));  if ((e) == NULL) return NULL; \
    /* inicializace dat */ \
    (e)->data->value = 1; \
    (e)->data->key = (k); \
    (e)->next = NULL;




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

stack_t * stack_resize(stack_t * s, size_t new_size) {
    // vytvoří nový 2x větší zásobník
    stack_t * new_stack = stack_init(new_size);

    // projde celý předešlý zásobník a nakopíruje ho do nového
    for (size_t i = 0; i <= s->size; i++) 
        new_stack->stack[i] = s->stack[i];
    
    // uvolní předešlý zásobník
    stack_free(s);

    return new_stack;
}

void stack_clear(stack_t * s) {
    // projde všechny prvky a vymaže je
    for (size_t i = 0; i < s->size; i++)
        htab_free(s->stack[i]);
    
}

void stack_free(stack_t * s) {
    // vymaže všechna data ze stacku
    stack_clear(s);
    // uvolní paměť tabulky
    free(s);
}

void stack_push(stack_t * s, htab_t * t) {
    // chyba při plné tabulce
    if (s->top == (s->size - 1)) {
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

void stack_pop(stack_t * s) {
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
// ? askdnaslkdnalskdnalskdn
size_t htab_hash_function(const char *str) {
    uint32_t hash = 0;     // 32 bitů
    const unsigned char *p;
    for(p = (const unsigned char *)str; *p != '\0'; p++)
        hash = (65599 * hash) + *p;

    return hash;
}

htab_t * htab_init(size_t n) {
    // alokace paměti na hromadě
    htab_t *hash_table = malloc(sizeof(htab_t) + (n * sizeof(databox)));
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

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)) {
    // projde pole ukazatelů na svázané listy
    for (size_t i = 0; i < t->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        databox * data = t->ptr_arr[i];

        printf("Line %ld: ", i + 1);
        // projde postupně všechny prvky seznamu
        while(data != NULL) {
            // aplikace funkce
            f(data->data);     
            data = data->next;
        }
        printf("NULL\n");
    }
}

htab_pair_t * htab_find(htab_t * t, htab_key_t key) {
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // nastavení ukazatele na první prvek
    databox * element = t->ptr_arr[index_in_arr];

    // iterace, dokud se nenarazí na NULL ptr
    while (element != NULL) {
        // porovnání, pokud se záznam vyskytuje -> vráti se
        if (strcmp(element->data->key, key) == 0)
            return element->data;

        // printf("next\n");
        element = element->next;
    }
    
    // pro případ nenalezení prvku
    return NULL;    
}

htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key) {
    
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // inicializace dvou ukazaleů: prev_element bude ukazovat na element
    databox * element, * prev_element;
    element = prev_element = t->ptr_arr[index_in_arr];
    
    // v případě prvního elementu v řadě
    if (element == NULL) {
        // alokace a inicializace dat
        INIT_ELEMENT(element, key, t);
        t->ptr_arr[index_in_arr] = element;
        
        return element->data;
    }

    // hledání prvku iterováním přes lineárně vázaný seznam tabulky
    do {
        prev_element = element;
        
        // kontrola, jestli se se 'key' vyskytuje v tabulce
        if (strcmp(element->data->key, key) == 0) {
            element->data->value++;
            return element->data;
        }        
        element = element->next;
    } while (element != NULL);
    
    // prvek nebyl nalezen -> je vytvořen nový
    INIT_ELEMENT(element, key, t);    
    prev_element->next = element;

    return element->data;
}

htab_t *htab_move(size_t n, htab_t *from) {
    htab_t * new_hash_table = htab_init(n);
    if (new_hash_table == NULL)
        return NULL;
    
    for (size_t i = 0; i < from->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        databox * element = from->ptr_arr[i];
        
        // projde postupně všechny prvky seznamu
        while(element != NULL) {

            // inicializace nového slova pro uložení do paměti
            char *new_word;
            new_word = malloc(MAX_WORD_LEN + 1);
            strcpy(new_word, element->data->key);
            
            // vyhledá slovo a inkrementuje četnost výskytu nebo ho přidá do seznamu
            htab_pair_t *new_data;
            if ((new_data = htab_lookup_add(new_hash_table, new_word)) == NULL) {
                error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word);
                exit(0);
            }

            // zkopírování četností klíčů do nové tabulky
            new_data->value = element->data->value;
                
            element = element->next;
        }
    }

    htab_clear(from);
    
    return new_hash_table;
}

void htab_clear(htab_t * t) {
    // projde pole ukazatelů na svázané listy
    for (size_t i = 0; i < t->arr_size; i++) {
        
        databox * del_data = t->ptr_arr[i];          // ukazatel na první prvek v seznamu
        char del_str[MAX_WORD_LEN + 1];              // buffer pro hledaný string
        
        // odstraňuje prvky dokud žádný v seznamu nezůstane
        while(del_data != NULL) {
            databox * new_del_data = del_data->next; // nastavení ukazatele na další prvek před vymazáním původního
            strcpy(del_str, del_data->data->key);    // zkopírování hledaného textu do bufferu
            htab_erase(t, del_str);                  // uvolnění prvku z paměti
            del_data = new_del_data;                 // posun ukazatele na další prvek
        }
    }
}

bool htab_erase(htab_t * t, htab_key_t key) {
    // získání indexu podle klíče 'key' v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // inicializace dvou ukazaleů: prev_element bude ukazovat na element
    databox * element, * prev_element;
    element = prev_element = t->ptr_arr[index_in_arr];
    
    // I. | NULL |
    // prázdný seznam
    if (element == NULL) 
        return false;

    // II. | del | -> | NULL |
    // prvek je první v seznamu, NEMÁ následníka
    if (element->next == NULL) 
        if (strcmp(element->data->key, key) == 0) {
            FREE_DATA(element);
            
            t->ptr_arr[index_in_arr] = NULL;
            return true;
        }

    // III. | del | -> | ptr | -> ...
    // prvek je první v seznamu, MÁ následníka
    if (element->next != NULL) 
        if (strcmp(element->data->key, key) == 0) {
            t->ptr_arr[index_in_arr] = element->next;

            FREE_DATA(element);
            
            return true;
        }    

    // IV. ... -> | ptr | -> | del | -> | NULL |
    //     ... -> | ptr | -> | del | -> | ptr  | -> ...
    // prvek je uprostřed nebo poslední v seznamu
    while (element->next != NULL) {
        prev_element = element;
        element = element->next;
        
        // kontrola, jestli se se 'key' vyskytuje v tabulce
        if (strcmp(element->data->key, key) == 0) {
            prev_element->next = element->next;
            
            FREE_DATA(element);
            
            return true;
        }        
    }

    return false;
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
        if(htab_find(hash_table, new_word) != NULL)
            free_word = true;

        if (htab_lookup_add(hash_table, new_word) == NULL)
            return error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word);
        
        if (free_word) free(new_word);  
    }

    // vytisknutí dat 'hash_table'
    print_htab_all(hash_table);

    // uvolnění tabulky z paměti
    htab_free(hash_table);  
    
    return 0;
}