/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	19. 11. 2021 23:27:09
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
    MAKE_TREE()
    // MAKE_SMALL_TREE()
    // MAKE_DEF_TABLE()
    // MAKE_SYMTABLE()
    
    printf("\n\n");


    semantic(&node4);

    
    // * TESTOVÁNÍ
    node_delete(&node1);
    
    
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

int is_id_used(char *id, stack_t *symtable, def_table_t deftable) {
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

int is_f_set(char *name, def_table_t deftable, int state) {
    switch (state) {
        // definice nové funkce
        case DEFINED:
            for (size_t i = 0; i < deftable.size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable.item[i].name, name) == 0) {
                    (deftable.item[i].data == DEFINED) ?
                        printf("redefinice funkce\n") :
                        printf("deklarovanou funkci nelze znovu definovat\n");
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // deklarace nové funkce
        case DECLARED:
            for (size_t i = 0; i < deftable.size; i++) {
                // pokud je funkce již v tabulce, tak došlo k chybě
                if (strcmp(deftable.item[i].name, name) == 0) {
                    (deftable.item[i].data == DECLARED) ?
                        printf("redeklarace funkce\n") :
                        printf("definovanou funkci nelze znovu deklarovat\n");
                    
                    return SEM_DEFINE;
                }
            }
            break;
        
        // nová funkce je volána
        case CALLED:
            for (size_t i = 0; i < deftable.size; i++) {
                if (strcmp(deftable.item[i].name, name) == 0) {
                    // funkce musí být před zavoláním definována
                    if (deftable.item[i].data == DECLARED) {
                        printf("funkce nebyla definována\n");
                        return SEM_DEFINE;
                    } else {
                        printf("call function %s\n", name);
                        return EXIT_SUCCESS;
                    }
                }
            }

        // funkce nebyla nalezena => chyba
        printf("volání nedefinované funkce\n");
        return SEM_DEFINE;
    }
    
    return EXIT_SUCCESS;
}


int process_main_list(t_node *node, stack_t *symtable, def_table_t deftable) {
    while ((*node).next_count != 1) {
        TEMP_VARS()
        
        // DEFINICE funkce
        if (strcmp(next.data[1].data, "function") == 0) {
            printf("function definition\n");
            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr.next[1].data[1].data, symtable, deftable)    ERR_()
            _ERR() is_f_set(curr.next[1].data[1].data, deftable, DEFINED)       ERR_()
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr.next[1].data[1].data, &deftable, DEFINED) ERR_()

            // přidání pouze definice funkce do globálního rámce tabulky symbolů
            // f_define();

        // DEKLARACE funkce
        } else if (strcmp(next.data[1].data, "global") == 0) {
            printf("function declaration\n");
            // sémantická kontrola stejného jména proměnné nebo funkce a její redeklarace
            _ERR() is_id_used(curr.next[1].data[1].data, symtable, deftable)    ERR_()
            _ERR() is_f_set(curr.next[1].data[1].data, deftable, DECLARED)      ERR_()
            // přidání id funkce do tabulky funkcí
            _ERR() def_table_add(curr.next[1].data[1].data, &deftable, DEFINED) ERR_()

            // přidání funkce do globálního rámce tabulky symbolů + sémantická kontrola nitra
            // f_declare();

        // VOLÁNÍ funkce
        } else {
            printf("function call\n");
            is_f_set(next.data[1].data, deftable, CALLED);

            // 
        }

        printf("\n");

        node = &(node->next[1]);
    }

    return EXIT_SUCCESS;
}

int process_stmt_list(t_node *node, stack_t *symtable, def_table_t deftable) {

    while ((*node).next_count != 1) {
        TEMP_VARS()

        if (strcmp(next.data[1].data, "<decl-local>") == 0) {
            printf("local variable declaration\n");
            _ERR() is_id_used(next.next[1].data[1].data, symtable, deftable)    ERR_()

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

int process_decl_local(t_node *node, stack_t *symtable) {
    
    if (node->next[4].next_count != 1) {
        char *var_type = node->next[3].next[0].data[0].data;
        // vyhodnocení, zda jsou všechny prvky expr stejného typu
        _ERR() eval_expr_type(node->next[4].next[1].next[0])                    ERR_()
        char *assign_type = node->next[4].next[1].next[0].data[0].data;

        // provede se typová kontrola nad přiřazením
        TYPE_CHECK(var_type, assign_type)

        // přidání proměnné do tabulky symbolů
        add_var_to_symtable(node->next[2].data[0].data, symtable, DECLARED);

        return EXIT_SUCCESS;
    }

    
    // přidání proměnné do tabulky symbolů
    add_var_to_symtable(node->next[2].data[0].data, symtable, DEFINED);

    return EXIT_SUCCESS;
}

int process_def_decl_fcall() {

    return EXIT_SUCCESS;
}

int eval_expr_type(t_node node) {
    // TODO 
    // rekurzivně (nejspíš) projít strom a vyhodit, zda všechny typy prvků souhlasí
}



// TODO
int add_var_to_symtable(t_node *node, stack_t *symtable, bool state) {
    (void)node;
    (void)symtable;    

    return EXIT_SUCCESS;
}
// TODO
int add_scope_to_symtable(t_node *node, stack_t *symtable) {
    (void)node;
    (void)symtable;

    return EXIT_SUCCESS;
}



int semantic(t_node *node) {
    // pokud je soubor prázdný, není třeba provádět sémantickou analýzu
    if (node->next_count == 3) {
        // pokud je dán pouze výraz require
        if (node->next[2].next_count == 1)
            return SEM_OK;

        // inicializace 
        def_table_t *deftable = def_table_init();
        stack_t *symtable = stack_init(STACK_SIZE);

        process_main_list(&(node->next[2]), symtable, *deftable);

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


