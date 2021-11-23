/**
 * @file code-generator.c
 * @authors David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 * @brief Definice funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	23. 11. 2021 20:12:53
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
#include "hashtable/hashtable.h"
#include "test.h"

//globální proměnné, slouží k uvolnění paměti a ukončení programu
t_tree*global_tree;
buffer_t*global_code_buffer;

void generate_code(t_tree*tree, code_t*code){
    //nastavení globálních proměnných
    global_tree = tree;
    global_code_buffer = &code->text;

    strcpy(code->text.data, ".IFJcode21\n\n");
    strcat_realloc(&code->text, "# -- global variables for type checks\nDEFVAR GF@TYPE_var_1\nDEFVAR GF@TYPE_var_2\n\n");
    strcat_realloc(&code->text, "# -- result variable for conversion\nDEFVAR GF@CONVERTION_var\n\n");
    strcat_realloc(&code->text, "# -- operand variables for expression evaluation\nDEFVAR GF@OP_EXPR1\nDEFVAR GF@OP_EXPR2\n");
    //změna názvů identifikátorů ve stromě na unikátní identifikátory
    ht_table_t ht_already_processed;
    ht_init(&ht_already_processed);

    int depth = 0;
    int depth_total = 0;
    
    printf("\n\n\n\n");
    char*fc = NULL;
    rename_all_id(tree->first, &ht_already_processed, &fc, &depth, &depth_total);
    tree_print(*tree->first, 0);
    ht_delete_all(&ht_already_processed);

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
    for (; strcmp(current_node->next[0]->data[0].data,"eps") != 0; current_node = current_node->next[1]){
        def_declare_fcall_crossroad(code, current_node);
    }
}

void rename_all_id(t_node* tree, ht_table_t*ht_already_processed, char**fc, int*depth, int*depth_total) {
    if (strcmp(tree->data[0].data, "id") == 0){
        char*id = malloc(strlen(tree->data[1].data)+1);
        if (id == NULL) {
            free_memory_then_quit(99);
        }
        strcpy(id,tree->data[1].data); 
        if (ht_search(ht_already_processed,id) == NULL) {
            buffer_t new_id;
            buffer_init(&new_id);

            if(*fc == NULL) { // v globálním prostoru
                if (strcmp(tree->prev->next[0]->data[1].data, "function") == 0) {
                    *fc = tree->prev->next[1]->data[1].data;
                    strcat_format_realloc(&new_id, "%s_fc", id);
                    (*depth)++;
                    (*depth_total)++;

                }
                else if (strcmp(tree->prev->next[0]->data[1].data, "global") == 0){
                    if (strcmp(tree->prev->next[3]->data[1].data, "function") == 0){
                        strcat_format_realloc(&new_id, "%s_fc", id);
                    }
                    else {
                        strcat_format_realloc(&new_id, "%s_global", id);
                    }
                }
            }
            else{
                strcat_format_realloc(&new_id, "%s_%s_depth%d_dt%d", id, *fc, *depth, *depth_total);
            }

            END_IF_FAIL((&new_id));
            t_node*ref_node = tree;
            node_setdata(tree, new_id.data, 1);
            while (strcmp(ref_node->data[0].data, "<main-list>") != 0 && strcmp(ref_node->data[0].data, "<stmt-list>") != 0){
                ref_node = ref_node->prev;
            } 
            field_of_visibility_id_replacement(id, new_id.data, ref_node);
            ht_insert(ht_already_processed, new_id.data, 1);
            buffer_destroy(&new_id);
            free(id);
        }
    }
    else if ((strcmp(tree->data[1].data, "while") == 0) || (strcmp(tree->data[1].data, "if") == 0)){
        (*depth)++;
        (*depth_total)++;
    }
    else if ((strcmp(tree->data[1].data, "else") == 0)) {
        (*depth_total)++;
    }
    else if (strcmp(tree->data[1].data, "end") == 0) {
        (*depth)--;
        if (*depth == 0) {
            *fc = NULL;
        }
    }
    
    if (tree->next_count != 0) {
        for (int i = 0; i < tree->next_count; i++){
            rename_all_id(tree->next[i], ht_already_processed, fc, depth, depth_total);
        }
    }
}

void field_of_visibility_id_replacement(char*id, char*new_id, t_node*tree){
    if (strcmp(tree->data[0].data, "<stmt>") == 0) { //need to check for
        t_node*aux_node = tree->next[0]; //into the statement
        if (strcmp(aux_node->data[0].data, "<decl-local>") == 0) {
            if (strcmp(aux_node->next[1]->data[1].data, id) == 0) {
                aux_node = aux_node->next[4]; //aux node = <decl-assign>
                if (aux_node->next_count == 2) { // <decl-assign> -> = <item>
                    if (strcmp(aux_node->next[1]->next[0]->data[0].data, "id") == 0) {
                        if (strcmp(aux_node->next[1]->next[0]->data[1].data, id) == 0) {
                            /* replace the last id with this identificator, all
                                the other variables with the same identificator
                                will mean different variable, until the end of the if
                                or while statement */
                            node_setdata(aux_node->next[1]->next[0], new_id, 1);
                        }
                    } 
                }
                //now we must find the END keyword and then continue replacing

                while ((strcmp(aux_node->data[0].data, "<stmt-list>") != 0) && (strcmp(aux_node->data[0].data, "<main-list>") != 0)) {
                    aux_node = aux_node->prev;
                }

                //aux_node == <def-decl-fcall> || <while> || <if>
                aux_node = aux_node->prev->next[1]; //will be in outer <stmt-list> or <main-list>
                field_of_visibility_id_replacement(id, new_id, aux_node);
            }  
        }
    } //we will not be dealing with variable shadowing
    
    else if (strcmp(tree->data[0].data, "id") == 0) {  
        if (strcmp(tree->data[1].data, id) == 0) {
            tree->data[1].data = realloc(tree->data[1].data, strlen(new_id) +1);
            if (tree->data[1].data == NULL) {
                free_memory_then_quit(99);
            }
            strcpy(tree->data[1].data, new_id);
        }
    }
    for (int i = 0; i < tree->next_count; i++){
        if (tree->next[i] != NULL) {
            field_of_visibility_id_replacement(id, new_id, tree->next[i]);
        }
    }
}

