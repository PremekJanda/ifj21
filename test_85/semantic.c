/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	26. 11. 2021 09:17:29
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
    
//     return SEM_OK;
// }



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Sémantická kontrola programu - exit(3)  - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int is_id_used_locally(key_t id, stack_t *symtable, def_table_t deftable) {
    // id nesmí být v tabulce funkcí
    for (size_t i = 0; i < deftable.size; i++) 
        if (strcmp(deftable.item[i].name, id) == 0)
            return SEM_DEFINE;

    // proměnná nesmí být ve stejném rámci již deklarována
    if (htab_find(symtable->stack[symtable->top], id) != NULL) 
        return SEM_DEFINE;    

    return SEM_OK;
}

int is_id_used(key_t id, stack_t *symtable, def_table_t deftable) {
    // hledání v tabulce symbolů
    for (top_t i = symtable->top; i >= 0; i--) 
        if (htab_find(symtable->stack[i], id) != NULL)
            return SEM_DEFINE;
    
    // hledání v tabulce funkcí
    for (size_t i = 0; i < deftable.size; i++) 
        if (strcmp(deftable.item[i].name, id) == 0)
            return SEM_DEFINE;

    return SEM_OK;
}

int is_var_set(key_t name, stack_t *symtable) {
    // hledání v tabulce symbolů
    htab_item_t *item;
    for (top_t i = symtable->top; i >= 0; i--)
        if ((item = htab_find(symtable->stack[i], name)) != NULL)
            return (item->ret_values == -1) ? SEM_OK : SEM_DEFINE;

    return SEM_DEFINE;
}

key_t get_var_type(key_t name, stack_t *symtable) {
    // hledání v tabulce symbolů
    htab_item_t *item;
    for (top_t i = symtable->top; i >= 0; i--) {
        if ((item = htab_find(symtable->stack[i], name)) != NULL) {
            if (item->ret_values == -1) {
                // v případě nalezení proměnné se načte její typ
                // name = realloc(name, strlen(item->type) + 1);
                // strcpy(name, item->type);
                // return SEM_OK;
                
                // ALLOC_STR(type, item->type)
                // free(name);
                // name = type;
                // return SEM_OK;
                return item->type;
            } else {
                return NULL;
            }
        }
    }

    return NULL;
}

int is_f_set(key_t name, def_table_t *deftable, int state) {
    switch (state) {
        // definice nové funkce
        case DEFINED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable->item[i].name, name) == 0) {
                    // (deftable->item[i].state == DEFINED) ?
                        // printf("redefinice funkce\n") :
                        // printf("deklarovanou funkci nelze znovu definovat\n");
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // deklarace nové funkce
        case DECLARED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable->item[i].name, name) == 0) {
                    // (deftable->item[i].state == DECLARED) ?
                        // printf("redeklarace funkce\n") :
                        // printf("definovanou funkci nelze znovu deklarovat\n");
                    
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
                        // printf("volání funkce, která nebyla definována (což může být ok)\n");
                        return SEM_OK;
                    } else {
                        // printf("call function %s\n", name);
                        // přidá příznak do tabulky funkcí, o volání funkce
                        deftable->item[i].called = 1;
                        return SEM_OK;
                    }
                }
            }

        // funkce nebyla nalezena => chyba
        // printf("volání nedeklarované či nedefinované funkce\n");
        return SEM_DEFINE;
    }
    
    return SEM_OK;
}

