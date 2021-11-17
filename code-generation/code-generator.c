/**
 * @file code-generator.c
 * @authors David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 * @brief Definice funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	17. 11. 2021 13:47:11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "code-generator.h"
#include "tree.h"
#include "mystring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//globální proměnné, slouží k uvolnění paměti a ukončení programu
t_tree*tree_global;
buffer_t*code_buffer_global;

int eval_expression(t_node*tree);

void generate_code(t_tree*tree, buffer_t*code){
    strcpy(code->data, ".IFJcode21\n\n");
    
    //nastavení globálních proměnných
    tree_global = tree;
    code_buffer_global = code;

    //current_node = <prog>
    t_node*current_node = tree->first;

    if (strcmp(current_node->data[0].data, "<prog>") == 0){
        current_node = check_prog_node(current_node); //current_node = <main-list>
    }
    else {
        free_memory_then_quit(2);
    }
    if (current_node == NULL){
        free_memory_then_quit(2);
    }
    
     //current_node = <def-declare-fcall> || eps, cyklus projde přes všechny prvky v rekurzivním listu
    for (current_node = &current_node->next[0]; strcmp(current_node->data[0].data,"eps") != 0; current_node = &current_node->next[1])
    {
        def_declare_fcall_crossroad(code, current_node);
    }
}

t_node*check_prog_node(t_node*prog_node){
    if(strcmp(prog_node->next[0].data[0].data, "require") != 0) {
        return NULL;
    }
    if(strcmp(prog_node->next[1].data[0].data, "\"ifj21\"") != 0) {
        return NULL;
    }
    if(strcmp(prog_node->next[2].data[0].data, "<main-list>") != 0) {
        return NULL;
    }
    return &(prog_node->next[2]);
}

void def_declare_fcall_crossroad(buffer_t*code, t_node*main_node){ //main_node = <def-declare-fcall>
    if (strcmp(main_node->next[0].data[0].data, "global") == 0) {
        const char*id = main_node->next[1].data[1].data; //id = id.attribute
        create_global_variable(code, &main_node->next[3], id); 
    }
    else if (strcmp(main_node->next[0].data[0].data, "function") == 0) {
        function_gen(code, main_node);
    }
    else{
        function_call_gen(code, main_node);
    }
}

void function_call_gen(buffer_t*code, t_node*fcall_node){
    
}

void function_gen(buffer_t*code, t_node*function_node){
    
}

void create_global_variable(buffer_t*code,t_node*declaration_node, const char*id) {
    //declaration_node = function(<types>)<return-types> OR <type><declare-assign>
    if (strcmp(declaration_node->next[0].data[0].data, "function") != 0) {
        strcat_format_realloc(code, "DEFVAR GF@%s\n", id);
        END_IF_FAIL(code);

        if (strcmp(declaration_node->next[0].next[0].data[0].data, "integer") == 0) {
            
        }
        else if (strcmp(declaration_node->next[0].next[0].data[0].data, "string") == 0) {

        }
        else if (strcmp(declaration_node->next[0].next[0].data[0].data, "number") == 0) {
            
        }
    }
}

int find_label(buffer_t*code, char*label_id){
    char label = malloc(strlen(label_id) + 7); //LABEL label_id'\0'
    strcpy(label, "LABEL ");
    strcat(label, label_id);
    char*label_location = strnstr(code->data,label, code->capacity);
    free(label); //we don't need it anymore

    if (label_location == NULL)
        return -1;
    else{
        return label_location - code->data + strlen(label_id) + 7; //index of next char after \n of label
    } 
}

void free_memory_then_quit(int return_code) {
    tree_delete(tree_global);
    buffer_destroy(code_buffer_global);
    exit(return_code);
}