t_node*check_prog_node(t_node*prog_node){
    if(strcmp(prog_node->next[0]->data[1].data, "require") != 0) {
        return NULL;
    }
    if(strcmp(prog_node->next[1]->data[1].data, "\"ifj21\"") != 0) {
        return NULL;
    }
    if(strcmp(prog_node->next[2]->data[0].data, "<main-list>") != 0) {
        return NULL;
    }
    return prog_node->next[2];
}

void def_declare_fcall_crossroad(code_t*code, t_node*main_node){ //main_node = <def-declare-fcall>
    if (strcmp(main_node->next[0]->data[0].data, "global") == 0) {
        const char*id = main_node->next[1]->data[1].data; //id = id.attribute
        create_global_variable(code, main_node->next[3], id); 
    }
    else if (strcmp(main_node->next[0]->data[0].data, "function") == 0) {
        function_gen(code, main_node);
    }
    else{
        function_call_gen(code, main_node);
    }
}

void function_call_gen(code_t*code, t_node*fcall_node){
    
}

void function_gen(code_t*code, t_node*function_node){
    
}

void generate_MOVE(code_t*code, const char*id_dst, char*dst_type, char*frame_dst, t_node*item){
    if (strcmp(item->next[0]->data[0].data, "id") == 0) {
        
        strcat_format_realloc(&code->text, "TYPE GF@TYPE_var_1 %s@%s\n", frame_dst, id_dst);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "MOVE GF@TYPE_var_2 string@%s\n", dst_type);
        END_IF_FAIL((&code->text));
        buffer_t label;
        buffer_init(&label);
        strcat_format_realloc(&label, "TYPES_EQ_%d", code->total_label_count++);
        END_IF_FAIL((&label));
        char item_id_frame[3];
        strcat_format_realloc(&code->text, "JUMPIFEQ %s GF@TYPE_var1 GF@TYPE_var_2\n\n", label.data);
        END_IF_FAIL((&code->text));

        if (is_global(code, item->next[0]->data[1].data) != NULL) {
            strcpy(item_id_frame, "GF");
        }
        else if(is_local(code, item->next[0]->data[1].data)){
            strcpy(item_id_frame, "LF");
        }
        else {
            strcpy(item_id_frame, "TF");
        }

        //typy nejsou stejné, poté musí být typ v pravo integer a vlevo number
        strcat_format_realloc(&code->text, "INT2FFLOAT %s@%s %s@%s", item_id_frame, item->next[0]->data[1].data, item_id_frame, item->next[0]->data[1].data);
        END_IF_FAIL((&code->text));
        

        strcat_format_realloc(&code->text, "%s:\nMOVE %s@%s %s@%s\n", label.data, frame_dst, id_dst,item_id_frame, item->next[0]->data[1].data);
        END_IF_FAIL((&code->text));
        buffer_destroy(&label);
    }
    else if (strcmp(item->next[0]->data[0].data, "expr") == 0) {
        expr_results_t results;
        init_result_field(code, &results);

        eval_expression(code, item->next[0], &results);
        
        strcat_format_realloc(&code->text, "MOVE %s@%s LF@%s", frame_dst, id_dst ,results.result_var_id[1]);
        END_IF_FAIL((&code->text));
    }
}

