/**
 * @file code-generator.h
 * @author Radomír Bábek - xbabek02, VUT FIT
 * @brief Hlavičkový soubor funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	25. 11. 2021 03:18:14
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

typedef struct{
    bool results[3];
    char result_var_id[3][26];
}expr_results_t;


void init_code(code_t*code);

void init_result_field(code_t*code, expr_results_t*results);

void eval_expression(code_t*code, t_node*expr, expr_results_t*result_field);

/**
 * @brief Vygeneruje kód jazyka IFJ21Code ze syntaktického stromu podrobenému syntaktické a sémantické analýze
 * 
 * @param tree Syntaktický strom
 * @param code Řetězcový buffer
 */
void generate_code(t_node*tree, code_t*code);

/**
 * @brief Najde hledaný label v kódu ifj21code
 * 
 * @param code code buffer
 * @param label_id Identifikátor labelu, který vyhledáváme
 * @return int index prvního znaku za odřádkováním instrukce LABEL, nebo -1 při nenalezení
 */
int find_label(code_t*code, char*label_id);

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
 */
void field_of_visibility_id_replacement(char*id, char*new_id, t_node*tree);

/**
 * @brief Zkontroluje, jestli je identifikátor k nalezení v lokální rámci LF
 * 
 * @param code Ifj21Code kód
 * @param id Identifikátor proměnné
 * @return Vrací 1, pokud je k nalezení v LF, jinak 0
 */
int is_local(code_t*code, char*id);


/**
 * @brief Zkontroluje, jestli je identifikátor k nalezení v globálním rámci GF
 * 
 * @param code Ifj21Code kód
 * @param id Identifikátor proměnné
 * @return Ukazatel na definici proměnné, nebo NULL pokud není proměnná k nalezení v GF
 */
char*is_global(code_t*code, char*id);

void generate_assignment(code_t*code, t_node*assignment);

void generate_if(code_t*code, t_node*if_node, char*fc);

void generate_while(code_t*code, t_node*while_node, char*fc);

void generate_local_decl(code_t*code, t_node*local_dec);

void generate_return(code_t*code, t_node*return_node);

void predefine_vars_of_stmt_list(code_t*code, t_node*stmt_list);

void eval_condition(code_t*code, t_node*condition);

void stmt_list_crossroad(code_t*code, t_node*stmt_list, char*fc_from);

#endif