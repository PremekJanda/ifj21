/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	25. 11. 2021 11:45:42
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje sémantické kontroly nad abstraktním syntaktickým stromem
 *         generovaným syntaktickou analýzou
 */

#include "semantic.h"

int return_signal = SEM_OK;

// TODO globální proměnné přidat do rámc a kontrolovat jejich parametry
// TODO 

// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //

// int main() {

//     // * TESTOVÁNÍ
//     MAKE_TREE()
//     // MAKE_SMALL_TREE()
//     // MAKE_F_DECL_TREE()
//     // MAKE_DEF_TABLE()
//     // MAKE_SYMTABLE()
    
//     printf("\n\n");


//     semantic(&node1);

    
//     // * TESTOVÁNÍ
//     node_delete(&node1);    
//     // symtable_free(stack);
    
//     return EXIT_SUCCESS;
// }



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Sémantická kontrola programu - exit(3)  - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int is_id_used(key_t id, stack_t *symtable, def_table_t deftable) {
    // hledání v tabulce symbolů
    for (top_t i = symtable->top; i >= 0; i--) 
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

int eval_fcall(def_table_t deftable) {
    for (size_t i = 0; i < deftable.size; i++) {
        // chyba pokud byla funkce volána, ale nebyla definována
        if (deftable.item[i].called && !deftable.item[i].state)
            return SEM_DEFINE;
    }

    return EXIT_SUCCESS;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Typová nekompatibilita výrazů - exit(6) - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int eval_expr_type(t_node *node, key_t *value, key_t *type) {
    // printf("[[%s %s %d]]", node->data[0].data, node->data[1].data, node->next_count);
    // printf("ROOTNODE: %s %s %d ~~~\n", node->data[0].data, node->data[1].data, node->next_count);

    // unární oprátor
    if (node->next_count == 2) {
        t_node *unary_node = (strcmp(node->next[0]->data[1].data, "#")) ? node->next[0] : node->next[1];
        
        if (!strcmp(unary_node->data[0].data, "string")) {
            free(*value);
            free(*type);

            ALLOC_STR(t, "integer")
            size_t len = strlen(unary_node->data[1].data);
            key_t v = malloc(len / 10 + 2);
            sprintf(v, "%ld", len);
            
            *type = t;
            *value = v;

            return SEM_OK;
        } 

        return SEM_ASSIGN;

    //binární operátory
    } else if (node->next_count == 3) {
        // levý operand
        if (eval_expr_type(node->next[0], value, type)) {
            free(*value);
            free(*type);
            return SEM_ASSIGN;
        }

        ALLOC_STR(type1, *type)
        ALLOC_STR(val1, *value)

        // printf("~~~val1    %s %s %d ~~~\n", type1, val1, node->next_count);
        
        // pravý operand
        if (eval_expr_type(node->next[2], value, type)) {
            free(*value); free(*type);
            free(type1);  free(val1);
            return SEM_ASSIGN;
        }

        ALLOC_STR(type2, *type)
        ALLOC_STR(val2, *value)

        // printf("~~~val2    %s %s %d ~~~\n", type2, val2, node->next_count);

        bool sem_assign = strcmp(type1, type2);

        // konverze z int na number
        if ((!strcmp("number", type1) && !strcmp("integer", type2)) || 
            (!strcmp("number", type2) && !strcmp("integer", type1))) {
            
            *type = realloc(*type, strlen("number") + 1);
            sprintf(*type, "%s", "number");

            // provedení konverze bylo v pořádku
            sem_assign = false;
        }
        
        // printf("??? %s %s => %d ???\n", type1, type2, sem_assign);
        
        // uvolnění zdrojů při chybě
        free(type1); free(type2);
        free(val1);  free(val2);

        return (sem_assign) ? SEM_ASSIGN : SEM_OK;
        
    } else if (node->next_count == 1) {
        eval_expr_type(node->next[0], value, type);
        return SEM_OK;

    } else if (node->next_count == 0) {
        free(*value);
        free(*type);

        ALLOC_STR(t, node->data[0].data)
        ALLOC_STR(v, node->data[1].data)
        
        *type = t;
        *value = v;

        return SEM_OK;
    }

    // rekurzivně (nejspíš) projít strom a vyhodit, zda všechny typy prvků souhlasí

    // po projití stromem naallokuje velikostu stringu, který bude zapisovat jako value a type

    return EXIT_SUCCESS;
}



// - - - - - - - - - - - - - - - - - - - //
// - - - -  Zpracování gramatiky - - - - //
// - - - - - - - - - - - - - - - - - - - //

// zohoduje se o tom, zda bude funkce definována, deklarována nebo volána
int process_main_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    // vytvoření globálního rámce
    _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    while (node->next_count) {
        TEMP_VARS()

        printf("--------------------------- %s -------------------------\n", next->data[0].data);

        // DEFINICE funkce
        if (strcmp(next->data[0].data, "function") == 0) {
            printf("function definition\n");
            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr->next[1]->data[1].data, symtable, *deftable)    ERR_()
            _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DEFINED)      ERR_()
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DEFINED) ERR_()

            _ERR() f_define(curr, symtable, deftable)                               ERR_()

        // DEKLARACE funkce nebo proměnné
        } else if (strcmp(next->data[0].data, "global") == 0) {

            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr->next[1]->data[1].data, symtable, *deftable)    ERR_()

            // určení, že jde o funkci
            if (curr->next[3]->next_count == 6) {
                printf("function declaration\n");
                _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DECLARED)     ERR_()
                // přidání id funkce do tabulky funkcí
                _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DECLARED) ERR_()
                
                _ERR() f_declare(curr, symtable)                              ERR_()
                
            // jde o proměnnou
            } else {
                    // alokace proměnných
                    ALLOC_STR(name, curr->next[1]->data[1].data);
                    ALLOC_STR(type, curr->next[3]->next[0]->next[0]->data[0].data);
                    // hodnota prozatím není známa
                    key_t value = NULL;

                    // INICIALIZACE
                    if (curr->next[3]->next[1]->next_count == 2) {
                        printf("global var initialization\n");
                        key_t assign_type = NULL;
                        
                        // vyhodnocení typu přiřazené hodnoty
                        if (eval_expr_type(curr->next[3]->next[1]->next[1], &value, &assign_type)) {
                            free(name);
                            free(type);
                            free(assign_type);
                            free(value);
                            return SEM_ASSIGN;
                        }
                        
                        // provede se typová kontrola nad přiřazením
                        if (strcmp(type, assign_type)) {
                            free(name);
                            free(type);
                            free(assign_type);
                            free(value);
                            return SEM_ASSIGN;
                        }
                        
                        free(assign_type);

                    // DEKLARACE
                    } else {
                        printf("global var declaration\n");
                    }

                    // přidání proměnné do tabulky
                    add_var_to_symtable(type, name, value, GLOBAL, symtable);

                    symtable_print(symtable);
            }

        // VOLÁNÍ funkce
        } else {
            printf("function call\n");
            _ERR() is_f_set(next->data[1].data, deftable, CALLED)               ERR_()
        }

        printf("\n");

        node = node->next[1];
    }

    return EXIT_SUCCESS;
}

