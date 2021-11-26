/**
 *  Soubor: semantic.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	26. 11. 2021 01:38:42
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

#define FREE_VAR_DECL() \
    free(name); \
    free(type); \
    free(assign_type); \
    free(value); \

#define UPDATE_EXPR(__return) \
    *type = t; \
    *value = v; \
    return __return; 



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
int eval_expr_type(t_node *node, key_t *value, key_t *type, stack_t *symtable);



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

int process_cond(t_node *node, stack_t *symtable);

/**
 * @brief 
 * @param node 
 * @param symtable 
 * @return ERROR_CODE @see #defined
 */
int process_decl_local(t_node *node, stack_t *symtable);

int process_assign_or_fcall(t_node *node, stack_t *symtable, def_table_t *deftable, key_t first_id);

int process_id_list(t_node *node, fce_item_t **item, stack_t *symtable);

int process_param_list(t_node *node, fce_item_t **item, stack_t *symtable);

int process_f_or_item_list(t_node *node, fce_item_t **item, stack_t *symtable);

int process_item_another(t_node *node, fce_item_t **item, stack_t *symtable);

int process_types(t_node *node, fce_item_t **item);

int process_return_types(t_node *node, fce_item_t **item, int *return_values);

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


#define GENERATE_BUILTIN_FUNCTIONS() \
    htab_item_t *item; \
    \
    /* function reads() : string */ \
    ALLOC_STR(type_reads_string, "") \
    ALLOC_STR(value_reads_string, "") \
    ALLOC_STR(reads, "reads") \
    ALLOC_STR(reads_string, "string") \
    def_table_add("reads", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_reads_string, reads, value_reads_string, 0, 0); \
    fce_item_push(&(item->fce), reads_string); \
    item->ret_values = 1; \
    \
    /* function readi() : integer */ \
    ALLOC_STR(type_readi_integer, "") \
    ALLOC_STR(value_readi_integer, "") \
    ALLOC_STR(readi, "readi") \
    ALLOC_STR(readi_integer, "integer") \
    def_table_add("readi", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_readi_integer, readi, value_readi_integer, 0, 0); \
    fce_item_push(&(item->fce), readi_integer); \
    item->ret_values = 1; \
    \
    /* function readn() : number */ \
    ALLOC_STR(type_readn_number, "") \
    ALLOC_STR(value_readn_number, "") \
    ALLOC_STR(readn, "readn") \
    ALLOC_STR(readn_number, "number") \
    def_table_add("readn", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_readn_number, readn, value_readn_number, 0, 0); \
    fce_item_push(&(item->fce), readn_number); \
    item->ret_values = 1; \
    \
    /* function write ( term 1 , term 2 , ... , term n ) */ \
    ALLOC_STR(type_write_, "") \
    ALLOC_STR(value_write_, "") \
    ALLOC_STR(write, "write") \
    def_table_add("write", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_write_, write, value_write_, 0, 0); \
    item->ret_values = 0; \
    \
    /* function tointeger( f : number) : integer */ \
    ALLOC_STR(type_tointeger_integer, "") \
    ALLOC_STR(value_tointeger_integer, "") \
    ALLOC_STR(tointeger, "tointeger") \
    ALLOC_STR(tointeger_integer, "integer") \
    ALLOC_STR(tointeger_f, "number") \
    def_table_add("tointeger", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_tointeger_integer, tointeger, value_tointeger_integer, 0, 0); \
    fce_item_push(&(item->fce), tointeger_integer); \
    fce_item_push(&(item->fce), tointeger_f); \
    item->ret_values = 1; \
    \
    /* function substr( s : string, i : number, j : number) : string */ \
    ALLOC_STR(type_substr_string, "") \
    ALLOC_STR(value_substr_string, "") \
    ALLOC_STR(substr, "substr") \
    ALLOC_STR(substr_string, "string") \
    ALLOC_STR(substr_s, "string") \
    ALLOC_STR(substr_i, "number") \
    ALLOC_STR(substr_j, "number") \
    def_table_add("substr", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_substr_string, substr, value_substr_string, 0, 0); \
    fce_item_push(&(item->fce), substr_string); \
    fce_item_push(&(item->fce), substr_s); \
    fce_item_push(&(item->fce), substr_i); \
    fce_item_push(&(item->fce), substr_j); \
    item->ret_values = 1; \
    \
    /* function ord( s : string, i : integer) : integer */ \
    ALLOC_STR(type_ord_integer, "") \
    ALLOC_STR(value_ord_integer, "") \
    ALLOC_STR(ord, "ord") \
    ALLOC_STR(ord_integer, "integer") \
    ALLOC_STR(ord_s, "string") \
    ALLOC_STR(ord_i, "integer") \
    def_table_add("ord", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_ord_integer, ord, value_ord_integer, 0, 0); \
    fce_item_push(&(item->fce), ord_integer); \
    fce_item_push(&(item->fce), ord_s); \
    fce_item_push(&(item->fce), ord_i); \
    item->ret_values = 1; \
    \
    /* function chr( i : integer) : string */ \
    ALLOC_STR(type_chr_string, "") \
    ALLOC_STR(value_chr_string, "") \
    ALLOC_STR(chr, "chr") \
    ALLOC_STR(chr_string, "string") \
    ALLOC_STR(chr_i, "integer") \
    def_table_add("chr", deftable, 1); \
    item = htab_lookup_add(symtable->stack[0], type_chr_string, chr, value_chr_string, 0, 0); \
    fce_item_push(&(item->fce), chr_string); \
    fce_item_push(&(item->fce), chr_i); \
    item->ret_values = 1;


#endif // __SEMANTIC_H__