int eval_fcall(def_table_t deftable) {
    for (size_t i = 0; i < deftable.size; i++) {
        // chyba pokud byla funkce volána, ale nebyla definována
        // if (deftable.item[i].called && !deftable.item[i].state)
        //     return SEM_DEFINE;
        // chyba pokud nebyla definována
        if (!deftable.item[i].state)
            return SEM_DEFINE;
    }

    return SEM_OK;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Typová nekompatibilita výrazů - exit(6) - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int eval_expr_type(t_node *node, key_t *value, key_t *type, stack_t *symtable) {
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

        return SEM_TYPE;

    //binární operátory
    } else if (node->next_count == 3) {
        int sem_type = SEM_OK;

        // levý operand
        if ((sem_type = eval_expr_type(node->next[0], value, type, symtable))) {
            free(*value);
            free(*type);
            return sem_type;
        }

        ALLOC_STR(type1, *type)
        ALLOC_STR(val1, *value)

        // printf("~~~val1    %s %s %d ~~~\n", type1, val1, node->next_count);
        
        // pravý operand
        if ((sem_type = eval_expr_type(node->next[2], value, type, symtable))) {
            free(*value); free(*type);
            free(type1);  free(val1);
            return sem_type;
        }

        ALLOC_STR(type2, *type)
        ALLOC_STR(val2, *value)

        // printf("~~~val2    %s %s %d ~~~\n", type2, val2, node->next_count);
        
        sem_type = strcmp(type1, type2);

        // konverze z int na number
        if ((!strcmp("number", type1) && !strcmp("integer", type2)) || 
            (!strcmp("number", type2) && !strcmp("integer", type1))) {
            
            *type = realloc(*type, strlen("number") + 1);
            sprintf(*type, "%s", "number");

            // provedení konverze bylo v pořádku
            sem_type = SEM_OK;
        }
        
        // printf("??? %s %s => %d ???\n", type1, type2, sem_type);
        
        // uvolnění zdrojů při chybě
        free(type1); free(type2);
        free(val1);  free(val2);

        return sem_type;
        
    } else if (node->next_count == 1) {
        return eval_expr_type(node->next[0], value, type, symtable);
    } else if (node->next_count == 0) {
        free(*value);
        free(*type);

        ALLOC_STR(t, node->data[0].data)
        ALLOC_STR(v, node->data[1].data)

        // pokud se narazí na proměnnou
        if (!strcmp(t, "id")) {
            htab_item_t *item;
            
            // vyhledá se id v tabulce symolů
            if ((item = symtable_lookup_item(symtable, v)) != NULL) {
                // nahrazení za nový typ a hodnotu
                free(t); free(v);
                ALLOC_STR(t, item->type)
                ALLOC_STR(v, item->value)

                UPDATE_EXPR(SEM_OK)

            // proměnná nebyla nalezena -> chyba
            } else {
                // printf("!! id not found !!");
                UPDATE_EXPR(SEM_DEFINE)
            }
        }

        UPDATE_EXPR(SEM_OK)
    }

    return SEM_OK;
}


int eval_list_eq(fce_item_t *dest, fce_item_t *src, int err_code) {

    // fce_print(src, 99);
    // fce_print(dest, 99);
    // printf("%s", src->next_f_item);
    // printf("%s", dest->next_f_item);

    if ((dest == NULL && src != NULL) || (dest != NULL && src == NULL))
        return err_code;

    while (dest != NULL && src != NULL) {
        
        if (strcmp(dest->key, src->key))
            return err_code;

        dest = dest->next_f_item;
        src = src->next_f_item;
    }

    if ((dest == NULL && src != NULL) || (dest != NULL && src == NULL))
        return err_code;

    return SEM_OK;
}

int eval_param_list(key_t name, fce_item_t *dest, stack_t *symtable, def_table_t *deftable) {
    if (!strcmp(name, "write"))
        return SEM_OK;

    if (is_f_set(name, deftable, 3))
        return SEM_DEFINE;
    
    htab_item_t *item = symtable_lookup_item(symtable, name);
    if (item == NULL)
        return SEM_DEFINE;
        
    fce_item_t *params = item->fce;
    int counter = item->ret_values;

    while (counter != 0 && params != NULL) {
        params = params->next_f_item;
        counter--;
    }

    // fce_print(params, 99);
    // fce_print(dest, 99);
    // printf("%s", *params);
    // printf("%s", *dest);
    // printf("%s", name);

    return eval_list_eq(params, dest, SEM_FCE);
}

// int eval_list_ret_assign(fce_item_t *dest, fce_item_t *src) {
//     (void **)dest;   
//     (void **)src;

//     return SEM_OK;
// }


// - - - - - - - - - - - - - - - - - - - //
// - - - -  Zpracování gramatiky - - - - //
// - - - - - - - - - - - - - - - - - - - //

