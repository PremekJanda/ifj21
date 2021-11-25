/**
 * @file code-generator.c
 * @authors Radomír Bábek - xbabek02, VUT FIT
 * @brief Definice funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	25. 11. 2021 21:20:50
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
#include "hashtable.h"
#include "test.h"

//globální proměnné, slouží k uvolnění paměti a ukončení programu
t_node*global_tree;
buffer_t*global_code_buffer;

void generate_code(t_node*tree, code_t*code){
    //nastavení globálních proměnných
    global_tree = tree;
    global_code_buffer = &code->text;

    strcpy(code->text.data, ".IFJcode21\n\n");
    strcat_realloc(&code->text, "# -- global variables for type checks\nDEFVAR GF@TYPE_var_1\nDEFVAR GF@TYPE_var_2\n\n");
    strcat_realloc(&code->text, "# -- result variable for conversion\nDEFVAR GF@CONVERTION_var\n\n");
    strcat_realloc(&code->text, "# -- operand variables for expression evaluation\nDEFVAR GF@OP_EXPR1\nDEFVAR GF@OP_EXPR2\n");
    strcat_realloc(&code->text, "# -- result variable for comparison\nDEFVAR GF@COMP_RES\nDEFVAR GF@COMP_RES2\n");

    strcat_realloc(&code->text, "\n\n# -- BUILT IN FUNCTIONS\n");

    if (append_file(&code->text, "built_in.fc") != 0){
        fprintf(stderr,"Nenalezen soubor built_in.fc");
        free_memory_then_quit(99);
    }
    
    //změna názvů identifikátorů ve stromě na unikátní identifikátory
    ht_table_t ht_already_processed;
    ht_init(&ht_already_processed);

    fix_expr(tree);
    convert_strings(tree);

    int depth = 0;
    int depth_total = 0;

    char*fc = NULL;
    tree_print(*tree, 0);
    rename_all_id(tree, &ht_already_processed, &fc, &depth, &depth_total);
    tree_print(*tree, 0);

    ht_delete_all(&ht_already_processed);

    //current_node = <prog>
    t_node*current_node = tree;

    if (strcmp(tree->data[0].data, "<prog>") == 0){
        current_node = check_prog_node(current_node); //current_node = <main-list>
    }
    else {
        free_memory_then_quit(2);
    }
    if (current_node == NULL){
        free_memory_then_quit(2);
    }
    
     //current_node = <def-declare-fcall> || eps, cyklus projde přes všechny prvky v rekurzivním listu
    for (; current_node->next[0] != NULL; current_node = current_node->next[1]){
        def_declare_fcall_crossroad(code, current_node->next[0]);
    }

    printf("%s", code->text.data);
}

void fix_expr(t_node*tree) {
    if (tree == NULL){
        return;
    }
    if (tree->next == NULL){
        return;
    }
    for (int i = 0; i < tree->next_count; i++) {
        if (tree->next[i] == NULL) {
            return;
        }

        if (strcmp(tree->next[i]->data[0].data, "expr") == 0) {
            if ((strcmp(tree->next[i]->next[0]->data[0].data, "id") == 0) && tree->next_count == 1){
                t_node*node = tree->next[i];
                
                tree->next[i] = tree->next[i]->next[0];
                tree->next[i]->prev = node->prev;
                tree->next[i]->next_count = 0;
                free(node);
            }
        }
        fix_expr(tree->next[i]);
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
                if (strcmp(tree->prev->data[0].data, "<stmt>") == 0){
                    if (strcmp(tree->prev->next[1]->next[0]->data[0].data, "(") == 0) {
                        strcat_format_realloc(&new_id, "%s_fc", id);
                    }
                }
                else {
                    strcat_format_realloc(&new_id, "%s_%s_depth%d_dt%d", id, *fc, *depth, *depth_total);
                }
            }

            END_IF_FAIL((&new_id));
            t_node*ref_node = tree;
            node_setdata(tree, new_id.data, 1); //mění první
            while (strcmp(ref_node->data[0].data, "<main-list>") != 0 && strcmp(ref_node->data[0].data, "<stmt-list>") != 0){
                ref_node = ref_node->prev;
            } 
            field_of_visibility_id_replacement(id, new_id.data, ref_node);
            ht_insert(ht_already_processed, new_id.data, 1);
            buffer_destroy(&new_id);
            free(id);
        }
    }
    else if ((strcmp(tree->data[1].data, "do") == 0) || (strcmp(tree->data[1].data, "then") == 0)){
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
                if (strcmp(aux_node->next[0]->data[0].data, "eps") != 0) {                    
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
                if (strcmp(aux_node->data[0].data, "<stmt-list>") == 0) {
                    //aux_node == <def-decl-fcall> || <while> || <if>
                    aux_node = aux_node->prev->prev->prev->next[1]; //will be in outer <stmt-list> or <main-list>
                }
                else{
                    //just for debug info
                    exit(99999);
                }
                field_of_visibility_id_replacement(id, new_id, aux_node);
                return;
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
    if(strcmp(prog_node->next[1]->data[0].data, "\"ifj21\"") != 0) {
        return NULL;
    }
    if(strcmp(prog_node->next[2]->data[0].data, "<main-list>") != 0) {
        return NULL;
    }
    return prog_node->next[2];
}

void def_declare_fcall_crossroad(code_t*code, t_node*main_node){ //main_node = <def-declare-fcall>
    if (strcmp(main_node->next[0]->data[1].data, "global") == 0) {
        const char*id = main_node->next[1]->data[1].data; //id = id.attribute
        create_global_variable(code, main_node->next[3], id); 
    }
    else if (strcmp(main_node->next[0]->data[1].data, "function") == 0) {
        strcat_format_realloc(&code->text, "\n#------------------------------\nJUMP END_%s\n", main_node->next[1]->data[1].data);
        function_gen(code, main_node);
        strcat_format_realloc(&code->text, "\n#------------------------------\nLABEL END_%s\n", main_node->next[1]->data[1].data);
    }
    else{
        function_call_gen(code, main_node);
    }
}

void function_call_gen(code_t*code, t_node*fcall_node){
    strcat_format_realloc(&code->text,"\n# -- call of %s\nCREATEFRAME\n", fcall_node->next[0]->data[1].data);

    //průchod item-listem
    bool first_went_through = true; 
    int index = 0;
    buffer_t temp; //na vytvoření názvu návratové proměnné
    buffer_init(&temp);
    for (t_node*items = fcall_node->next[1]->next[1]->next[0]; strcmp(items->data[0].data, "eps") != 0; items = items->next[index+1])
    {   
        int var_id = 1;
        temp.data[0] = '\0';
        
        strcat_format_realloc(&temp, "%%%d",var_id++);
        strcat_format_realloc(&code->text, "DEFVAR TF@%s\n",temp.data);
        move_item_to_var(code, "TF", temp.data, items->next[index]);
        
        if (first_went_through) {
            index++;
            first_went_through = false;
        }
    }
    
    strcat_format_realloc(&code->text, "CALL %s\n", fcall_node->next[0]->data[1].data);
    buffer_destroy(&temp);
}

void function_gen(code_t*code, t_node*function_node){
    strcat_format_realloc(&code->text, "\nLABEL %s\n", function_node->next[1]->data[1].data);
    END_IF_FAIL((&code->text));
    strcat_realloc(&code->text, "PUSHFRAME\n");

    int index = 0;
    
    //zpracování argumentů
    bool first_went_through = true; 
    for (t_node*args = function_node->next[3]; strcmp(args->next[0]->data[0].data, "eps") != 0; args = args->next[index])
    {   
        int var_id = 1;
        strcat_format_realloc(&code->text, "DEFVAR LF@%s\nMOV LF@%s LF@%%%d", args->next[index]->data[1].data, args->next[index]->data[1].data, var_id++);
        
        if (first_went_through) {
            index++;
            first_went_through = false;
        }
    }
    
    first_went_through = true;
    //zpracování návratových hodnot
    for (t_node*ret_vals = function_node->next[5]; strcmp(ret_vals->next[0]->data[0].data, "eps") != 0; ret_vals = ret_vals->next[2])
    {   
        int var_id = 1;
        strcat_format_realloc(&code->text, "DEFVAR LF@%%%dr\n", var_id++);
        
        if (first_went_through) {
            index++;
            first_went_through = false;
        }
    }
    
    strcat_realloc(&code->text, "\n\n");
    stmt_list_crossroad(code, function_node->next[6], function_node->next[1]->data[1].data);  

    strcat_realloc(&code->text, "POPFRAME\nRETURN\n");
}

void stmt_list_crossroad(code_t*code, t_node*stmt_list, char*fc_from){
    //rekurzivní podmínka
    if (stmt_list->next[0] == NULL) {
        if (strcmp(stmt_list->next[0]->data[0].data, "eps") == 0) {
            return;
        }
    }
    t_node*stmt = stmt_list->next[0];
    if (strcmp(stmt->next[0]->data[0].data, "id") == 0){
        if (strcmp(stmt->next[1]->next[0]->data[0].data, "(") == 0){
            function_call_gen(code, stmt);
        }
        else {
            generate_assignment(code, stmt);
        }
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<decl-local>") == 0) {
        generate_local_decl(code, stmt->next[0]);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<while>") == 0) {
        generate_while(code, stmt->next[0], fc_from);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<if>") == 0) {
        generate_if(code, stmt->next[0], fc_from);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<return>") == 0) {
        generate_return(code, stmt->next[0]);
    }

    //rekurzivní volání
    stmt_list_crossroad(code, stmt_list->next[1], fc_from);
}

void generate_local_decl(code_t*code, t_node*local_dec){
    
    
    strcat_format_realloc(&code->text, "\n# -- local declaration of variable %s\nDEFVAR LF@%s\n",local_dec->next[1]->data[1].data, local_dec->next[1]->data[1].data);
    END_IF_FAIL((&code->text));
    char*type = local_dec->next[3]->next[0]->data[1].data; //for the type check later
    

    if (strcmp(local_dec->next[4]->data[0].data, "eps") != 0) {
        t_node*aux_node = local_dec->next[4]->next[1]; //<f_or_item>
        if (strcmp(aux_node->next[0]->data[0].data, "expr") == 0) {
            expr_results_t results; init_result_field(code, &results);
            eval_expression(code, aux_node->next[0], &results);
            strcat_format_realloc(&code->text, "MOVE LF@%s LF@%s\n", local_dec->next[1]->data[1].data, results.result_var_id[0]);
            END_IF_FAIL((&code->text));
        }
        else if (strcmp(aux_node->next[1]->next[0]->data[0].data, "eps") == 0) { //id copy
            char frame[3];
            if (is_global(code, aux_node[0].data[1].data) != NULL) {
                strcpy(frame, "GF");
            }
            else {
                strcpy(frame, "LF");
            }
            strcat_format_realloc(&code->text, "MOVE LF@%s %s@%s\n", local_dec->next[1]->data[1].data, frame, aux_node->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else {
            function_call_gen(code, aux_node);
            strcat_format_realloc(&code->text, "MOVE LF@%s LF@%%1r\n", local_dec->next[1]->data[1].data);
        }
    }
    
}

void generate_while(code_t*code, t_node*while_node, char*fc){
    buffer_t while_label; buffer_init(&while_label);
    buffer_t end_label; buffer_init(&end_label);

    strcat_format_realloc(&while_label, "WHILE%d_%s", code->total_conditionals_count, fc);
    strcat_format_realloc(&end_label, "END_WHILE%d_%s", code->total_conditionals_count, fc);

    code->total_conditionals_count++;

    predefine_vars_of_stmt_list(code, while_node->next[3]);
}

void predefine_vars_of_stmt_list(code_t*code, t_node*stmt_list){
    t_node*declaration_node;
    for (; strcmp(stmt_list->next[0]->data[0].data, "eps") != 0; stmt_list = stmt_list->next[1]) {
        if (strcmp(stmt_list->next[0]->next[0]->data[0].data, "<declare-local>") == 0){
            generate_local_decl(code, stmt_list->next[0]);
            
            t_node*aux = stmt_list->next[0]->next[0];
            t_node id; node_init(&id);
            node_setdata(&id, aux->next[1]->data[0].data, 1);
            node_setdata(&id, aux->next[0]->data[0].data, 0);
            node_addnext(stmt_list->next[0]->next[0], &id);

            t_node equals; node_init(&equals);
            node_setdata(&equals, "assign", 0);
            node_setdata(&equals, "=", 1);
            
            //dokončím po dodělání gramatiky
            
        }
    }
}

void generate_if(code_t*code, t_node*if_node, char*fc){
    buffer_t else_label; buffer_init(&else_label);
    buffer_t end_label; buffer_init(&end_label);

    strcat_format_realloc(&else_label, "ELSE%d_%s", code->total_conditionals_count, fc);
    strcat_format_realloc(&end_label, "ENDIF%d_%s", code->total_conditionals_count, fc);

    eval_condition(code, if_node->next[1]);
    //result of condition will be in the GF@COMP_RES
    code->total_conditionals_count++;
    
    strcat_format_realloc(&end_label, "JUMPIFNEQ %s GF@COMP_RES bool@true\n", else_label.data);
    stmt_list_crossroad(code, if_node->next[3], fc);
    strcat_format_realloc(&end_label, "JUMP %s\n", end_label.data);
    strcat_format_realloc(&end_label, "LABEL %s\n", else_label.data);
    stmt_list_crossroad(code, if_node->next[5], fc);
    strcat_format_realloc(&end_label, "LABEL %s\n", end_label.data);

    buffer_destroy(&else_label);
    buffer_destroy(&end_label);
}

void generate_assignment(code_t*code, t_node*assignment){
    char frame[3];
    frame[0] = '\0';
    if (is_global(code, assignment->next[0]->data[1].data) != NULL) {
        strcpy(frame, "GF");
    }
    else {
        strcpy(frame, "LF");
    }
    
    //pro lepší přehlednost práce s gramatikou
    t_node*id_list = assignment->next[1]->next[0]; //id_list = rest of the ids to assign to
    t_node*aux = assignment->next[1]->next[2]; //aux = <f-or-item-list>
    
    if (strcmp(aux->next[0]->data[0].data, "id") == 0){
        if (strcmp(aux->next[1]->next[0]->data[0].data, "(")){
            char retval[25]; retval[0] = '\0';
            int var_nb = 1;

            function_call_gen(code, aux);
            sprintf(retval, "%%%dr", var_nb++);
            strcat_format_realloc(&code->text, "\n# -- return values from fc\nMOVE %s@%s TF@%s\n", frame, assignment->next[0]->data[1].data, retval);
            if (strcmp(id_list->next[0]->data[0].data, "eps") != 0){
                do //assign other variables
                {
                    if (is_global(code, id_list->next[1]->data[1].data) != NULL) {
                        strcpy(frame, "GF");
                    }
                    else {
                        strcpy(frame, "LF");
                    }
        
                    sprintf(retval, "%%%dr", var_nb++);
                    strcat_format_realloc(&code->text, "MOVE %s@%s TF@%s\n", frame, id_list->next[1]->data[1].data, retval);
                    END_IF_FAIL((&code->text));
                    id_list = id_list->next[2];
                } while (strcmp(id_list->next[0]->data[0].data, "eps") != 0);

            }
            //následující sekce může pracovat stejně pro expr i id, proto return
            return;
        }
    } //if

    //expr and id
    
    t_node ref; node_init(&ref);
    node_addnext(&ref, aux->next[0]); //kvůli kompatibilitě s funkcí move_item_to_var
    move_item_to_var(code, frame, assignment->next[0]->data[1].data, &ref);

    if (strcmp(aux->next[0]->data[0].data, "expr") == 0){
        aux = aux->next[1];
    }
    else {
        aux = aux->next[1]->next[0];
    }

    //aux = <item-another>

    while (strcmp(id_list->next[0]->data[0].data, "eps") != 0) {
        if (is_global(code, id_list->next[1]->data[1].data) != NULL) {
            strcpy(frame, "GF");
        }
        else {
            strcpy(frame, "LF");
        }
        move_item_to_var(code, frame, id_list->next[1]->data[1].data, aux->next[1]);
        aux = aux->next[2];
        id_list = id_list->next[2];
    }
}

void eval_condition(code_t*code, t_node*condition) {
    expr_results_t result1; init_result_field(code, &result1);
    expr_results_t result2; init_result_field(code ,&result2);

    char*id[2];
    char frame[2][3];
    int op = 0; //nabyde hodnoty 2 při druhém průchodu cyklu
    for (int i = 0; i < 2; i++)
    {
        if (strcmp(condition->next[op]->next[0]->data[0].data, "id") == 0){
            id[i] = condition->next[0]->next[0]->data[1].data;
            if (is_global(code, condition->next[op]->next[0]->data[1].data) != NULL){
                strcpy(frame[i], "GF");
            }
            else {
                strcpy(frame[i], "LF");
            }
        }
        else {
            eval_expression(code, condition->next[op]->next[0], &result1);
            id[i] = result1.result_var_id[0];
            strcpy(frame[i], "LF");
        }
        op = 2;
    }

    t_node*cond_operator = condition->next[1];
    if (strcmp(cond_operator->data[1].data, "<") == 0){
        strcat_format_realloc(&code->text, "\n# -- comparison <\nLT GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
    }
    else if (strcmp(cond_operator->data[1].data, ">") == 0){
        strcat_format_realloc(&code->text, "\n# -- comparison >\nGT GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
    }
    else if (strcmp(cond_operator->data[1].data, "==") == 0){
        strcat_format_realloc(&code->text, "\n# -- comparison ==\nEQ GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
    }
    else if (strcmp(cond_operator->data[1].data, "~=") == 0){
        strcat_format_realloc(&code->text, "\n# -- comparison =~\nEQ GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "NOT GF@COMP_RES GF@COMP_RES\n");
        END_IF_FAIL((&code->text));
        
    }
    else if (strcmp(cond_operator->data[1].data, "<=") == 0){
        strcat_format_realloc(&code->text, "EQ GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "GT GF@COMP_RES2 %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "OR GF@COMP_RES GF@COMP_RES GF@COMP_RES2\n");
        END_IF_FAIL((&code->text));
    }
    else if (strcmp(cond_operator->data[1].data, ">=") == 0){
        strcat_format_realloc(&code->text, "EQ GF@COMP_RES %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "GT GF@COMP_RES2 %s@%s %s@%s\n", frame[0], id[0], frame[1], id[1]);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "OR GF@COMP_RES GF@COMP_RES GF@COMP_RES2\n");
        END_IF_FAIL((&code->text));
    }
}


void generate_return(code_t*code, t_node*return_node){
    int index = 0;
    
    //průchod item-listem
    bool first_went_through = true; 
    buffer_t temp; //na vytvoření názvu návratové proměnné
    buffer_init(&temp);
    for (t_node*items = return_node->next[1]; strcmp(items->next[0]->data[0].data, "eps") != 0; items = items->next[index+1])
    {   
        int var_id = 1;
        temp.data[0] = '\0';
        
        strcat_format_realloc(&temp, "%%%dr",var_id);
        move_item_to_var(code, "LF", temp.data, items->next[index]);
        
        if (first_went_through) {
            index++;
            first_went_through = false;
        }
    }
    buffer_destroy(&temp);
}

void move_item_to_var(code_t*code, char*dest_frame, const char*dest_id, t_node*item){

    if (strcmp(item->next[0]->data[0].data, "id") == 0){
        buffer_t buffer;
        buffer_init(&buffer);
        END_IF_FAIL((&buffer));
        strcat_format_realloc(&buffer, "DEFVAR GF@%s", dest_id);
        END_IF_FAIL((&buffer));
        if (strstr(code->text.data, buffer.data) != NULL) {
            strcat_format_realloc(&code->text, "MOVE %s@%s GF@%s", dest_frame, dest_id, item->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else {
            strcat_format_realloc(&code->text, "MOVE %s@%s LF@%s", dest_frame, dest_id, item->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        buffer_destroy(&buffer);
    }
    else {
        expr_results_t results;
        init_result_field(code, &results);

        eval_expression(code, item->next[0], &results);
        
        strcat_format_realloc(&code->text, "MOVE %s@%s LF@%s", dest_frame, dest_id ,results.result_var_id[1]);
        END_IF_FAIL((&code->text));
        results.results[0] = false;
    }
}

void init_result_field(code_t*code, expr_results_t*results) {
    for (int i = 0; i < 3; i++)
    {
        results->results[i] = false;
        results->result_var_id[i][0] = '\0';

        sprintf(results->result_var_id[i], "result%d_e%d", i+1, code->total_expr_count);
        strcat_format_realloc(&code->text, "DEFVAR LF@%s\n", results->result_var_id[i]);
    }
    code->total_expr_count++;
}

char*is_global(code_t*code, char*id){
    buffer_t buffer;
    buffer_init(&buffer);
    END_IF_FAIL((&buffer));
    strcat_format_realloc(&buffer, "DEFVAR GF@%s", id);
    END_IF_FAIL((&buffer));
    char* result;
    result = strstr(code->text.data, buffer.data);
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
            move_item_to_var(code, "GF", id, assignment_node->next[1]);
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
            strcpy_realloc(&buffer, tree->data[1].data);
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
            if (tree->next[i] != NULL){
                convert_strings(tree->next[i]);
            }
        }
    }
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
           results->results[i] = false; //false means free
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

                
                move_item_to_var(code, "GF", "OP_EXPR1", expr->next[0]);
                move_item_to_var(code, "GF", "OP_EXPR2", expr->next[2]);

                was_result_then_free_it(expr->next[0], result_field);
                was_result_then_free_it(expr->next[2], result_field);

                char*result = first_free_result(result_field);

                strcat_format_realloc(&code->text, "ADD LF@%s GF@OP_EXPR1 GF@OP_EXPR2", result);
                node_setdata(expr, "id", 0);
                node_setdata(expr, result, 1);
                node_delete(expr->next[0]);
                node_delete(expr->next[1]);
                node_delete(expr->next[2]);

                for (int i = 0; i < 3; i++) {
                    if (strcmp(result, result_field->result_var_id[i]) == 0) {
                        result_field->results[i] = true; //true means used
                    }
                }
                
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
    
    node_delete(global_tree);
    buffer_destroy(global_code_buffer);
    exit(return_code);
}

void init_code(code_t*code){
    buffer_init(&code->text);
    END_IF_FAIL((&code->text));
    code->total_conditionals_count = 0;
    code->total_expr_count = 0;
}
