/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	06. 12. 2021 17:21:10
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Sémantické kontroly nad syntaktickým stromem generovaným syntaktickou analýzou
 */

#include "semantic.h"

// návratový kód chyby během chodu programu
int return_signal = SEM_OK;
// číslo řádku chyby
int err_on_line = 0;


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
        if (!strcmp(deftable.item[i].name, id))
            return SEM_DEFINE;

    return SEM_OK;
}

int is_var_set(key_t name, stack_t *symtable) {
    // hledání v tabulce symbolů
    htab_item_t *item;
    for (top_t i = symtable->top; i >= 0; i--)
        if ((item = htab_find(symtable->stack[i], name)) != NULL)
            // musí jít proměnnou
            return (item->ret_values == -1) ? SEM_OK : SEM_DEFINE;

    return SEM_DEFINE;
}

int is_f_set(key_t name, def_table_t *deftable, int state) {
    switch (state) {
        // definice nové funkce
        case DEFINED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (!strcmp(deftable->item[i].name, name)) {
                    if (deftable->item[i].state == DEFINED) {
                        fprintf(stderr, "ERROR: Redefinition of function \"%s\"\n", name);
                        return SEM_DEFINE;
                    }
                }
            }
            break;
        
        // deklarace nové funkce
        case DECLARED:
            for (size_t i = 0; i < deftable->size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (!strcmp(deftable->item[i].name, name)) {
                    (deftable->item[i].state == DECLARED) ?
                        fprintf(stderr, "ERROR: Redeclaration of function \"%s\"\n", name) :
                        fprintf(stderr, "ERROR: Defined function \"%s\" can't be redeclared\n", name);
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // nová funkce je volána
        case CALLED:
            for (size_t i = 0; i < deftable->size; i++) {
                if (!strcmp(deftable->item[i].name, name)) {
                    // funkce musí být před zavoláním definována
                    if (deftable->item[i].state == DECLARED) {
                        return SEM_OK;
                    } else {
                        // přidá příznak do tabulky funkcí, o volání funkce
                        deftable->item[i].called = 1;
                        return SEM_OK;
                    }
                }
            }

        // funkce nebyla nalezena => chyba
        fprintf(stderr, "ERROR: Call of undeclared function \"%s\"\n", name);
        return SEM_DEFINE;
    }
    
    return SEM_OK;
}

int eval_fcall(def_table_t deftable) {
    // hledání v tabulce funkcí
    for (size_t i = 0; i < deftable.size; i++) {
        // chyba pokud nebyla funkce definována
        if (!deftable.item[i].state) {
            fprintf(stderr, "ERROR: Call of undefined function\n");
            return SEM_DEFINE;
        }
    }

    return SEM_OK;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Typová nekompatibilita výrazů - exit(6) - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int eval_expr_type(t_node *node, key_t *value, key_t *type, stack_t *symtable) {
    // unární oprátor
    if (node->next_count == 2) {
        t_node *unary_node = (strcmp(node->next[0]->data[1].data, "#")) ? node->next[0] : node->next[1];
        
        if (!strcmp(unary_node->data[0].data, "string")) {
            free(*value);
            free(*type);

            // alokuje se délka potřebná pro jeho hodnotu
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
        
        // pravý operand
        if ((sem_type = eval_expr_type(node->next[2], value, type, symtable))) {
            free(*value); free(*type);
            free(type1);  free(val1);
            return sem_type;
        }

        ALLOC_STR(type2, *type)
        ALLOC_STR(val2, *value)
        
        sem_type = strcmp(type1, type2);

        // konverze z int na number
        if (NUM_OR_INT(type1, type2) || !strcmp(node->next[1]->data[1].data, "/")) {
            *type = realloc(*type, strlen("number") + 1);
            sprintf(*type, "%s", "number");

            // provedení konverze bylo v pořádku
            sem_type = SEM_OK;
        }
        
        // uvolnění zdrojů při chybě
        free(type1); free(type2);
        free(val1);  free(val2);

        return sem_type;
        
    } else if (node->next_count == 1) {
        return eval_expr_type(node->next[0], value, type, symtable);

    // nejníže položený prvek ve stromě
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

                // jedná se o proměnnou
                if (item->ret_values == -1) {
                    ALLOC_STR(t, item->type)
                    ALLOC_STR(v, item->value)  
                    
                    UPDATE_EXPR(SEM_OK) 
                    
                // jedná se o funkci
                } else if (item->ret_values > 0) {
                    fce_item_t *return_list = NULL;
                    get_f_return_list(item->key, &return_list, symtable);
                    
                    ALLOC_STR(t, return_list->key)
                    ALLOC_STR(v, item->value)   
                    
                    fce_free(return_list);
                    
                    UPDATE_EXPR(SEM_OK)
                } else {
                    fprintf(stderr, "ERROR: Called function \"%s\" does not return values\n", v);
                    UPDATE_EXPR(SEM_ASSIGN)
                }

            // proměnná nebyla nalezena -> chyba
            } else {
                fprintf(stderr, "ERROR: Identifier \"%s\" was not found\n", v);
                UPDATE_EXPR(SEM_DEFINE)
            }
        }

        UPDATE_EXPR(SEM_OK)
    }

    return SEM_OK;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Operace se seznamy hodnot, typů exit(4,5,6) - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

int eval_list_eq(fce_item_t *dest, fce_item_t *src, int err_code) {
    // pokud je jiný počet hodnot listů nastane chyba
    if ((dest == NULL && src != NULL) || (dest != NULL && src == NULL)) {
        fprintf(stderr, "ERROR: Invalid number of parameters/arguments\n");    
        return err_code;
    }

    while (dest != NULL && src != NULL) {
        // porovnání typové kompatibility
        if (strcmp(dest->key, src->key) && !(!strcmp(dest->key, "number") && !strcmp(src->key, "integer"))) {
            fprintf(stderr, "ERROR: Invalid types, given \"%s\", expected \"%s\"\n", src->key, dest->key);
            return err_code;
        }

        dest = dest->next_f_item;
        src = src->next_f_item;
    }

    // pokud je jiný počet hodnot listů nastane chyba
    if ((dest == NULL && src != NULL) || (dest != NULL && src == NULL)) {
        fprintf(stderr, "ERROR: Invalid number of parameters/arguments within function\n");    
        return err_code;
    }

    return SEM_OK;
}

int eval_param_list(key_t name, fce_item_t *dest, stack_t *symtable, def_table_t *deftable) {
    // u funkce write není třeba kontrolovat počty a typy parametrů
    if (!strcmp(name, "write"))
        return SEM_OK;

    // ověří se, zda je volaná funkce definovaná
    if ((is_f_set(name, deftable, 3) != 0)) 
        return return_signal;
    
    // nalezení identifikátoru funkce
    htab_item_t *item = symtable_lookup_item(symtable, name);
    if (item == NULL) {
        fprintf(stderr, "ERROR: Identifier of function  \"%s\" has not been found\n", name);
        return SEM_DEFINE;
    }
        
    fce_item_t *params = item->fce;
    int counter = item->ret_values;

    // posune se ukazatel na začátek listu parametrů
    while (counter != 0 && params != NULL) {
        params = params->next_f_item;
        counter--;
    }

    // zkontroluje se správnost parametrů
    return eval_list_eq(params, dest, SEM_FCE);
}

int get_f_return_list(key_t name, fce_item_t **dest, stack_t *symtable) {
    // nalezne položku v tabulce
    htab_item_t *item = symtable_lookup_item(symtable, name);
    if (item == NULL)
        return SEM_DEFINE;
    
    fce_item_t *params = item->fce;
    int counter = 0;

    if (*dest != NULL)
        fce_free(*dest);
        
    *dest = malloc(sizeof(fce_item_t));
    *dest = NULL;
    
    // vytvoří nový list návratových typů
    while (counter != item->ret_values && params != NULL) {
        ALLOC_STR(id, params->key)
        fce_item_push(dest, id);
        params = params->next_f_item;
        counter++;
    }
    
    return SEM_OK;
}

int eval_return_eq(fce_item_t *dest, fce_item_t *src) {
    // chyba, pokud je větší počet návratových hodnot
    if (dest == NULL && src != NULL) {
        fprintf(stderr, "ERROR: Invalid number of return values\n");  
        return SEM_FCE;
    }

    while (dest != NULL && src != NULL) {
        // porovnání typové kompatibility
        if (strcmp(dest->key, src->key) && !(!strcmp(dest->key, "number") && !strcmp(src->key, "integer"))) {
            fprintf(stderr, "ERROR: Invalid return type \"%s\", expecting \"%s\"\n", src->key, dest->key);
            return SEM_FCE;
        }

        dest = dest->next_f_item;
        src = src->next_f_item;
    }

    // chyba, pokud je větší počet návratových hodnot
    if (dest == NULL && src != NULL) {
        fprintf(stderr, "ERROR: Invalid number of return values\n");    
        return SEM_FCE;
    }

    return SEM_OK;
}

int append_list(fce_item_t **dest, fce_item_t *src) {
    fce_item_t *tmpitem = src;
    
    while (tmpitem != NULL) {
        ALLOC_STR(key, tmpitem->key)
        fce_item_push(dest, key);
        tmpitem = tmpitem->next_f_item;
    }

    fce_free(src);

    return SEM_OK;
}


// - - - - - - - - - - - - - - - - - - - //
// - - - -  Zpracování gramatiky - - - - //
// - - - - - - - - - - - - - - - - - - - //

int process_main_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    // vytvoření globálního rámce
    _ERR() add_scope_to_symtable(symtable)                                                      ERR_(node)
    
    // vygenerování vestavěných funkcí do globální tabulky symbolů
    GENERATE_BUILTIN_FUNCTIONS()

    while (node->next_count > 1) {
        TEMP_VARS()

        // DEFINICE funkce
        if (strcmp(next->data[1].data, "function") == 0) {
            // zkontroluje zda nejde o redefinici funkce                
            _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DEFINED)                     ERR_(node)

            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            if (!is_id_used(curr->next[1]->data[1].data, symtable, *deftable))
                def_table_add(curr->next[1]->data[1].data, deftable, DEFINED);
                
            _ERR() f_define(curr, symtable, deftable)                                           ERR_(node)
            
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DEFINED)                ERR_(node)

        // DEKLARACE funkce nebo proměnné
        } else if (strcmp(next->data[1].data, "global") == 0) {

            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr->next[1]->data[1].data, symtable, *deftable)                 ERR_(node)

            // určení, že jde o funkci
            if (curr->next[3]->next_count == 5) {
                _ERR() is_f_set(curr->next[1]->data[1].data, deftable, DECLARED)                ERR_(node)
                // přidání id funkce do tabulky funkcí
                _ERR() def_table_add(curr->next[1]->data[1].data, deftable, DECLARED)           ERR_(node)
                
                _ERR() f_declare(curr, symtable)                                                ERR_(node)
                
            // jde o proměnnou
            } else {
                    // alokace proměnných
                    ALLOC_STR(name, curr->next[1]->data[1].data);
                    ALLOC_STR(type, curr->next[3]->next[0]->next[0]->data[1].data);
                    // hodnota prozatím není známa
                    key_t value = NULL;

                    // INICIALIZACE
                    if (curr->next[3]->next[1]->next_count == 2) {
                        key_t assign_type = NULL;
                        
                        // vyhodnocení typů přiřazovaných hodnot
                        if ((return_signal = eval_expr_type(curr->next[3]->next[1]->next[1], &value, &assign_type, symtable))) {
                            fprintf(stderr, "ERROR: Type incompatibility in expression between \"%s\" and \"%s\"\n", assign_type, type);
                            FREE_VAR_DECL()
                            return return_signal;
                        }
                        
                        // provede se typová kontrola nad přiřazením
                        if (strcmp(type, assign_type)) {
                            FREE_VAR_DECL()
                            return SEM_ASSIGN;
                        }
                        
                        free(assign_type);
                    }

                    // přidání proměnné do tabulky
                    add_var_to_symtable(type, name, value, GLOBAL, symtable);
            }

        // VOLÁNÍ funkce
        } else {
            _ERR() is_f_set(next->data[1].data, deftable, CALLED)                               ERR_(node)
            fce_item_t *param_list = malloc(sizeof(fce_item_t));
            param_list = NULL;

            return_signal = process_param_list(curr->next[2], &param_list, symtable);

            if (return_signal) {
                fce_free(param_list);
                return return_signal;
            }
            
            if ((return_signal = eval_param_list(next->data[1].data, param_list, symtable, deftable)) != 0)
                search_line(node);

            fce_free(param_list);
            return return_signal;
        }

        node = node->next[1];
    }

    return SEM_OK;
}