// zohoduje se o tom, zda bude funkce definována, deklarována nebo volána
int process_main_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    // vytvoření globálního rámce
    _ERR() add_scope_to_symtable(symtable)                                      ERR_()
    
    // vygenerování vestavěných funkcí do globální tabulky symbolů
    GENERATE_BUILTIN_FUNCTIONS()

    while (node->next_count > 1) {
        TEMP_VARS()

        // printf("--------------------------- %s -------------------------\n", next->data[1].data);

        // DEFINICE funkce
        if (strcmp(next->data[1].data, "function") == 0) {
            // printf("function definition\n");
            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr->next[1]->data[1].data, symtable, *deftable)    ERR_()
            _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DEFINED)      ERR_()
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DEFINED) ERR_()

            _ERR() f_define(curr, symtable, deftable)                               ERR_()

        // DEKLARACE funkce nebo proměnné
        } else if (strcmp(next->data[1].data, "global") == 0) {

            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr->next[1]->data[1].data, symtable, *deftable)    ERR_()

            // určení, že jde o funkci
            if (curr->next[3]->next_count == 5) {
                // printf("function declaration\n");
                _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DECLARED)     ERR_()
                // přidání id funkce do tabulky funkcí
                _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DECLARED) ERR_()
                
                _ERR() f_declare(curr, symtable)                              ERR_()
                
            // jde o proměnnou
            } else {
                    // alokace proměnných
                    ALLOC_STR(name, curr->next[1]->data[1].data);
                    ALLOC_STR(type, curr->next[3]->next[0]->next[0]->data[1].data);
                    // hodnota prozatím není známa
                    key_t value = NULL;

                    // INICIALIZACE
                    if (curr->next[3]->next[1]->next_count == 2) {
                        // printf("global var initialization\n");
                        key_t assign_type = NULL;
                        
                        // vyhodnocení typů přiřazovaných hodnot
                        if (eval_expr_type(curr->next[3]->next[1]->next[1], &value, &assign_type, symtable)) {
                            FREE_VAR_DECL()
                            return SEM_TYPE;
                        }
                        
                        // provede se typová kontrola nad přiřazením
                        if (strcmp(type, assign_type)) {
                            FREE_VAR_DECL()
                            return SEM_ASSIGN;
                        }
                        
                        free(assign_type);

                    // DEKLARACE
                    } 
                    // else {
                    //     // printf("global var declaration\n");
                    // }

                    // přidání proměnné do tabulky
                    add_var_to_symtable(type, name, value, GLOBAL, symtable);

                    //symtable_print(symtable);
            }

        // VOLÁNÍ funkce
        } else {
            // printf("function call\n");
            _ERR() is_f_set(next->data[1].data, deftable, CALLED)               ERR_()
            // eval_param_list(, )
        }

        // printf("\n");

        node = node->next[1];
    }

    return SEM_OK;
}

int process_stmt_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    while (node->next_count != 1 && node->next_count != 0) {
        TEMP_VARS()

        if (!strcmp(next->data[0].data, "<decl-local>")) {
            // printf("local variable declaration/initialization\n");
            _ERR() is_id_used_locally(next->next[1]->data[1].data, symtable, *deftable)     ERR_()

            // přidání proměnné do rámce
            _ERR() process_decl_local(node, symtable)                               ERR_()


        } else if (!strcmp(next->data[0].data, "<while>")) {
            // printf("while\n");

            _ERR() process_cond(curr->next[0]->next[1], symtable)                                               ERR_()
            _ERR() process_while(curr, symtable, deftable)                          ERR_()
            
        } else if (!strcmp(next->data[0].data, "<if>")) {
            // printf("if\n");
            _ERR() process_cond(curr->next[0]->next[1], symtable)                   ERR_()
            _ERR() process_if(curr, symtable, deftable)                             ERR_()

        } else if (!strcmp(next->data[0].data, "<return>")) {
            // printf("function returns\n");

        } else {
            // printf("assign or function call\n");
            _ERR() process_assign_or_fcall(curr->next[1], symtable, deftable, curr->next[0]->data[1].data) ERR_()
        }

        node = node->next[1];
    }
    
    return SEM_OK;
}

// TODO
int process_while(t_node *node, stack_t *symtable, def_table_t *deftable) {
    (void)node;
    (void)symtable;
    (void)deftable;

    // _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    return SEM_OK;
}

// TODO
int process_if(t_node *node, stack_t *symtable, def_table_t *deftable) {
    (void)node;
    (void)symtable;
    (void)deftable;

    // _ERR() add_scope_to_symtable(symtable)                                      ERR_()

    return SEM_OK;
}