void init_result_field(code_t*code, expr_results_t*results) {
    for (int i = 0; i < 3; i++)
    {
        results->results[i] = false;
        results->result_var_id[i][0] = '\0';

        sprintf(results->result_var_id[i], "result%d_e%d", i+1, code->total_expr_count++);
        strcat_format_realloc(&code->text, "DEFVAR LF@%s\n", results->result_var_id[i]);
    }
}

int is_local(code_t*code, char*id) {
    char*dest;
    buffer_t buffer;
    buffer_init(&buffer);
    strcat_format_realloc(&buffer, "DEFVAR LF@%s",id);
    dest = strstr(code->text.data, buffer.data);
    int local;

    if (dest != NULL) {
        int PUSHFRAME_cnt, POPFRAME_cnt = 0;
        char*dest_save = dest;
        //tato implementace by nefungovala, pokud by více popframe za sebou způsobilo ztrátu rámce
        //to se ale nemůže stát, protože, je proměnná buď lokální, nebo dočasná, jinak by k ní vůbec nebyl přístup
        while ((dest = strstr(dest, "POPFRAME")) != NULL){
            POPFRAME_cnt++;
            dest++;
        }
        dest = dest_save;
        while ((dest = strstr(dest, "PUSHFRAME")) != NULL){
            PUSHFRAME_cnt++;
            dest++;
        }
        if (POPFRAME_cnt == PUSHFRAME_cnt) {
            local = 1;
        }
    }
    else{
        buffer.data[7] = 'T';
        buffer.data[8] = 'F';
        dest = strstr(code->text.data, buffer.data);
        if (dest != NULL) {
            int PUSHFRAME_cnt, POPFRAME_cnt = 0;
            char*dest_save = dest;
            while ((dest = strstr(dest, "POPFRAME")) != NULL){
                POPFRAME_cnt++;
                dest++;
            }
            dest = dest_save;
            while ((dest = strstr(dest, "PUSHFRAME")) != NULL){
                PUSHFRAME_cnt++;
                dest++;
            }
            if (POPFRAME_cnt == PUSHFRAME_cnt) {
                local = 0;
            }
        }
    }

    buffer_destroy(&buffer);
    return local;
}

