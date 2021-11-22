/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	22. 11. 2021 19:28:27
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje sémantické kontroly nad abstraktním syntaktickým stromem
 *         generovaným syntaktickou analýzou
 */

#include "semantic.h"

int return_signal = SEM_OK;

// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //

int main() {

    // * TESTOVÁNÍ
    // MAKE_TREE()
    MAKE_SMALL_TREE()
    // MAKE_DEF_TABLE()
    // MAKE_SYMTABLE()
    
    printf("\n\n");


    semantic(&node1);

    
    // * TESTOVÁNÍ
    node_delete(&node1);
    // stack_free(stack);
    
    
    return EXIT_SUCCESS;
}



// #define VAR_NAME(var) #var

#define TYPE_CHECK(var_type, assign_type) \
    return (strcmp(var_type, assign_type) == 0) ? 0 : 1;
    

#define _ERR() \
    if ((return_signal = 

#define ERR_() \
    ) != 0) return return_signal; 


#define TEMP_VARS() \
    t_node curr = (*node).next[0]; \
    t_node next = (*node).next[0].next[0]; \
    (void)curr; \
    (void)next;

    // printf("curr: %s\n", curr.data[1].data);
    // printf("next: %s\n", next.data[1].data);

// #define TYPE_CHECK() 

int is_id_used(key_t id, stack_t *symtable, def_table_t deftable) {
    // hledání v tabulce symbolů
    for (top_t i = symtable->top; i >= 0; i++) 
        if (htab_find(symtable->stack[i], id) != NULL)
            return EXIT_FAILURE;
    
    // hledání v tabulce funkcí
    for (size_t i = 0; i < deftable.size; i++) 
        if (strcmp(deftable.item[i].name, id) == 0)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int is_f_set(key_t name, def_table_t *deftable, int state) {
    switch (state) {
        // definice nové funkce
        case DEFINED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable->item[i].name, name) == 0) {
                    (deftable->item[i].state == DEFINED) ?
                        printf("redefinice funkce\n") :
                        printf("deklarovanou funkci nelze znovu definovat\n");
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // deklarace nové funkce
        case DECLARED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable->item[i].name, name) == 0) {
                    (deftable->item[i].state == DECLARED) ?
                        printf("redeklarace funkce\n") :
                        printf("definovanou funkci nelze znovu deklarovat\n");
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // nová funkce je volána
        case CALLED:
            for (size_t i = 0; i < deftable->size; i++) {
                if (strcmp(deftable->item[i].name, name) == 0) {
                    // funkce musí být před zavoláním definována
                    if (deftable->item[i].state == DECLARED) {
                        printf("volání funkce, která nebyla definována (což může být ok)\n");
                        return EXIT_SUCCESS;
                    } else {
                        printf("call function %s\n", name);
                        // přidá příznak do tabulky funkcí, o volání funkce
                        deftable->item[i].called = 1;
                        return EXIT_SUCCESS;
                    }
                }
            }

        // funkce nebyla nalezena => chyba
        printf("volání nedeklarované či nedefinované funkce\n");
        return SEM_DEFINE;
    }
    
    return EXIT_SUCCESS;
}

// zohoduje se o tom, zda bude funkce definována, deklarována nebo volána
int process_main_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    // vytvoření globálního rámce
    // _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    while ((*node).next_count != 1) {
        TEMP_VARS()
        
        printf("(((%s))) (((%s)))\n", curr.data[0].data, curr.data[1].data);
        printf("(((%s))) (((%s)))\n", next.data[0].data, next.data[1].data);

        // DEFINICE funkce
        if (strcmp(next.data[1].data, "function") == 0) {
            printf("function definition\n");
            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr.next[1].data[1].data, symtable, *deftable)    ERR_()
            _ERR() is_f_set(curr.next[1].data[1].data, deftable, DEFINED)      ERR_()
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr.next[1].data[1].data, deftable, DEFINED) ERR_()

            // TODO přidání pouze definice funkce do globálního rámce tabulky symbolů
            // TODO f_define();

        // DEKLARACE funkce nebo proměnné
        } else if (strcmp(next.data[1].data, "global") == 0) {

            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr.next[1].data[1].data, symtable, *deftable)    ERR_()

            // určení, že jde o funkci
            if (curr.next[3].next_count == 3) {
                printf("function declaration\n");
                _ERR() is_f_set(curr.next[1].data[1].data, deftable, DECLARED)     ERR_()
                // přidání id funkce do tabulky funkcí
                _ERR() def_table_add(curr.next[1].data[1].data, deftable, DEFINED) ERR_()
                
                // TODO přidání funkce do globálního rámce tabulky symbolů + sémantická kontrola nitra
                // TODO f_declare();


            // jde o proměnnou
            } else {
                    key_t name = curr.next[1].data[1].data;
                    key_t type = curr.next[3].next[0].next[0].data[0].data;

                    key_t value = NULL;

                    // INICIALIZACE
                    if (curr.next[3].next[1].next_count == 2) {
                        printf("global var initialization\n");
                        key_t assign_type = NULL;
                        // _ERR() eval_expr_type(curr.next[3].next[1].next[1].next[0], value, assign_type)    ERR_()
                        _ERR() eval_expr_type(curr.next[3].next[1].next[1].next[0], &value, &assign_type)    ERR_()

                        printf("[%s m m %s]", assign_type, value);
                        
                        // provede se typová kontrola nad přiřazením
                        TYPE_CHECK(type, assign_type)


                        free((void*)assign_type);
                        free((void*)value);

                    // DEKLARACE
                    } else {
                        printf("global var declaration\n");
                    }

                    // přidání proměnné do tabulky
                    add_var_to_symtable(type, name, value, symtable);
            }

            // TODO - globální proměnné

        // VOLÁNÍ funkce
        } else {
            printf("function call\n");
            _ERR() is_f_set(next.data[1].data, deftable, CALLED)               ERR_()
        }

        printf("\n");

        node = &(node->next[1]);
    }

    return EXIT_SUCCESS;
}