// TODO
int process_stmt_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    while (node->next_count != 1) {
        TEMP_VARS()

        if (!strcmp(next->data[0].data, "<decl-local>")) {
            printf("local variable declaration/initialization\n");
            _ERR() is_id_used(next->next[1]->data[1].data, symtable, *deftable)     ERR_()

            // přidání proměnné do rámce
            _ERR() process_decl_local(node, symtable)                               ERR_()


        } else if (!strcmp(next->data[0].data, "<while>")) {
            printf("while\n");
            // _ERR() process_cond(curr)                                               ERR_()
            _ERR() process_while(curr, symtable, deftable)                          ERR_()
            
        } else if (!strcmp(next->data[0].data, "<if>")) {
            printf("if\n");
            // _ERR() process_cond(curr)                                               ERR_()
            _ERR() process_if(curr, symtable, deftable)                             ERR_()

        } else if (!strcmp(next->data[0].data, "<return>")) {
            printf("function returns\n");

        } else {
            printf("assign or function call\n");

            // TODO return check
        }

        node = node->next[1];
    }
    
    return EXIT_SUCCESS;
}

// TODO
int process_while(t_node *node, stack_t *symtable, def_table_t *deftable) {
    (void)node;
    (void)symtable;
    (void)deftable;

    // _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    return EXIT_SUCCESS;
}

// TODO
int process_if(t_node *node, stack_t *symtable, def_table_t *deftable) {
    (void)node;
    (void)symtable;
    (void)deftable;

    // _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    return EXIT_SUCCESS;
}

int process_cond(t_node *node) {
    printf("processing cond\n");
    key_t value1, type1, value2, type2;

    // ověření obou stran porovnání
    _ERR() eval_expr_type(node->next[0], &value1, &type1)                 ERR_()
    _ERR() eval_expr_type(node->next[2], &value2, &type2)                 ERR_()

    if (strcmp(type1, type2)) 
        return SEM_ASSIGN;

    // uvolnění paměti
    free(value1); free(type1);
    free(value2); free(type2);
    
    printf("cond succes\n");
    
    return EXIT_SUCCESS;
}

