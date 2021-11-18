/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:18. 11. 2021 04:11:48
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Obsahuje sémantické kontroly nad abstraktním syntaktickým stromem
 *         generovaným syntaktickou analýzou
 */


#include "semantic.h"
#include "symtable.h"
#include "tree.h"

#include "test.h"

// - - - - - - - - - - - - - - - - - - - - //
// - - - - - - - - M_A_I_N - - - - - - - - //
// - - - - - - - - - - - - - - - - - - - - //

// #define VAR_NAME(var) #var

#define TYPE_CHECK() \

// #define TYPE_CHECK() 
    
int is_f_set(def_table_t deftable, char *name, int state) {
    printf("name: %s\n", name);

    switch (state) {
    case DEFINED:
        for (size_t i = 0; i < deftable.size; i++) {
            if (strcmp(deftable.item[i].name, name) == 0) {
                if (deftable.item[i].data == DEFINED) {
                    printf("redefinice funkce\n");
                } else {
                    printf("Deklarovanou funkci nelze znovu definovat\n");
                }
                // exit(SEM_DEFINE);
                return EXIT_FAILURE;
            }
        }
        
        break;

    
    case DECLARED:
        for (size_t i = 0; i < deftable.size; i++) {
            if (strcmp(deftable.item[i].name, name) == 0) {
                if (deftable.item[i].data == DECLARED) {
                    printf("redeklarace funkce\n");
                } else {
                    printf("Definovanou funkci nelze znovu deklarovat\n");
                }
                // exit(SEM_DEFINE);
                return EXIT_FAILURE;
            }
        }
        
        break;

    
    case CALLED:
        for (size_t i = 0; i < deftable.size; i++) {
            if (strcmp(deftable.item[i].name, name) == 0) {
                if (deftable.item[i].data == DECLARED) {
                    printf("funkce nebyla definována\n");
                    // exit(SEM_DEFINE);
                    return EXIT_FAILURE;
                } else {
                    return EXIT_SUCCESS;
                }
            }
        }

        // funkce nebyla nalezena
        printf("volání nedefinované funkce\n");
        // exit(SEM_DEFINE);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int create_symtable(t_node node) {
    // do else function if while
    (void)node;
    
    return SEM_OK;
}

int add_scope_to_symtable() {
    
    

    return SEM_OK;
}


int main() {

    // * TESTOVÁNÍ
    MAKE_TREE()
    // MAKE_SMALL_TREE()
    // MAKE_DEF_TABLE()
    // MAKE_SYMTABLE()
    
    printf("\n\n");


    def_table_t *deftable = def_table_init();

    t_node node = node4;

    while (node.next_count != 1) {
        t_node curr = node.next[0];
        t_node next = node.next[0].next[0];

        printf("curr: %s\n", curr.data[1].data);
        printf("next: %s\n", next.data[1].data);
        
        if (strcmp(next.data[1].data, "function") == 0) {
            printf("function definition\n");
            is_f_set(*deftable, curr.next[1].data[1].data, DEFINED);
            def_table_add(deftable, curr.next[1].data[1].data, DEFINED);

        } else if (strcmp(next.data[1].data, "global") == 0) {
            printf("function declaration\n");
            is_f_set(*deftable, curr.next[1].data[1].data, DECLARED);
            def_table_add(deftable, curr.next[1].data[1].data, DEFINED);
            
        } else {
            printf("function call\n");
            is_f_set(*deftable, next.data[1].data, CALLED);
        }

        printf("\n");

        node = node.next[1];
    }
    
    
    // * PRINT
    def_table_print(*deftable);

    

    // * TESTOVÁNÍ
    node_delete(&node1);
    // stack_free(stack);
    def_table_free(deftable);
    
    return EXIT_SUCCESS;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích stránek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


