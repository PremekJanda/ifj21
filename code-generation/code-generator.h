#ifndef __CODE_GENERATOR_H
#define __CODE_GENERATOR_H

#include "tree.h"

int eval_expression(t_node*tree);

void generate_code(t_tree*tree, buffer_t*code);

/**
 * @brief Returns index in code->data of start of LABEL <label> instruction.
 * 
 * @param code Ifj21Code code
 * @param label_id label_id we search for
 * @return int index of letter L of LABEL in the code
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

void function_call_gen(buffer_t*code,t_node*fcall_node);

void function_gen(buffer_t*code,t_node*function_node);

#endif