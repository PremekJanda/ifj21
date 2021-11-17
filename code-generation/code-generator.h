#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATOR_H

#include "tree.h"

int eval_expression(t_node*tree);

void generate_code(t_tree*tree, buffer_t*code);

typedef enum {TF,LF,GF} frame_t;

#define END_IF_FAIL(code) \
    if (code->data == NULL){ \
        free_memory_then_quit(99);\
    }\

/**
 * @brief Najde hledaný label v kódu ifj21code
 * 
 * @param code code buffer
 * @param label_id Identifikátor labelu, který vyhledáváme
 * @return int index prvního znaku za odřádkováním instrukce LABEL, nebo -1 při nenalezení
 */
int find_label(buffer_t*code, char*label_id);

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
void def_declare_fcall_crossroad(buffer_t*code, t_node*main_node);

/**
 * @brief Vygeneruje kód volání funkce
 * 
 * @param code Buffer, do kterého funkce zapíše mezikód
 * @param fcall_node Uzel syntaktického stromu, obsahující volání funkce
 */
void function_call_gen(buffer_t*code,t_node*fcall_node);

/**
 * @brief Vygeneruje kód funkce
 * 
 * @param code Buffer, do kterého funkce zapíše mezikód
 * @param function_node Uzel syntaktického stromu, obsahující definici funkce
 */
void function_gen(buffer_t*code,t_node*function_node);

/**
 * @brief Ukončí program, uvolní všechnu přiřazenou paměť
 * 
 * @param return_code kód, se kterým program skončí
 */
void free_memory_then_quit(int return_code);

void create_function_label(buffer_t*code, const char*id);

/**
 * @brief Vygeneruje kód pro deklaraci globální proměnné s možností přiřazení hodnoty
 * 
 * @param code Buffer, do kterého se píše mezikód
 * @param declaration_node Uzel syntaktického stromu, obsahující globální deklaraci
 * @param id Identifikátor funkce, nebo proměnné. Deklarace funkce bude ignorována
 * 
 */
void create_global_variable(buffer_t*code,t_node*declaration_node, const char*id);

#endif