int process_cond(t_node *node, stack_t *symtable) {
    // printf("processing cond\n");
    ALLOC_STR(value1, ""); ALLOC_STR(type1, "");
    ALLOC_STR(value2, ""); ALLOC_STR(type2, "");

    // ověření obou stran porovnání
    _ERR() eval_expr_type(node->next[0]->next[0], &value1, &type1, symtable)                 ERR_()
    _ERR() eval_expr_type(node->next[2]->next[0], &value2, &type2, symtable)                 ERR_()

    // obě strany musí bát stejné
    bool sem_type = strcmp(type1, type2);

    // uvolnění paměti
    free(value1); free(type1);
    free(value2); free(type2);
    
    // printf("cond succes\n");
    
    return (sem_type) ? SEM_TYPE : SEM_OK;
}

// TODO označení chyby
int process_decl_local(t_node *node, stack_t *symtable) {
    TEMP_VARS()

    // atribut a typ proměnné
    ALLOC_STR(name, next->next[1]->data[1].data);
    ALLOC_STR(type, next->next[3]->next[0]->data[1].data);
    // hodnota prozatím není známa
    key_t value = NULL;
    
    // INICIALIZACE
    if (next->next[4]->next_count != 1) {
        // printf("local var initialization\n");

        // vyhodnocení, zda jsou všechny prvky expr stejného typu
        key_t assign_type = NULL;
        
        // vyhodnocení typů přiřazovaných hodnot
        if (eval_expr_type(next->next[4]->next[1]->next[0], &value, &assign_type, symtable)) {
            FREE_VAR_DECL()
            return SEM_TYPE;
        }

        // provede se typová kontrola nad přiřazením
        if (strcmp(type, assign_type)) {
            FREE_VAR_DECL()
            return SEM_ASSIGN;
        }

        free(assign_type);

    } else {
        // printf("local var declaration -> value nil\n");
        ALLOC_CHECK((value = malloc(strlen("nil") + 1)))
        sprintf(value, "%s", "nil");
    }

    // DEKLARACE
    // přidání proměnné do tabulky symbolů
    add_var_to_symtable(type, name, value, LOCAL, symtable);

    return SEM_OK;
}

int process_assign_or_fcall(t_node *node, stack_t *symtable, def_table_t *deftable, key_t first_id) {
    
    // je volána funkce bez potřeby kontrolovat návratové hodnoty
    if (!strcmp(node->next[0]->data[0].data, "(")) {
        fce_item_t *param_list = malloc(sizeof(fce_item_t));
        param_list = NULL;

        return_signal = process_param_list(node->next[1], &param_list, symtable);

        if (return_signal) {
            fce_free(param_list);
            return return_signal;
        }
        
        return_signal = eval_param_list(first_id, param_list, symtable, deftable);

        fce_free(param_list);
        return return_signal;

    // jedná se o přiřazení
    } else {
        int err_code;

        // alokuje se první přiřazované hodnota
        ALLOC_STR(id, first_id)

        // přidání prvního id levé strany přiřazení do seznamu
        fce_item_t *id_list = malloc(sizeof(fce_item_t));
        id_list = NULL;
        fce_item_push(&id_list, id);

        // zpracují se všechny id levého přiřazení
        err_code = process_id_list(node->next[0], &id_list, symtable);

        if (err_code) {
            fce_free(id_list);
            return err_code;
        }

        // zpracování pravé strany přiřazení
        fce_item_t *assign_list = malloc(sizeof(fce_item_t));
        assign_list = NULL;

        err_code = process_f_or_item_list(node->next[2], &assign_list, symtable, deftable);

        if (err_code) {
            fce_free(id_list);
            fce_free(assign_list);
            return err_code;
        }

        // printf("ID LIST:     "); fce_print(id_list, 99);
        // printf("ASSIGN LIST: "); fce_print(assign_list, 99);

        err_code = eval_list_eq(id_list, assign_list, SEM_ASSIGN);

        fce_free(id_list);
        fce_free(assign_list);

        return err_code;
    }

    return SEM_OK;
}