int process_stmt_list(t_node *node, stack_t *symtable, def_table_t *deftable) {

    while (node->next_count != 1 && node->next_count != 0) {
        TEMP_VARS()
        
        if (!strcmp(next->data[0].data, "<decl-local>")) {
            _ERR() is_id_used_locally(next->next[1]->data[1].data, symtable, *deftable)         ERR_(node)

            // přidání proměnné do rámce
            _ERR() process_decl_local(node, symtable)                                           ERR_(node)

        } else if (!strcmp(next->data[0].data, "<while>")) {
            _ERR() process_cond(curr->next[0]->next[1], symtable)                               ERR_(node)
            _ERR() process_while(curr->next[0], symtable, deftable)                             ERR_(node)
            
        } else if (!strcmp(next->data[0].data, "<if>")) {
            _ERR() process_cond(curr->next[0]->next[1], symtable)                               ERR_(node)
            _ERR() process_if(curr->next[0], symtable, deftable)                                ERR_(node)

        } else if (!strcmp(next->data[0].data, "<return>")) {
            key_t name = (key_t)node->prev->next[1]->data[1].data;
            _ERR() process_return_list(curr->next[0]->next[1], symtable, deftable, name)        ERR_(node)

        } else {
            _ERR() process_assign_or_fcall(curr->next[1], symtable, deftable, curr->next[0]->data[1].data) ERR_(node)
        }

        node = node->next[1];
    }
    
    return SEM_OK;
}