char*is_global(code_t*code, char*id){
    buffer_t buffer;
    buffer_init(&buffer);
    strcat_format_realloc(&buffer,"%s_global",id);
    char*result = strstr(code->text.data,buffer.data);
    buffer_destroy(&buffer);
    return result;
}

void create_global_variable(code_t*code,t_node*declaration_node, const char*id) {
    //declaration_node = function(<types>)<return-types> OR <type><declare-assign>
    if (strcmp(declaration_node->next[0]->data[0].data, "<type>") == 0) {
        strcat_format_realloc(&code->text, "DEFVAR GF@%s\n", id);
        END_IF_FAIL((&code->text));

        t_node*assignment_node;
        assignment_node = declaration_node->next[1]; // <declare-assign>
        if (strcmp(assignment_node->next[0]->data[0].data,"eps") != 0) {
            generate_MOVE(code, id, declaration_node->next[0]->next[0]->data[0].data, "GF", assignment_node->next[1]);
            //EXPR EVAL

            if (strcmp(declaration_node->next[0]->next[0]->data[0].data, "integer") == 0) { //<type> -> integer
                
            }
            else if (strcmp(declaration_node->next[0]->next[0]->data[0].data, "string") == 0) { //<type> -> string
                
            }
            else if (strcmp(declaration_node->next[0]->next[0]->data[0].data, "number") == 0) { //<type> -> number
            
            }
        }
        
    }
}

void implicite_retyping(t_node*expr1, t_node*expr2){
    if (strcmp(expr1->data[0].data, "integer") == 0 && strcmp(expr2->data[0].data, "number") == 0) {
        buffer_t buffer;
        buffer_init(&buffer);
        strcat_format_realloc(&buffer, "%a", (float)atoi(expr1->data[1].data));
        END_IF_FAIL((&buffer));
        expr1->data[1].data = realloc(expr1->data[1].data, strlen(buffer.data) + 1);
        strcpy(expr1->data[1].data, buffer.data);
        expr1->data[1].length = strlen(buffer.data);

        strcpy(expr1->data[0].data, "number");
        expr1->data[0].length = 6;

        buffer_destroy(&buffer);
    }

    else if (strcmp(expr2->data[0].data, "integer") == 0 && strcmp(expr1->data[0].data, "number") == 0) {
        buffer_t buffer;
        buffer_init(&buffer);
        strcat_format_realloc(&buffer, "%a", (float)atoi(expr2->data[1].data));
        END_IF_FAIL((&buffer));
        expr2->data[1].data = realloc(expr2->data[1].data, strlen(buffer.data) + 1);
        strcpy(expr2->data[1].data, buffer.data);
        expr2->data[1].length = strlen(buffer.data);

        strcpy(expr2->data[0].data, "number");
        expr2->data[0].length = 6;

        buffer_destroy(&buffer);
    }
}

void convert_strings(t_node*tree){
    if (tree->next_count == 0){
        if (strcmp(tree->data[0].data, "string") == 0) {
            buffer_t buffer;
            buffer_init(&buffer);
            strcpy_realloc(&buffer, tree->data[0].data);
            END_IF_FAIL((&buffer));
            replace_all_chars_by_string(&buffer, '\\', "\\092");
            replace_all_chars_by_string(&buffer, '\t', "\\009");
            replace_all_chars_by_string(&buffer, '\r', "\\013"); 
            replace_all_chars_by_string(&buffer, '\v', "\\011");
            replace_all_chars_by_string(&buffer, '\a', "\\007"); 
            replace_all_chars_by_string(&buffer, '\f', "\\012");
            replace_all_chars_by_string(&buffer, '\b', "\\008");
            replace_all_chars_by_string(&buffer, '\n', "\\010");  
            replace_all_chars_by_string(&buffer, ' ', "\\032");
            replace_all_chars_by_string(&buffer, '#', "\035");
        }
    }
    else{
        for(int i = 0; i < tree->next_count; i++) {
            convert_strings(tree->next[i]);
        }
    }
}

