/**
 *  Soubor: htab.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	14. 10. 2021 02:23:16
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
    printf("%s\t%d\n", data->key, data->value);
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
    /* free((e)->data->value); */ \
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
    (e)->next = NULL; \
    (t)->size++;




// - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro pro práci s tabulkou - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - //

size_t htab_bucket_count(const htab_t * t) {
    return t->arr_size;
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

    t->size = 0;
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

htab_pair_t * htab_find(htab_t * t, htab_key_t key) {
    // získání indexu podle klíče v tabulce
    size_t index_in_arr = htab_hash_function(key) % t->arr_size;
    // nastavení ukazatele na první prvek
    databox * element = t->ptr_arr[index_in_arr];

    // iterace, dokud se nenarazí na NULL ptr
    while (element != NULL) {
        // porovnání, pokud se záznam vyskytuje -> vráti se
        if (strcmp(element->data->key, key) == 0) {
            // printf("OK found: '%s' : '%s'\n", key, element->data->key);
            return element->data;
        }

        // printf("find: %s : %s\n", key, element->data->key);
        // printf("next\n");
        element = element->next;
    }
    
    // pro případ nenalezení prvku
    return NULL;    
}

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)) {
    
    // projde pole ukazatelů na svázané listy
    for (size_t i = 0; i < t->arr_size; i++) {
        // ukazatel na první prvek v seznamu
        databox * data = t->ptr_arr[i];
        
        // projde postupně všechny prvky seznamu
        while(data != NULL) {
            // aplikace funkce
            f(data->data);     
            data = data->next;
        }
    }
}

void htab_free(htab_t * t) {
    
    // volá funkci clear pro vymazání dat z tabulky
    htab_clear(t);
    
    // uvolní strukturu tabulky
    free(t);
}

size_t htab_hash_function(const char *str) {
    #ifndef HASHTEST
    uint32_t hash = 0;     // 32 bitů
    const unsigned char *p;
    for(p = (const unsigned char *)str; *p != '\0'; p++)
        hash = (65599 * hash) + *p;

    #else

    typedef unsigned short int HashIndexType;
    unsigned char Random8[256];

    HashIndexType hash;
    unsigned char h1, h2;

    if (*str == 0) return 0;
    h1 = *str; h2 = *str + 1;
    str++;
    while (*str) {
        h1 = Random8[h1 ^ *str];
        h2 = Random8[h2 ^ *str];
        str++;
    }
  
    // rozmezí indexu 0..65535 
    hash = ((HashIndexType)h1 << 8)|(HashIndexType)h2;
    
    #endif /* HASHTEST */

    return hash;
}

htab_t * htab_init(size_t n) {
    // alokace paměti na hromadě
    htab_t *hash_table = malloc(sizeof(htab_t) + (n * sizeof(databox)));
    // v případě chyby alokace se ukončí program
    if (hash_table == NULL) 
        return NULL;

    // inicializace vstupních hodnot tabulky
    hash_table->arr_size = n;
    hash_table->size = 0;
    
    // nastavení kořenových ukazatelů na NULL
    for (size_t i = 0; i < n; i++) 
        hash_table->ptr_arr[i] = NULL;
    
    return hash_table;
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

size_t htab_size(const htab_t * t) {
    return t->size;
}


// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //
int main ()
{
    // zavolání funkce pro inicializaci tabulky    
    htab_t *hash_table = htab_init(HASH_TABLE_DIMENSION);
    if (hash_table == NULL)
        return error_exit("Nepovedlo se alokovat tabulku do paměti!\n");
    
    // char word[MAX_WORD_LEN + 1];
    // int word_len = 0;
    // FILE *file_input = stdin;
    // bool warning = true;
    
    // // čte postupně všechna slova ze stdin
    // while ((word_len = read_word(word, MAX_WORD_LEN, file_input)) != EOF) {
    //     // prázdný řetězec bude přeskočen
    //     if (word_len == 0) continue;
    //     // vytisknutí chybové hlášky v případě delšího slova
    //     if (warning && word_len > MAX_WORD_LEN) {
    //         warning_msg("Bylo nalezeno příloš dlouhé slovo!\n");
    //         warning = false;
    //     }

    //     // inicializace nového slova pro uložení do paměti
    //     char *new_word;
    //     new_word = malloc(MAX_WORD_LEN + 1);
    //     strcpy(new_word, word);

    //     // pokud již zadané slovo existuje, vrátí se 
    //     bool free_word = false;
    //     if(htab_find(hash_table, new_word) != NULL)
    //         free_word = true;
        
    //     // vyhledá slovo a inkrementuje četnost výskytu nebo ho přidá do seznamu
    //     if (htab_lookup_add(hash_table, new_word) == NULL)
    //         return error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word);

    //     // pokud slovo bylo nalezeno, vyjme se z paměti
    //     if (free_word) free(new_word);
    // }

    #ifdef MOVETEST
    
        // přesunutí tabulky
        htab_t *new_hash_table = htab_move(5, hash_table);

        // vytisknutí dat 'new_hash_table'
        print_htab_all(new_hash_table);

        // uvolnění tabulky z paměti
        htab_free(new_hash_table);

    #else

        // vytisknutí dat 'hash_table'
        print_htab_all(hash_table);

    #endif  /* MOVETEST */

    // vytisknutí dat 'new_hash_table'
    print_htab_all(hash_table);

    // uvolnění tabulky z paměti
    htab_free(hash_table);  
    
    return 0;
}