int process_while(t_node *node, stack_t *symtable, def_table_t *deftable) {
    _ERR() add_scope_to_symtable(symtable)                                                      ERR_(node)
    _ERR() process_stmt_list(node->next[3], symtable, deftable)                                 ERR_(node)
    symtable_pop(symtable);

    return SEM_OK;
}

int process_if(t_node *node, stack_t *symtable, def_table_t *deftable) {
    _ERR() add_scope_to_symtable(symtable)                                                      ERR_(node)
    _ERR() process_stmt_list(node->next[3], symtable, deftable)                                 ERR_(node)
    symtable_pop(symtable);

    _ERR() add_scope_to_symtable(symtable)                                                      ERR_(node)
    _ERR() process_stmt_list(node->next[5], symtable, deftable)                                 ERR_(node)
    symtable_pop(symtable);

    return SEM_OK;
}

int process_cond(t_node *node, stack_t *symtable) {
    ALLOC_STR(value1, ""); ALLOC_STR(type1, "");
    ALLOC_STR(value2, ""); ALLOC_STR(type2, "");

    // ověření obou stran porovnání
    _ERR() eval_expr_type(node->next[0]->next[0], &value1, &type1, symtable)                 ERR_(node)
    _ERR() eval_expr_type(node->next[2]->next[0], &value2, &type2, symtable)                 ERR_(node)

    // obě strany musí bát stejné
    bool sem_type = strcmp(type1, type2);

    // porovnání integer a number
    if (NUM_OR_INT(type1, type2)) 
        sem_type = SEM_OK;

    // porovnaní s typem nil
    if (CMP_NIL(type1, type2, node->next[1]->next[0]->data[0].data))
        sem_type = SEM_OK;

    // uvolnění paměti
    free(value1); free(type1);
    free(value2); free(type2);
    
    if (sem_type) {
        fprintf(stderr, "ERROR: Type incompatibility in comparison\n");
        return SEM_TYPE;
    } else {
        return SEM_OK;
    }
}

