/**
 *  Soubor: semantic.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	24. 11. 2021 06:27:18
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Rozhraní pro semantickou analýzu
 */

// následující řádky zabrání násobnému vložení:
#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "symtable.h"
#include "tree.h"

#include "test.h"


// 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
//      undefined variable
//      redefined variable
//      undefined function
//      redefined function
//      undefined reference to function
//      undefined reference to variable
//      non-initialized variable
// 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
//      type incompatibility
// 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce.
//      invalid parameter type
//      invalid return type
//      invalid number of returned values
//      no return ([type] expected)
// 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
//      incompatible types
// 7 - ostatní sémantické chyby.
//      index out of range

// ? Možné rozšíření warningů
//      unused function
//      unused variable

// TODO
// operáto # očekává string

#define SEM_OK 0
#define SEM_DEFINE 3
#define SEM_ASSIGN 4
#define SEM_FCE 5
#define SEM_TYPE 6
#define SEM_OTHER 7

#define LOCAL 1
#define GLOBAL 0

// Vrací funkce co má v každé větvi
// Vrací funkce v každé větvi (if else musí vrátit v obou případech)
// Sedí typy
// Jasou proměnné definované
// Jsou redefinované

// pozor na popnutí globálního rámce
// přesunout deklaraci proměnné před while, aby nedocházelo k redefinování s každým cyklem 
// přepis proměnné na stejném rámci




// - - - - - - - - - - - - - - - //
// - - - - Pomocná makra - - - - //
// - - - - - - - - - - - - - - - //


#define _ERR() \
    if ((return_signal = 

#define ERR_() \
    ) != 0) return return_signal; 

#define TYPE_CHECK(__var_type, __assign_type) \
    if (strcmp(__var_type, __assign_type)) \
        return SEM_ASSIGN;


#define ALLOC_STR(__var_name, __src) \
    key_t __var_name = malloc(strlen(__src) + 1); \
    ALLOC_CHECK(__var_name) \
    sprintf(__var_name, "%s", __src);


#define TEMP_VARS() \
    t_node *curr = node->next[0]; \
    t_node *next = node->next[0]->next[0]; \
    (void)curr; \
    (void)next;

    // printf("curr: %s\n", curr->data[1].data);
    // printf("next: %s\n", next->data[1].data);



// - - - - - - - - - - - - - - - - - - //
// - - - - Sémantické kontroly - - - - //
// - - - - - - - - - - - - - - - - - - //

/**
 * @brief Hlavní funkce zajišťující sémantickou kontrolu
 * @param root_node Ukazatel na kořenový stromu
 */
int semantic(t_node *root_node);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Sémantická kontrola programu - exit(3)  - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief 
 * @param id 
 * @param symtable 
 * @param deftable 
 * @return ERROR_CODE @see #defined
 */
int is_id_used(key_t id, stack_t *symtable, def_table_t deftable);

/**
 * @brief 
 * @param name 
 * @param symtable 
 * @param state 
 * @return ERROR_CODE @see #defined
 */
int is_f_set(key_t name, def_table_t *deftable, int state);

/**
 * @brief Hledá funkce, které byly volány a pokud nejsou definovány, tak vyhodí chybu
 * @param deftable 
 * @return ERROR_CODE @see #defined
 */
int eval_fcall(def_table_t deftable);



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Typová nekompatibilita výrazů - exit(6) - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief 
 * @param node
 * @return ERROR_CODE @see #defined
 */
// int eval_expr_type(t_node node, key_t value, key_t type);
int eval_expr_type(t_node *node, key_t *value, key_t *type);



// - - - - - - - - - - - - - - - - - - - //
// - - - -  Zpracování gramatiky - - - - //
// - - - - - - - - - - - - - - - - - - - //

/**
 * @brief Zpracování těla programu aka globálních proměnných a funkcí
 * @param node 
 * @param symtable 
 * @param deftable 
 * @return ERROR_CODE @see #defined
 */
int process_main_list(t_node *node, stack_t *symtable, def_table_t *deftable);

/**
 * @brief 
 * @param node 
 * @param symtable 
 * @param deftable 
 * @return ERROR_CODE @see #defined
 */
int process_stmt_list(t_node *node, stack_t *symtable, def_table_t *deftable);

int process_while(t_node *node, stack_t *symtable, def_table_t *deftable);

int process_if(t_node *node, stack_t *symtable, def_table_t *deftable);

/**
 * @brief 
 * @param node 
 * @param symtable 
 * @return ERROR_CODE @see #defined
 */
int process_decl_local(t_node *node, stack_t *symtable);

int process_cond(t_node *node);

int process_types(t_node *node, fce_item_t **item);

int process_return_types(t_node *node, fce_item_t **item, size_t *return_values);

int process_f_arg_list(t_node *node, fce_item_t **item, stack_t *symtable);

// - - - - - - - - - - - - - - - - - - - //
// - - - - -  Práce s funkcemi - - - - - //
// - - - - - - - - - - - - - - - - - - - //

int f_declare(t_node *node, stack_t *symtable);

int f_define(t_node *node, stack_t *symtable, def_table_t *deftable);



// - - - - - - - - - - - - - - - - - - - - - - - //
// - - - -  Operace nad tabulkou symbolů - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - //

/**
 * @brief 
 * @param node 
 * @param symtable 
 * @param value 
 * @return ERROR_CODE @see #defined
 */
int add_var_to_symtable(key_t type, key_t attribute, key_t value, bool local, stack_t *symtable);

/**
 * @brief 
 * @param node 
 * @param symtable 
 * @return ERROR_CODE @see #defined
 */
int add_scope_to_symtable(stack_t *symtable);

int add_f_to_table();


#endif // __SEMANTIC_H__