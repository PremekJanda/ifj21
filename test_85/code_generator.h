/**
 * @file code-generator.h
 * @author Radomír Bábek - xbabek02, VUT FIT
 * @brief Hlavičkový soubor funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	28. 11. 2021 00:39:33
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATOR_H

#include "tree.h"
#include "mystring.h"
#include "hashtable.h"
#include <stdbool.h>

typedef enum {TF,LF,GF} frame_t;

#define END_IF_FAIL(buffer) \
    if (buffer->data == NULL){ \
        free_memory_then_quit(99);\
    }\

typedef struct{
    int total_expr_count;
    int total_conditionals_count;
    int total_label_count;
    buffer_t text;
}code_t;

void init_code(code_t*code);

void convert_strings(t_node*tree);

void eval_expression(code_t*code, t_node*expr);

void fix_expr(t_node*tree);


// macro to create new write function call with one argument item, item will be blank and at index 9
#define CREATE_NEW_WRITE_CALL(new_nodes) \
    node_setdata(new_nodes[0], "<stmt-list>", 0); \
    node_setdata(new_nodes[0], "", 1); \
    \
    node_setdata(new_nodes[1], "<stmt>", 0); \
    node_setdata(new_nodes[1], "", 1); \
    node_addnext(new_nodes[0], new_nodes[1]); \
    \
    node_setdata(new_nodes[2], "id", 0); \
    node_setdata(new_nodes[2], "write_fc", 1); \
    node_addnext(new_nodes[1], new_nodes[2]); \
    \
    node_setdata(new_nodes[3], "<assign-or-fcall>", 0); \
    node_setdata(new_nodes[3], "", 1); \
    node_addnext(new_nodes[1], new_nodes[3]); \
    \
    node_setdata(new_nodes[4], "(", 0); \
    node_setdata(new_nodes[4], "(", 1); \
    node_addnext(new_nodes[3], new_nodes[4]);\
    \
    node_setdata(new_nodes[5], "<param-list>", 0); \
    node_setdata(new_nodes[5], "", 1); \
    node_addnext(new_nodes[3], new_nodes[5]); \
    \
    node_setdata(new_nodes[6], ")", 0); \
    node_setdata(new_nodes[6], ")", 1); \
    node_addnext(new_nodes[3], new_nodes[6]); \
    \
    node_setdata(new_nodes[7], "<item-list>", 0); \
    node_setdata(new_nodes[7], "", 1); \
    node_addnext(new_nodes[5], new_nodes[7]); \
    \
    node_setdata(new_nodes[8], "<item>", 0); \
    node_setdata(new_nodes[8], "", 1); \
    node_addnext(new_nodes[7], new_nodes[8]); \
    \
    node_setdata(new_nodes[9], "<item-another>", 0); \
    node_setdata(new_nodes[9], "", 1); \
    node_addnext(new_nodes[7], new_nodes[9]); \
    \
    node_setdata(new_nodes[10], "eps", 0); \
    node_setdata(new_nodes[10], "", 1); \
    node_addnext(new_nodes[9], new_nodes[10]);

/**
 * @brief Vygeneruje kód jazyka IFJ21Code ze syntaktického stromu podrobenému syntaktické a sémantické analýze
 * 
 * @param tree Syntaktický strom
 * @param code Řetězcový buffer
 */
void generate_code(t_node*tree, code_t*code);

/**
 * @brief Zkontroluje první uzel syntaktického stromu a posune se dál v analýze
 * 
 * @param prog_node První uzel syntaktického stromu
 * @return Ukazatel na uzel <main-list>
 */
t_node*check_prog_node(t_node*prog_node);

/**
 * @brief Zpracuje main uzel syntaktického stromu
 * 
 * @param code Buffer, do kterého funkce zapíše mezikód
 * @param main_node Uzel, který se zpracovává
 * @return 0 při nedostatku paměti, 1 při úspěchu
 */
void def_declare_fcall_crossroad(code_t*code, t_node*main_node);