int process_f_or_return_list(t_node *node, fce_item_t **item, stack_t *symtable, def_table_t *deftable, key_t f_name) {
    if (node->next_count > 1) {
        // zpracuje se param list
        fce_item_t *param_list = malloc(sizeof(fce_item_t));
        param_list = NULL;

        // porovnání argumentů s parametry funkce
        _ERR() eval_param_list(f_name, param_list, symtable, deftable)                          ERR_(node)

        // list návratových hodnot volané funkce se připojí k již existujícímu
        fce_item_t *return_list = NULL;
        _ERR() get_f_return_list(f_name, &return_list, symtable)                                ERR_(node)
        append_list(item, return_list);
    
        // pokračuje se dalšími prvky seznamu
        _ERR() process_return_f_or_items__(node->next[3], item, symtable, deftable)             ERR_(node) 

        return SEM_OK;
    }

    ALLOC_STR(type, symtable_lookup_item(symtable, f_name)->type)
    fce_item_push(item, type);
    
    _ERR() process_return_f_or_items__(node->next[0], item, symtable, deftable)                 ERR_(node)
    
    return SEM_OK;
}

int process_return_f_or_items(t_node *node, fce_item_t **item, stack_t *symtable, def_table_t *deftable) {
    // provede se vyhodnocení výrazu je výrazu
    if (!strcmp(node->next[0]->data[0].data, "expr")) {
        ALLOC_STR(value, "");
        ALLOC_STR(type, "");
        eval_expr_type(node->next[0], &value, &type, symtable);
        fce_item_push(item, type);
        free(value);
        
        _ERR() process_return_f_or_items__(node->next[1], item, symtable, deftable)             ERR_(node)
    } else {
        _ERR() process_f_or_return_list(node->next[1], item, symtable, deftable, node->next[0]->data[1].data) ERR_(node)
    }

    return SEM_OK;
}