// TODO označení chyby
int process_decl_local(t_node *node, stack_t *symtable) {
    TEMP_VARS()

    // atribut a typ proměnné
    ALLOC_STR(name, next->next[1]->data[1].data);
    ALLOC_STR(type, next->next[3]->next[0]->data[0].data);
    // hodnota prozatím není známa
    key_t value = NULL;
    
    // INICIALIZACE
    if (next->next[4]->next_count != 1) {
        printf("local var initialization\n");

        // vyhodnocení, zda jsou všechny prvky expr stejného typu
        key_t assign_type = NULL;
        
        // provede se typová kontrola nad přiřazením
        if (eval_expr_type(next->next[4]->next[1]->next[0], &value, &assign_type)) {
            free(assign_type);
            free(value);
            free(name);
            free(type);
            return SEM_ASSIGN;
        }
        
        // printf("|||%s|||\n", next->next[3]->next[0]->data[0].data);

        // TODO zpracovat error
        if (strcmp(type, assign_type)) {
            free(name);
            free(type);
            free(assign_type);
            free(value);
            return SEM_ASSIGN;
        }

        free(assign_type);

    } else {
        printf("local var declaration -> value nil\n");
        ALLOC_CHECK((value = malloc(strlen("nil") + 1)))
        sprintf(value, "%s", "nil");
    }

    // DEKLARACE
    // přidání proměnné do tabulky symbolů
    add_var_to_symtable(type, name, value, LOCAL, symtable);

    return EXIT_SUCCESS;
}

int process_f_or_item_list() {
    
    return EXIT_SUCCESS;
}

int process_types(t_node *node, fce_item_t **item) {
    // nejprve určí první typ
    if (node->next_count != 1) {
        ALLOC_STR(item_key, node->next[0]->next[0]->data[1].data);
        fce_item_push(item, item_key);

        node = node->next[1];
        
        // projde type-list dokud se nenarazí na eps
        while (node->next_count != 1) {
            ALLOC_STR(ret_key_next, node->next[1]->next[0]->data[1].data);
            fce_item_push(item, ret_key_next);

            node = node->next[2];
        }
    }
    
    return EXIT_SUCCESS;
}

int process_return_types(t_node *node, fce_item_t **item, int *return_values) {
    printf("%s", "process return-types\n");

    if (node->next_count != 1) {
        ALLOC_STR(ret_key_first, node->next[1]->next[0]->data[0].data);
        fce_item_push(item, ret_key_first);

        node = node->next[2];
    
        // projde type-list dokud se nenarazí na eps
        while (node->next_count != 1) {
            ALLOC_STR(ret_key_next, node->next[1]->next[0]->data[0].data);
            fce_item_push(item, ret_key_next);

            node = node->next[2];

            (*return_values)++;
        }

        (*return_values)++;
    }

    printf("%s", "succes return-types\n");
    return EXIT_SUCCESS;
}

int process_f_arg_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    printf("%s", "process f-arg-list\n");
    
    // nejprve určí první typ
    if (node->next_count != 1) {
        // f-arg
        ALLOC_STR(type_first, node->next[0]->next[2]->next[0]->data[0].data);
        ALLOC_STR(key_first, node->next[0]->next[0]->data[1].data);
        ALLOC_STR(value_second, "");

        // přidání typu do lokální tabulky proměnných
        ALLOC_CHECK(htab_lookup_add(symtable->stack[symtable->top], type_first, key_first, value_second, LOCAL, -1))

        // přidání typu do listu parametrů funkce
        ALLOC_STR(arg_type_first, node->next[0]->next[2]->next[0]->data[0].data);
        fce_item_push(item, arg_type_first);

        // posun na další f-arg-another
        node = node->next[1];
        
        // f-arg-another
        while (node->next_count != 1) {
            // f-arg
            ALLOC_STR(type, node->next[1]->next[2]->next[0]->data[0].data);
            ALLOC_STR(key, node->next[1]->next[0]->data[1].data);
            ALLOC_STR(value, "");

            // přidání typu do lokální tabulky proměnných
            ALLOC_CHECK(htab_lookup_add(symtable->stack[symtable->top], type, key, value, LOCAL, -1))

            // přidání typu do listu parametrů funkce
            ALLOC_STR(arg_type, node->next[1]->next[2]->next[0]->data[0].data);
            fce_item_push(item, arg_type);

            // posun na další f-arg-another
            node = node->next[2];
        }
    }
    
    printf("%s", "success f-arg-list\n");
    return EXIT_SUCCESS;
}