// TODO
int process_stmt_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    while ((*node).next_count != 1) {
        TEMP_VARS()

        if (strcmp(next.data[1].data, "<decl-local>") == 0) {
            printf("local variable declaration\n");
            _ERR() is_id_used(next.next[1].data[1].data, symtable, *deftable)   ERR_()

            // přidání proměnné do rámce
            _ERR() process_decl_local(node, symtable)                           ERR_()

        } else if (strcmp(next.data[1].data, "<while>") == 0) {
            printf("while\n");
            // TODO
            // add_scope_to_symtable();
            
        } else if (strcmp(next.data[1].data, "<if>") == 0) {
            printf("if\n");
            // TODO
            // add_scope_to_symtable();

        } else if (strcmp(next.data[1].data, "<return>") == 0) {
            printf("function returns\n");

        } else {
            printf("assign or function call\n");

            // return check
        }
    }
    
    return EXIT_SUCCESS;
}

// TODO
int process_decl_local(t_node *node, stack_t *symtable) {
    key_t type = node->next[3].next[0].data[0].data;
    key_t name = node->next[1].data[1].data;
    key_t value = NULL;
    
    // DEFINICE
    if (node->next[4].next_count != 1) {
        printf("local var initialization\n");

        // vyhodnocení, zda jsou všechny prvky expr stejného typu
        key_t assign_type = NULL;
        // _ERR() eval_expr_type(node->next[4].next[1].next[0], value, assign_type)    ERR_()
        _ERR() eval_expr_type(node->next[4].next[1].next[0], &value, &assign_type)    ERR_()

        // provede se typová kontrola nad přiřazením
        TYPE_CHECK(type, assign_type)

        free(assign_type);
    } else {
        printf("global var declaration\n");
    }

    // DEKLARACE
    // přidání proměnné do tabulky symbolů
    add_var_to_symtable(type, name, value, symtable);

    return EXIT_SUCCESS;
}

int process_def_decl_fcall() {

    return EXIT_SUCCESS;
}

// int eval_expr_type(t_node node, key_t value, key_t type) {
int eval_expr_type(t_node node, key_t *value, key_t *type) {
    (void)node;

    // // ! DELETE LATER
    // value = (key_t)malloc(3);
    // sprintf(value, "%d", rand() % 50);

    // type = (key_t)malloc(10);
    // sprintf(type, "%d", rand() % 50);


    // ! DELETE LATER
    *value = (void*)malloc(3);
    sprintf(*value, "%d", rand() % 50);

    *type = (void*)malloc(10);
    sprintf(*type, "%d", rand() % 50);



    printf("[%s %s]", *type, *value);


    

    // free(*type);
    // free(*value);

    // rekurzivně (nejspíš) projít strom a vyhodit, zda všechny typy prvků souhlasí

    // po projití stromem naallokuje velikostu stringu, který bude zapisovat jako value a type

    return EXIT_SUCCESS;
}

int eval_fcall(def_table_t deftable) {
    for (size_t i = 0; i < deftable.size; i++)
        // chyba pokud byla funkce volána, ale nebyla definována
        if (deftable.item[i].called && !deftable.item[i].state)
            return SEM_DEFINE;

    return EXIT_SUCCESS;
}



// TODO upravit, aby se volala
// TODO key_t type;                // datový typ výzaru
// TODO key_t attribute;           // identifikátor
// TODO key_t value;               // hodnota (pokud je dána)
int add_var_to_symtable(key_t type, key_t key, key_t value, stack_t *symtable) {
    // (void)type;
    // (void)key;
    // (void)value;
    // (void)symtable;

    printf("[%s %s %s]", type, key, value);

    // 
    htab_lookup_add(symtable->stack[symtable->top], type, key, value);


    // TODO chci upravovat expr ?

    // TODO htab_add_var(name, type, value)

    return EXIT_SUCCESS;
}

int add_scope_to_symtable(stack_t *symtable) {
    htab_t *htab = htab_init(HASH_TABLE_DIMENSION);
    
    if (htab == NULL)
        return 99;
        
    stack_push(&symtable, htab);

    return EXIT_SUCCESS;
}



int semantic(t_node *root_node) {
    // pokud je soubor prázdný, není třeba provádět sémantickou analýzu
    if (root_node->next_count == 3) {
        // pokud je dán pouze výraz require
        if (root_node->next[2].next_count == 1)
            return SEM_OK;

        // inicializace 
        def_table_t *deftable = def_table_init();
        stack_t *symtable = stack_init(STACK_SIZE);

        // zpracování hlavní části syntaktického stromu stromu
        return_signal = process_main_list(&(root_node->next[2]), symtable, deftable);

        printf("\nERRCODE: %d\n", return_signal);

        // kontrola, zda volané funkce byly řádně definovány
        if (!return_signal)
            return_signal = eval_fcall(*deftable);

        printf("\nERRCODE: %d\n", return_signal);

        // * PRINT
        def_table_print(*deftable);

        // * TESTOVÁNÍ
        stack_free(symtable);
        def_table_free(deftable);
    }

    return return_signal;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích stránek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


