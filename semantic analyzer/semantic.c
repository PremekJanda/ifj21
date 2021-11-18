/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:18. 11. 2021 02:31:57
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

// #define TYPE_CHECK() \
    
int is_f_set(def_table_t deftable, int state) {

    return 0;
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


    def_table_t *deftable = def_table_init();

    t_node node = node4;

    while (node.next_count != 1) {
        if (strcmp(node.next[0].next[0].data[0].data, "function") == 0) {
            printf("function definition\n");
            def_table_add(deftable, node.next[0].next[0].data[0].data, DEFINED);
        } else if (strcmp(node.next[0].next[0].data[0].data, "id") == 0) {
            printf("function call\n");
        } else {
            printf("function declaration\n");
        }

        node = node.next[1];
    }
    
    
    // * PRINT
    def_table_print(*deftable);

    

    // * TESTOVÁNÍ
    node_delete(&node1);
    // stack_free(stack);
    def_table_free(deftable);
    
    return 0;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce pro zásobník hashovacích stránek - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