// TODO zpracování idéček
int process_id_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    // printf("%s", "process id-list\n");
    
    if (node->next_count > 1) {
        while (node->next_count > 1) {
            // nově alokované id se přidá do seznamu
            ALLOC_STR(id, node->next[1]->data[1].data)
            fce_item_push(item, id);

            node = node->next[2];
        }
    }

    // fce_item_t *id_list = malloc(sizeof(fce_item_t));
    // id_list = NULL;

    // vyhodnocení, zda jsou všechna id existující názvy proměnných    
    // while((*item) != NULL) {
    //     // printf("brekeke\n");
    //     if (get_var_type(&(*item)->key, symtable))
    //         return SEM_DEFINE;
    //     // printf("-----%s-----", (*item)->key);
    //     (*item) = (*item)->next_f_item;
    // }
    
    fce_item_t *i = *item;
    // vyhodnocení, zda jsou všechna id existující názvy proměnných    
    while((i) != NULL) {
        key_t new_id;
        if ((new_id = get_var_type((i)->key, symtable)) != NULL) {
            ALLOC_STR(type, new_id)
            free((i)->key);
            (i)->key = type;
        } else {
            return SEM_DEFINE;
        }

        (i) = (i)->next_f_item;
    } 


    // printf("%s", "success id-list\n");
    
    return SEM_OK;
}

int process_f_or_item_list(t_node *node, fce_item_t **item, stack_t *symtable, def_table_t *deftable) {
    // printf("%s", "process f-or-item-list\n");
    
    // printf("ROOTNODE: %s %s %d ~~~\n", node->data[0].data, node->data[1].data, node->next_count);
    // printf("NEXT: %s %s %d ~~~\n", node->next[0]->data[0].data, node->next[0]->data[1].data, node->next[0]->next_count);

    if (!strcmp(node->next[0]->data[0].data, "id")) {
        
        // zpracuje volání funkce
        if (!strcmp(node->next[1]->next[0]->data[0].data, "(")) {

            // zpracuje list parametrů
            _ERR() process_param_list(node->next[1]->next[1], item, symtable)       ERR_()
            
            // zkontroluje správnost parametrů funkce
            _ERR() eval_param_list((key_t)node->next[0]->data[1].data, *item, symtable, deftable) ERR_()

        // zpracuje list výrazů nebo id
        } else {
            key_t id;
            if ((id = get_var_type((key_t)node->next[0]->data[1].data, symtable)) != NULL) {
                ALLOC_STR(type, id)
                fce_item_push(item, type);
            } else {
                return SEM_DEFINE;
            }
            
            _ERR() process_item_another(node->next[1]->next[0], item, symtable)     ERR_()
        }
    } else {
            key_t type = NULL;
            key_t value = NULL;

            if (eval_expr_type(node->next[0], &value, &type, symtable)) {
                free(type);
                free(value);
                return SEM_TYPE;
            }

            fce_item_push(item, type);

            free(value);
            
            // pokračuje se dalšími prvky
            _ERR() process_item_another(node->next[1], item, symtable)              ERR_()
    }

    // printf("%s", "success f-or-item-list\n");
    
    return SEM_OK;
}

int process_param_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    // printf("%s", "process param-list\n");
    // parametry funkce jsou prázdné
    if (!strcmp(node->next[0]->data[0].data, "eps"))
        return SEM_OK;
    
    if (!strcmp(node->next[0]->next[0]->next[0]->data[0].data, "expr")) {
        key_t value = NULL, type = NULL;
        if ((return_signal = eval_expr_type(node->next[0]->next[0]->next[0], &value, &type, symtable))) {
            free(value);
            free(type);
            return return_signal;
        }
        free(value);
        fce_item_push(item, type);
        
    } else {
        key_t id;
        if ((id = get_var_type((key_t)node->next[0]->next[0]->next[0]->data[1].data, symtable)) != NULL) {
            ALLOC_STR(type, id)
            fce_item_push(item, type);
        } else {
            return SEM_DEFINE;
        }
    }
    
    // pokračuje se na item another
    _ERR() process_item_another(node->next[0]->next[1], item, symtable)        ERR_()

    // printf("%s", "success param-list\n");
    
    return SEM_OK;
}

int process_item_another(t_node *node, fce_item_t **item, stack_t *symtable) {
    // printf("%s", "process item-another\n");
    while (node->next_count > 1) {
        // id se přidá do seznamu
        if (!strcmp(node->next[1]->next[0]->data[0].data, "id")) {

            key_t id;
            if ((id = get_var_type((key_t)node->next[1]->next[0]->data[1].data, symtable)) != NULL) {
                ALLOC_STR(type, id)
                fce_item_push(item, type);
            } else {
                return SEM_DEFINE;
            }

        // vyhodnotí se výraz
        } else {
            key_t type = NULL;
            key_t value = NULL;

            if ((return_signal = eval_expr_type(node->next[1]->next[0], &value, &type, symtable)) != 0) {
                free(type);
                free(value);
                
                return return_signal;
            }

            fce_item_push(item, type);
            free(value);
        }

        node = node->next[2];
    }
    
    // printf("%s", "success item-another\n");
    
    return SEM_OK;
}