int process_return_f_or_items__(t_node *node, fce_item_t **item, stack_t *symtable, def_table_t *deftable) {
    // list není prázdný, pokkračuje se v dalších prvcích
    if (node->next_count > 1) 
        _ERR() process_return_f_or_items(node->next[1], item, symtable, deftable)               ERR_(node)
    
    return SEM_OK;
}

int process_return_list(t_node *node, stack_t *symtable, def_table_t *deftable, key_t name) {
    fce_item_t *return_list = malloc(sizeof(fce_item_t));
    return_list = NULL;

    // List návratových hodnot není prázdný
    if (strcmp(node->next[0]->data[0].data, "eps")) {
        process_return_f_or_items(node->next[0], &return_list, symtable, deftable);
    }

    fce_item_t *f_return_list = NULL;
    if ((return_signal = get_f_return_list(name, &f_return_list, symtable))) {
        fce_free(return_list);
        return return_signal;
    }
    
    return_signal = eval_return_eq(f_return_list, return_list);

    fce_free(return_list);
    fce_free(f_return_list);

    return return_signal;
}

int process_decl_local(t_node *node, stack_t *symtable) {
    TEMP_VARS()

    // atribut a typ proměnné
    ALLOC_STR(name, next->next[1]->data[1].data);
    ALLOC_STR(type, next->next[3]->next[0]->data[1].data);
    // hodnota prozatím není známa
    key_t value = NULL;
    
    // INICIALIZACE
    if (next->next[4]->next_count != 1) {
        // vyhodnocení, zda jsou všechny prvky expr stejného typu
        key_t assign_type = NULL;
        
        // vyhodnocení typů přiřazovaných hodnot
        if ((return_signal = eval_expr_type(next->next[4]->next[1]->next[0], &value, &assign_type, symtable))) {
            FREE_VAR_DECL()
            return return_signal;
        }
        
        // provede se typová kontrola nad přiřazením
        if (strcmp(type, assign_type) && !(!strcmp(type, "number") && !strcmp(assign_type, "integer")) && strcmp(assign_type, "nil")) {
            fprintf(stderr, "ERROR: Type incompatibility during assignment given \"%s\" expected \"%s\"\n", assign_type, type);
            FREE_VAR_DECL()
            return SEM_ASSIGN; 
        }

        free(assign_type);

    } else {
        // iniciální hodnota je nil
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

        err_code = eval_list_eq(id_list, assign_list, SEM_ASSIGN);

        fce_free(id_list);
        fce_free(assign_list);

        return err_code;
    }

    return SEM_OK;
}