int f_declare(t_node *node, stack_t *symtable) {
    // alokace potřebných proměnných
    ALLOC_STR(name, node->next[1]->data[1].data);
    ALLOC_STR(type, "");
    ALLOC_STR(value, "");

    // přidání prázdné funkce do tabulky symbolů
    htab_item_t *htab_i;
    fce_item_t *fce_i = NULL;
    ALLOC_CHECK((htab_i = htab_lookup_add(symtable->stack[0], type, name, value, 0, 0)))
    
    // zpracuje návratové hodnoty a jejich počet
    process_return_types(node->next[3]->next[5], &fce_i, &htab_i->ret_values);

    // typy paramentrů
    process_types(node->next[3]->next[3], &fce_i);

    // přiřazení ukazatele
    htab_i->fce = fce_i;

    return EXIT_SUCCESS;
}

// TODO přidání definice funkce do globálního rámce tabulky symbolů + sémantická kontrola nitra
int f_define(t_node *node, stack_t *symtable, def_table_t *deftable) {
    // přidání lokálního rámce funkce
    _ERR() add_scope_to_symtable(symtable)                              ERR_()

    // alokace potřebných proměnných
    ALLOC_STR(name, node->next[1]->data[1].data);
    ALLOC_STR(type, "");
    ALLOC_STR(value, "");

    // přidání prázdné funkce do tabulky symbolů
    htab_item_t *htab_i;
    fce_item_t *fce_i = NULL;
    ALLOC_CHECK((htab_i = htab_lookup_add(symtable->stack[0], type, name, value, 0, 0)))
    
    // zpracuje návratové hodnoty a jejich počet
    process_return_types(node->next[5], &fce_i, &htab_i->ret_values);

    // typy paramentrů
    process_f_arg_list(node->next[3], &fce_i, symtable);

    // přiřazení ukazatele
    htab_i->fce = fce_i;
    
    _ERR() process_stmt_list(node->next[6], symtable, deftable)         ERR_()

    // ! odkomentovat
    printf("\n+ + + + + + + + + + + + + + + + + + + + +\nZÁSOBNÍK PŘED UKONČENÍM FUNKCE %s \n", node->next[1]->data[1].data);
    symtable_print(symtable);
    symtable_pop(symtable);
    
    return EXIT_SUCCESS;
}



// - - - - - - - - - - - - - - - - - - - - - - - //
// - - - -  Operace nad tabulkou symbolů - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - //

// * local == 0 -> přiřazení hodnoty
// * local == 1 -> přidání do tabulky
int add_var_to_symtable(key_t type, key_t key, key_t value, bool local, stack_t *symtable) {
    // printf("[%s %s %s]", type, key, value);

    // kontrola, zda nejde o redeklaraci lokální proměnné v jednom rámci
    if (local) {
        htab_item_t *h_item = htab_find(symtable->stack[symtable->top], key);
        
        if (h_item != NULL)
            return SEM_DEFINE;

    // přiřazení hodnoty        
    } else {
        // do první nalezené lokální proměnné budu přepisovat hodnoty
        for (top_t i = symtable->top; i >= 0; i--) {
            htab_item_t *h_item = htab_find(symtable->stack[i], key);
            if (h_item == NULL)
                continue;

            if (h_item->local) {
                // přepíše hodnotu prvku po prvním nalezení lokální proměnné
                free(h_item->value);
                h_item->value = malloc(strlen(value) + 1); 
                ALLOC_CHECK(h_item->value)
                sprintf(h_item->value, "%s", value);
                break;
            }
        }
    }
    
    ALLOC_CHECK(htab_lookup_add(symtable->stack[symtable->top], type, key, value, local, -1))

    return EXIT_SUCCESS;
}

int add_scope_to_symtable(stack_t *symtable) {
    // inicializace nové hashovací tabulky
    htab_t *htab = htab_init(HASH_TABLE_DIMENSION);
    
    ALLOC_CHECK(htab)
        
    // přidání do zásobníku rámců
    symtable_push(&symtable, htab);

    return EXIT_SUCCESS;
}


int semantic(t_node *root_node) {
    // pokud je soubor prázdný, není třeba provádět sémantickou analýzu
    if (root_node->next_count == 3) {
        // pokud je dán pouze výraz require
        if (root_node->next[2]->next_count == 1)
            return SEM_OK;

        // inicializace 
        def_table_t *deftable = def_table_init();
        stack_t *symtable = symtable_init(STACK_SIZE);

        // zpracování hlavní části syntaktického stromu stromu
        return_signal = process_main_list(root_node->next[2], symtable, deftable);

        // * TESTOVANI
        printf("\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\nGLOBÁLNÍ ZÁSOBNÍK\n");
        symtable_print(symtable);

        printf("\nERRCODE: %d\n", return_signal);

        // kontrola, zda volané funkce byly řádně definovány
        if (!return_signal)
            return_signal = eval_fcall(*deftable);

        printf("\nERRCODE: %d\n", return_signal);

        // * PRINT
        def_table_print(*deftable);

        // * TESTOVÁNÍ
        symtable_free(symtable);
        def_table_free(deftable);
    }

    return return_signal;
}
