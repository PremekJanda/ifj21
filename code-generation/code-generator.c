#include "code-generator.h"
#include "tree.h"
#include "mystring.h"
#include <stdlib.h>


int eval_expression(t_node*tree);

void generate_code(t_tree*tree, buffer_t*code){
    strcpy(code->data, ".IFJcode21\n");

    //current_node = <prog>
    t_node*current_node = tree->first;

    if (strcmp(current_node->data[0].data, "<prog>") == 0){
        current_node = check_prog_node(current_node); //current_node = <main-list>
    }
    else {
        tree_delete(tree);
        buffer_destroy(code);
        exit(2);
    }
    if (current_node == NULL){
        tree_delete(tree);
        buffer_destroy(code);
        exit(2);
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
        global_declaration_crossroad(code, main_node);
    }
    else if (strcmp(main_node->next[0].data[0].data, "function") == 0) {
        function_gen(code, main_node);
    }
    else{
        function_call_gen(code, main_node);
    }
}

void function_call_gen(buffer_t*code,t_node*fcall_node){

}

void function_gen(buffer_t*code,t_node*function_node){
    
}

void global_declaration_crossroad(code, main_node) {

}

int find_label(buffer_t*code, char*label_id){
    char label = malloc(strlen(label_id) + 7); //LABEL label_id'\0'
    strcpy(label, "LABEL ");
    strncat(label, label_id, code->capacity);
    char*label_location = strstr(code->data,label);
    free(label); //we don't need it anymore

    if (label_location == NULL)
        return -1;
    else{
        return label_location - code->data;
    } 
}