int process_types(t_node *node, fce_item_t **item) {
    // printf("%s", "process return-types\n");
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
    // printf("%s", "success return-types\n");
    
    return SEM_OK;
}

int process_return_types(t_node *node, fce_item_t **item, int *return_values) {
    // printf("%s", "process return-types\n");

    if (node->next_count != 1) {
        ALLOC_STR(ret_key_first, node->next[1]->next[0]->data[1].data);
        fce_item_push(item, ret_key_first);

        node = node->next[2];
    
        // projde type-list dokud se nenarazí na eps
        while (node->next_count != 1) {
            ALLOC_STR(ret_key_next, node->next[1]->next[0]->data[1].data);
            fce_item_push(item, ret_key_next);

            node = node->next[2];

            (*return_values)++;
        }

        (*return_values)++;
    }

    // printf("%s", "succes return-types\n");
    return SEM_OK;
}

int process_f_arg_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    // printf("%s", "process f-arg-list\n");
    
    // nejprve určí první typ
    if (node->next_count != 1) {
        // f-arg
        ALLOC_STR(type_first, node->next[0]->next[2]->next[0]->data[1].data);
        ALLOC_STR(key_first, node->next[0]->next[0]->data[1].data);
        ALLOC_STR(value_first, "");

        // přidání typu do lokální tabulky proměnných
        ALLOC_CHECK(htab_lookup_add(symtable->stack[symtable->top], type_first, key_first, value_first, LOCAL, -1))

        // přidání typu do listu parametrů funkce
        ALLOC_STR(arg_type_first, node->next[0]->next[2]->next[0]->data[1].data);
        fce_item_push(item, arg_type_first);

        // posun na další f-arg-another
        node = node->next[1];
        
        // f-arg-another
        while (node->next_count != 1) {
            // f-arg
            ALLOC_STR(type, node->next[1]->next[2]->next[0]->data[1].data);
            ALLOC_STR(key, node->next[1]->next[0]->data[1].data);
            ALLOC_STR(value, "");

            // přidání typu do lokální tabulky proměnných
            ALLOC_CHECK(htab_lookup_add(symtable->stack[symtable->top], type, key, value, LOCAL, -1))

            // přidání typu do listu parametrů funkce
            ALLOC_STR(arg_type, node->next[1]->next[2]->next[0]->data[1].data);
            fce_item_push(item, arg_type);

            // posun na další f-arg-another
            node = node->next[2];
        }
    }
    
    // printf("%s", "success f-arg-list\n");
    return SEM_OK;
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
    process_return_types(node->next[3]->next[4], &fce_i, &htab_i->ret_values);

    // typy paramentrů
    process_types(node->next[3]->next[2], &fce_i);

    // přiřazení ukazatele
    htab_i->fce = fce_i;

    return SEM_OK;
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
    // printf("\n+ + + + + + + + + + + + + + + + + + + + +\nZÁSOBNÍK PŘED UKONČENÍM FUNKCE %s \n", node->next[1]->data[1].data);
    //symtable_print(symtable);
    symtable_pop(symtable);
    
    return SEM_OK;
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

    return SEM_OK;
}

int add_scope_to_symtable(stack_t *symtable) {
    // inicializace nové hashovací tabulky
    htab_t *htab = htab_init(HASH_TABLE_DIMENSION);
    
    ALLOC_CHECK(htab)
        
    // přidání do zásobníku rámců
    symtable_push(&symtable, htab);

    return SEM_OK;
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
        // printf("\n~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\nGLOBÁLNÍ ZÁSOBNÍK\n");
        //symtable_print(symtable);

        // printf("\nERRCODE: %d\n", return_signal);

        // kontrola, zda volané funkce byly řádně definovány
        if (!return_signal)
            return_signal = eval_fcall(*deftable);

        // printf("\nERRCODE: %d\n", return_signal);

        // * PRINT
        //def_table_print(*deftable);

        // * TESTOVÁNÍ
        symtable_free(symtable);
        def_table_free(deftable);
    }

    return return_signal;
}