int process_id_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    // terace přes id-list
    if (node->next_count > 1) {
        while (node->next_count > 1) {
            // nově alokované id se přidá do seznamu
            ALLOC_STR(id, node->next[1]->data[1].data)
            fce_item_push(item, id);

            node = node->next[2];
        }
    }
    
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
    
    return SEM_OK;
}

int process_f_or_item_list(t_node *node, fce_item_t **item, stack_t *symtable, def_table_t *deftable) {
    if (!strcmp(node->next[0]->data[0].data, "id")) {
        
        // zpracuje volání funkce
        if (!strcmp(node->next[1]->next[0]->data[0].data, "(")) {

            // zpracuje list parametrů
            _ERR() process_param_list(node->next[1]->next[1], item, symtable)                   ERR_(node)
            
            // zkontroluje správnost parametrů funkce
            _ERR() eval_param_list((key_t)node->next[0]->data[1].data, *item, symtable, deftable) ERR_(node)
            
            _ERR() get_f_return_list((key_t)node->next[0]->data[1].data, item, symtable)        ERR_(node)

        // zpracuje list výrazů nebo id
        } else {
            key_t id;
            if ((id = get_var_type((key_t)node->next[0]->data[1].data, symtable)) != NULL) {
                ALLOC_STR(type, id)
                fce_item_push(item, type);
            } else {
                return SEM_DEFINE;
            }
            
            _ERR() process_item_another(node->next[1]->next[0], item, symtable)                 ERR_(node)
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
            _ERR() process_item_another(node->next[1], item, symtable)                          ERR_(node)
    }

    return SEM_OK;
}

int process_param_list(t_node *node, fce_item_t **item, stack_t *symtable) {
    // parametry funkce jsou prázdné
    if (!strcmp(node->next[0]->data[0].data, "eps"))
        return SEM_OK;
    
    // jedná se o expression
    if (!strcmp(node->next[0]->next[0]->next[0]->data[0].data, "expr")) {
        key_t value = NULL, type = NULL;
        if ((return_signal = eval_expr_type(node->next[0]->next[0]->next[0], &value, &type, symtable))) {
            free(value);
            free(type);
            return return_signal;
        }
        free(value);
        fce_item_push(item, type);
        
    // jedná se o proměnnouw
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
    _ERR() process_item_another(node->next[0]->next[1], item, symtable)                         ERR_(node)

    return SEM_OK;
}

int process_item_another(t_node *node, fce_item_t **item, stack_t *symtable) {
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
    
    return SEM_OK;
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
    
    return SEM_OK;
}

int process_return_types(t_node *node, fce_item_t **item, int *return_values) {
    // iteruje dokud nejsou následnící
    if (node->next_count != 1) {
        ALLOC_STR(ret_key_first, node->next[1]->next[0]->data[1].data);
        fce_item_push(item, ret_key_first);

        node = node->next[2];
    
        // projde type-list dokud se nenarazí na eps
        while (node->next_count != 1) {
            ALLOC_STR(ret_key_next, node->next[1]->next[0]->data[1].data);
            fce_item_push(item, ret_key_next);

            node = node->next[2];
            
            // zvyšuje počet návratových hodnot
            (*return_values)++;
        }

        (*return_values)++;
    }

    return SEM_OK;
}

int process_f_arg_list(t_node *node, fce_item_t **item, stack_t *symtable) {    
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
    
    return SEM_OK;
}



// - - - - - - - - - - - - - - - - - - - //
// - - - - -  Práce s funkcemi - - - - - //
// - - - - - - - - - - - - - - - - - - - //

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
    _ERR() process_return_types(node->next[3]->next[4], &fce_i, &htab_i->ret_values)            ERR_(node)

    // typy paramentrů
    _ERR() process_types(node->next[3]->next[2], &fce_i)                                        ERR_(node)

    // přiřazení ukazatele
    htab_i->fce = fce_i;

    return SEM_OK;
}

int f_define(t_node *node, stack_t *symtable, def_table_t *deftable) {
    // přidání lokálního rámce funkce
    _ERR() add_scope_to_symtable(symtable)    ERR_(node)      

    for (size_t i = 0; i < deftable->size; i++) {
        if (!strcmp(deftable->item[i].name, node->next[1]->data[1].data)) {
            // funkce už byla deklarována => provede se porovnání parametrů
            if (deftable->item[i].state == DECLARED) {
                fce_item_t *cmp_list = malloc(sizeof(fce_item_t));
                fce_item_t *ret_list = malloc(sizeof(fce_item_t));
                cmp_list = ret_list = NULL;
                int *empty_return_type = malloc(sizeof(int));
                
                // typy paramentrů
                if ((return_signal = process_f_arg_list(node->next[3], &cmp_list, symtable))) {
                    FREE_F_DEF()
                    return return_signal;
                }

                // porovnání parametrů
                if ((return_signal = eval_param_list(node->next[1]->data[1].data, cmp_list, symtable, deftable))) {
                    FREE_F_DEF()
                    return return_signal;
                }

                // zpracování návratových hodnot
                if ((return_signal = process_return_types(node->next[5], &ret_list, empty_return_type))) {
                    FREE_F_DEF()
                    return return_signal;
                }

                // získání návratových hodnot
                if ((return_signal = get_f_return_list(node->next[1]->data[1].data, &cmp_list, symtable))) {
                    FREE_F_DEF()
                    return return_signal;
                }
                
                // TODO ověřit, jestli zde nemá být spíš funkce eval_list_
                if ((return_signal = eval_list_eq(ret_list, cmp_list, 5))) {
                    FREE_F_DEF()
                    return return_signal;
                }
                
                FREE_F_DEF()
            
            // funkce ještě nebyla deklarována
            } else {
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
            }
        }
    }
    
    _ERR() process_stmt_list(node->next[6], symtable, deftable)                                 ERR_(node)

    // symtable_print(symtable);

    // zrušní rámce po opuštění funkce
    symtable_pop(symtable);
    
    return SEM_OK;
}



// - - - - - - - - - - - - - - - - - - - - - - - //
// - - - -  Operace nad tabulkou symbolů - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - //

int add_var_to_symtable(key_t type, key_t key, key_t value, bool local, stack_t *symtable) {
    // kontrola, zda nejde o redeklaraci lokální proměnné v jednom rámci
    if (local) {
        htab_item_t *h_item = htab_find(symtable->stack[symtable->top], key);
        
    if (h_item != NULL) {
        fprintf(stderr, "ERROR: Redeclaration of variable \"%s\"\n", key);
        return SEM_DEFINE;
    }

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

key_t get_var_type(key_t name, stack_t *symtable) {
    // hledání v tabulce symbolů
    htab_item_t *item;
    for (top_t i = symtable->top; i >= 0; i--)
        if ((item = htab_find(symtable->stack[i], name)) != NULL) 
            return (item->ret_values == -1) ? item->type : NULL;

    return NULL;
}



// - - - - - - - - - - - - - - - - //
// - - - -   Hlavní funkce - - - - //
// - - - - - - - - - - - - - - - - //

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

        // kontrola, zda volané funkce byly řádně definovány
        if (!return_signal)
            return_signal = eval_fcall(*deftable);

        symtable_free(symtable);
        def_table_free(deftable);
    }

    return return_signal;
}

int search_line(t_node *node) {
    if (node->line > 0)
        return node->line;

    for (int i = 0; i < node->next_count; i++)
        if (node->next[i]->line > 0) {
            fprintf(stderr, "ERROR: near line %d\n", node->next[i]->line);
            return node->next[i]->line;
        } else {
            return search_line(node->next[i]);
        }
    
    return -1;
}