/**
 * @brief Vygeneruje kód volání funkce
 * 
 * @param code Buffer, do kterého funkce zapíše mezikód
 * @param fcall_node Uzel syntaktického stromu, obsahující volání funkce
 */
void function_call_gen(code_t*code,t_node*fcall_node);

/**
 * @brief Překopíruje obsah uzlu item do proměnné uvnitř IFJ21Code
 * 
 * @param code Code buffer
 * @param dest_frame Rámec proměnné, do které budeme zapisovat
 * @param dest_id Identifikátor proměnné, do které budeme zapisovat
 * @param item Uzel, jehož obsah budeme kopírovat
 */
void move_item_to_var(code_t*code, char*dest_frame, const char*dest_id, t_node*item);

/**
 * @brief Vygeneruje kód funkce
 * 
 * @param code Buffer, do kterého funkce zapíše mezikód
 * @param function_node Uzel syntaktického stromu, obsahující definici funkce
 */
void function_gen(code_t*code,t_node*function_node);

/**
 * @brief Ukončí program, uvolní všechnu přiřazenou paměť
 * 
 * @param return_code kód, se kterým program skončí
 */
void free_memory_then_quit(int return_code);

void create_function_label(code_t*code, const char*id);

/**
 * @brief Vygeneruje kód pro deklaraci globální proměnné s možností přiřazení hodnoty
 * 
 * @param code Buffer, do kterého se píše mezikód
 * @param declaration_node Uzel syntaktického stromu, obsahující globální deklaraci
 * @param id Identifikátor funkce, nebo proměnné. Deklarace funkce bude ignorována
 * 
 */
void create_global_variable(code_t*code,t_node*declaration_node, const char*id);

/**
 * @brief Projde syntaktický strom a přejmenuje všechny identifikátory na unikátní. Rekurzivní funkce.
 * 
 * @param tree Syntaktický strom
 * @param ht_already_processed Hashovací tabulka, zajistí, že se nebude opravovat dvakrát stejný identifikátor
 * @param fc Identifikátor funkce, ve které se průchod stromem momentálně nachází
 * @param depth Úroveň zanoření
 * @param depth_total Hodnota začne na 0 a zvýší se při každém zanoření
 */
void rename_all_id(t_node* tree, ht_table_t*ht_already_processed, char**fc, int*depth, int*depth_total);

/**
 * @brief Změní názvy identifikátorů pouze pro ty které opravdu značí stejnou proměnnou
 * 
 * @param id Název identifikátoru k nalezení v syntaktickém stromu
 * @param new_id Nový unikátní identifikátor
 * @param tree Syntaktický strom
 * 
 * @return -1 pokud má přestat rekurzivní volání funkce nad všemi potomky
 */
int field_of_visibility_id_replacement(char*id, char*new_id, t_node*tree);

/**
 * @brief Zkontroluje, jestli je identifikátor k nalezení v lokální rámci LF
 * 
 * @param code Ifj21Code kód
 * @param id Identifikátor proměnné
 * @return Vrací 1, pokud je k nalezení v LF, jinak 0
 */
int is_local(code_t*code, char*id);

/**
 * @brief Zjistí, jestli je proměnná globální, funguje pouze na názvy proměnných ze syntaktického stromu
 * 
 * @param code Ifj21Code kód
 * @param id Identifikátor proměnné
 * @return booleovskou hodnotu
 */
bool is_global(char*id);

void convert_write(t_node*tree);

void generate_assignment(code_t*code, t_node*assignment);

void generate_if(code_t*code, t_node*if_node, char*fc);

void generate_while(code_t*code, t_node*while_node, char*fc);

void generate_local_decl(code_t*code, t_node*local_dec);

void generate_return(code_t*code, t_node*return_node);

void predefine_vars_of_stmt_list(code_t*code, t_node*stmt_list);

void eval_condition(code_t*code, t_node*condition);

void stmt_list_crossroad(code_t*code, t_node*stmt_list, char*fc_from);

#endif