void generate_assignment(){
    
}

char *first_free_result(expr_results_t*result_field)
{
    for (int i = 0; i < 3; i++)
    {
        if (result_field->results[i] == false){
            return result_field->result_var_id[i];
        }
    }   
    return NULL;
}

void was_result_then_free_it(t_node*node, expr_results_t*results){
    for (int i = 0; i < 3; i++) {
       if (strcmp(node->data[1].data,results->result_var_id[i]) == 0){
           results->results[i] = false;
           return;
        }
    }
}


void eval_expression(code_t*code, t_node*expr, expr_results_t*result_field) {
    if (expr->next_count != 1){
        if (expr->next_count == 3){ //binární operátory
            eval_expression(code, expr->next[0], result_field);
            eval_expression(code, expr->next[2], result_field);

            if (expr->next[1]->data[1].data[0] == '+'){ 
                implicite_retyping(expr->next[0], expr->next[2]);
                strcat_format_realloc(&code->text, "MOVE GF@OP_EXPR1 nil@nil");
                strcat_format_realloc(&code->text, "MOVE GF@OP_EXPR2 nil@nil");
                generate_MOVE(code, "OP_EXPR1", "nil", "GF", expr->next[0]);
                generate_MOVE(code, "OP_EXPR1", "nil", "GF", expr->next[2]);

                was_result_then_free_it(expr->next[0], result_field);
                was_result_then_free_it(expr->next[2], result_field);

                char*result = first_free_result(result_field);

                strcat_format_realloc(&code->text, "ADD LF@%s GF@OP_EXPR1 GF@OP_EXPR2", result);
                node_setdata(expr, "id", 0);
                node_setdata(expr, result, 1);
                node_delete(expr->next[0]);
                node_delete(expr->next[1]);
                node_delete(expr->next[2]);
                
                
            }
            else if (expr->next[1]->data[1].data[0] == '-'){
                implicite_retyping( expr->next[0], expr->next[2]);
            }
            else if (expr->next[1]->data[1].data[0] == '*'){
                implicite_retyping(expr->next[0], expr->next[2]);
            }
            else if (expr->next[1]->data[1].data[0] == '/'){
                implicite_retyping(expr->next[0], expr->next[2]);
            }
            else if (strcmp(expr->next[1]->data[1].data, "..") == 0){
                
            }
        }
        else if (expr->next_count == 2) { //unární operátory
            eval_expression(code, expr->next[0], result_field);
            if (expr->next[1]->data[1].data[0] == '#'){
                
            }
        }
    }
    else {
        t_tree_item*aux;
        aux = expr->data;
        expr->data = expr->next[0]->data;
        expr->next[0]->data = aux;

        node_delete(expr->next[0]);
    }
}

int find_label(code_t*code, char*label_id){
    char*label = malloc(strlen(label_id) + 7); //LABEL label_id'\0'
    strcpy(label, "LABEL ");
    strcat(label, label_id);
    char*label_location = strstr(code->text.data, label);
    free(label); //we don't need it anymore

    if (label_location == NULL)
        return -1;
    else{
        return label_location - code->text.data + strlen(label_id) + 7; //index of next char after \n of label
    } 
}

void free_memory_then_quit(int return_code) {
    
    node_delete(global_tree->first);
    buffer_destroy(global_code_buffer);
    exit(return_code);
}

int main(){
    MAKE_TREE();

    t_tree tree;
    tree.first = &node1;
    tree.active = &node1;

    code_t code;
    buffer_init(&code.text);
    code.total_conditionals_count = 0;
    code.total_expr_count = 0;

    generate_code(&tree, &code);

    printf("%s", code.text.data);

    node_delete(&node1);
    buffer_destroy(&code.text);
    return 0;
}