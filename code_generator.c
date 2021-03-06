/**
 * @file code-generator.c
 * @authors Radomír Bábek - xbabek02, VUT FIT
 * @brief Definice funkcí pro generování kódu
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	08. 12. 2021 01:15:26
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "code_generator.h"
#include "tree.h"
#include "mystring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashtable.h"
#include "test.h"
#include "symtable.h"

// globální proměnné, slouží k uvolnění paměti a ukončení programu
t_node *global_tree;
buffer_t *global_code_buffer;
stack_t *global_sym_table;

void generate_code(t_node *tree, code_t *code, stack_t *sym_table)
{

    // FÁZE PŘÍPRAV

    // nastavení globálních proměnných
    global_tree = tree;
    global_code_buffer = &code->text;
    global_sym_table = sym_table;

    strcpy(code->text.data, ".IFJcode21\n\n");
    strcat_realloc(&code->text, "# -- global variables for type checks\nDEFVAR GF@TYPE_var_1\nDEFVAR GF@TYPE_var_2\n\n");
    strcat_realloc(&code->text, "# -- result variable for conversion\nDEFVAR GF@CONVERTION1\nDEFVAR GF@CONVERTION2\n\n");
    strcat_realloc(&code->text, "# -- result variable for expression evaluation\nDEFVAR GF@EXPR1\nDEFVAR GF@EXPR2\n\n");
    strcat_realloc(&code->text, "# -- result variable for comparison\nDEFVAR GF@COMP_RES\nDEFVAR GF@COMP_RES2\n\n");
    strcat_realloc(&code->text, "# -- result variable for IS_ARITHMETIC\nDEFVAR GF@IS_ARITH_RES");

    strcat_realloc(&code->text, "\n\n# -- BUILT IN FUNCTIONS\n");

    if (append_file(&code->text, "built_in.fc") != 0)
    {
        fprintf(stderr, "Nenalezen soubor built_in.fc");
        free_memory_then_quit(99);
    }

    // nahrazení expr -> id za id
    fix_expr(tree);

    // překonvertování escape sekvencí na
    convert_strings(tree);

    // hashovací tabulka použita při přejmenovávání identifikátorů
    // díky ní rozeznám, které identifikátory již byly přejmenované
    ht_table_t ht_already_processed;
    ht_init(&ht_already_processed);
    ht_insert(&ht_already_processed, "write", 1); // aby byla funkce write ignorována zpracováním identifikátorů

    int depth = 0;
    int depth_total = 0;

    buffer_t fc;
    buffer_init(&fc);

    // změna názvů identifikátorů ve stromě na unikátní identifikátory
    rename_all_id(tree, &ht_already_processed, &fc, &depth, &depth_total);
    buffer_destroy(&fc);
    ht_delete_all(&ht_already_processed);

    // překonverování write funkcí
    int convert_result = 0;
    do
    {
        convert_result = convert_write(tree);
    } while (convert_result != 0);

    // current_node = <prog>
    t_node *current_node = tree;

    if (strcmp(tree->data[0].data, "<prog>") == 0)
    {
        current_node = check_prog_node(current_node); // current_node = <main-list>
    }
    else
    {
        free_memory_then_quit(2);
    }
    if (current_node == NULL)
    {
        free_memory_then_quit(2);
    }

    // FÁZE GENEROVÁNÍ

    // current_node = <def-declare-fcall> || eps, cyklus projde přes všechny prvky v rekurzivním listu
    for (; current_node->next[0] != NULL; current_node = current_node->next[1])
    {
        def_declare_fcall_crossroad(code, current_node->next[0]);
    }
    printf("%s", code->text.data);
}

int convert_write(t_node *tree)
{
    if (tree == NULL)
    {
        return 0;
    }
    if (strcmp(tree->data[0].data, "<stmt>") == 0)
    {
        if (strcmp(tree->next[0]->data[1].data, "write") == 0)
        {
            int index = 0; // where the item_another is stored to be checked for eps
            t_node *insert_after = tree->prev;

            // count the arguments
            if (strcmp(tree->next[1]->next[1]->next[0]->data[0].data, "eps") == 0)
            {
                node_setdata(tree->next[0], "write_fc", 1);
            }
            t_node *new_nodes[12]; // new nodes to create a node with function call
            t_node *item_list = tree->next[1]->next[1]->next[0];
            for (int i = 0; strcmp(item_list->next[0]->data[0].data, "eps") != 0; i++)
            {
                for (int k = 0; k < 12; k++)
                {
                    new_nodes[k] = malloc(sizeof(t_node));
                    if (new_nodes[k] == NULL)
                    {
                        free_memory_then_quit(99);
                    }
                    node_init(new_nodes[k]);
                }
                // zinicializuje strom volání funkce, prázdný item je na indexu 8
                CREATE_NEW_WRITE_CALL(new_nodes);
                if (i == 1)
                { // podmínka pro průchod cyklem
                    index++;
                }

                new_nodes[8]->next[0] = item_list->next[index]->next[0];
                new_nodes[8]->next[0]->prev = &(*(new_nodes[8]));
                new_nodes[8]->next_count++;

                // musíme něco přiřadit do nahrazeného itemu, jinak na jeho místě NULL nebo nechtěná reference
                node_setdata(new_nodes[11], "eps", 0);
                node_setdata(new_nodes[11], "", 1);
                item_list->next[index]->next[0] = new_nodes[11];

                new_nodes[0]->next[1] = insert_after->next[1];
                new_nodes[0]->next[1]->prev = new_nodes[0];
                new_nodes[0]->next_count++;
                insert_after->next[1] = new_nodes[0];

                insert_after = new_nodes[0];
                item_list = item_list->next[index + 1];
            }
            // najití ukazatele na nezkonvertovaný write
            t_node **pointer_to_write_node = NULL;
            insert_after = tree->prev;
            for (int k = 0; k < insert_after->prev->next_count; k++)
            {
                if (insert_after == insert_after->prev->next[k])
                {
                    pointer_to_write_node = &insert_after->prev->next[k];
                }
            }
            if (pointer_to_write_node != NULL)
            {
                // nahrazení přechůdce volání funkce
                *pointer_to_write_node = tree->prev->next[1];
                tree->next[1]->prev = *pointer_to_write_node;
            }

            tree->prev->next[1] = NULL;
            tree->prev->next_count = 1;

            tree_delete(tree->prev);
            return 1;
        }
    }
    for (int i = 0; i < tree->next_count; i++)
    {
        // ukončení rekurzivního volání, pokud již byla nalezena a přetransformována write funkce
        if (convert_write(tree->next[i]) == 1)
        {
            return 1;
        }
    }
    return 0;
}

void fix_expr(t_node *tree)
{
    if (tree == NULL)
    {
        return;
    }
    if (tree->next == NULL)
    {
        return;
    }
    for (int i = 0; i < tree->next_count; i++)
    {
        if (tree->next[i] == NULL)
        {
            return;
        }

        if (strcmp(tree->next[i]->data[0].data, "expr") == 0)
        {
            if ((strcmp(tree->next[i]->next[0]->data[0].data, "id") == 0) && tree->next[i]->next[1] == NULL)
            {
                t_node *node = tree->next[i];

                tree->next[i] = tree->next[i]->next[0];
                tree->next[i]->prev = &(*tree);
                tree->next[i]->next_count = 0;

                node->next[0] = NULL;
                node->next_count = 0;
                tree_delete(node);
            }
            return;
        }
        fix_expr(tree->next[i]);
    }
}

void rename_all_id(t_node *tree, ht_table_t *ht_already_processed, buffer_t *fc, int *depth, int *depth_total)
{
    if (strcmp(tree->data[0].data, "id") == 0)
    {
        char *id = malloc(strlen(tree->data[1].data) + 1);
        if (id == NULL)
        {
            free_memory_then_quit(99);
        }
        strcpy(id, tree->data[1].data);

        buffer_t new_id;
        buffer_init(&new_id);
        bool param = false;
        if (ht_search(ht_already_processed, id) == NULL)
        {
            if (strcmp(fc->data, "") == 0)
            { // v globálním prostoru
                if (strcmp(tree->prev->next[0]->data[1].data, "function") == 0)
                {
                    strcpy_realloc(fc, tree->prev->next[1]->data[1].data);
                    strcat_format_realloc(&new_id, "%s_fc", id);
                    (*depth)++;
                    (*depth_total)++;
                }
                else if (strcmp(tree->prev->next[0]->data[1].data, "global") == 0)
                {
                    if (strcmp(tree->prev->next[3]->next[0]->data[1].data, "function") == 0)
                    {
                        strcat_format_realloc(&new_id, "%s_fc", id);
                    }
                    else
                    {
                        strcat_format_realloc(&new_id, "%s_global", id);
                    }
                }
            }
            else
            {
                if (strcmp(tree->prev->data[0].data, "<stmt>") == 0)
                {
                    if (strcmp(tree->prev->next[1]->next[0]->data[0].data, "(") == 0)
                    {
                        strcat_format_realloc(&new_id, "%s_fc", id);
                    }
                }
                else if (strcmp(tree->prev->data[0].data, "<f-or-item-list>") == 0 || strcmp(tree->prev->data[0].data, "<f-or-item>") == 0)
                {
                    if (tree->prev->next[1] != NULL)
                    {
                        if (strcmp(tree->prev->next[1]->next[0]->data[0].data, "(") == 0)
                        {
                            strcat_format_realloc(&new_id, "%s_fc", id);
                        }
                    }
                }
                else
                {
                    if (strcmp(tree->prev->data[0].data, "<f-arg>") == 0)
                    {
                        strcat_format_realloc(&new_id, "%s_param_%s", id, fc->data);
                        param = true;
                    }
                    else
                    {
                        strcat_format_realloc(&new_id, "%s_%s_depth%d_dt%d", id, fc->data, *depth, *depth_total);
                    }
                }
            }

            END_IF_FAIL((&new_id));
            t_node *ref_node = tree;
            node_setdata(tree, new_id.data, 1); // mění první
            if (!param)
            { // pokud se nejedná o parametr
                while (strcmp(ref_node->data[0].data, "<main-list>") != 0 && strcmp(ref_node->data[0].data, "<stmt-list>") != 0)
                {
                    ref_node = ref_node->prev;
                }
            }
            else
            {
                // přejde do statement-listu
                while (ref_node->next[6] == NULL)
                {
                    ref_node = ref_node->prev;
                }
                ref_node = ref_node->next[6];
            }

            field_of_visibility_id_replacement(id, new_id.data, ref_node);
            ht_insert(ht_already_processed, new_id.data, 1);

        } // ht_search == NULL, nutná úprava pro případ předchozí deklarace funkce
        else if (tree->prev != NULL)
        {
            if (tree->prev->next[0] != NULL)
            {
                if (strcmp(tree->prev->next[0]->data[1].data, "function") == 0)
                {
                    strcpy_realloc(fc, tree->prev->next[1]->data[1].data);
                    (*depth)++;
                    (*depth_total)++;
                }
            }
        }

        buffer_destroy(&new_id);
        free(id);
    }
    // zanoření do whilu
    else if ((strcmp(tree->data[1].data, "do") == 0) || (strcmp(tree->data[1].data, "then") == 0))
    {
        (*depth)++;
        (*depth_total)++;
    }
    // zanoření do elsu, přičteme dosavadní počet zanoření
    else if ((strcmp(tree->data[1].data, "else") == 0))
    {
        (*depth_total)++;
    }
    // vynoření se z oboru platnosti, případné vynoření z funkce do globálního prostoru
    else if (strcmp(tree->data[1].data, "end") == 0)
    {
        (*depth)--;
        if (*depth == 0)
        {
            strcpy(fc->data, "");
        }
    }
    if (tree->next_count != 0)
    {
        for (int i = 0; i < tree->next_count; i++)
        {
            rename_all_id(tree->next[i], ht_already_processed, fc, depth, depth_total);
        }
    }
}

int field_of_visibility_id_replacement(char *id, char *new_id, t_node *tree)
{
    if (strcmp(tree->data[0].data, "<stmt>") == 0)
    {                                     // need to check for
        t_node *aux_node = tree->next[0]; // into the statement
        if (strcmp(aux_node->data[0].data, "<decl-local>") == 0)
        {
            if (strcmp(aux_node->next[1]->data[1].data, id) == 0)
            {
                aux_node = aux_node->next[4]; // aux node = <decl-assign>
                if (strcmp(aux_node->next[0]->data[0].data, "eps") != 0)
                {
                    if (strcmp(aux_node->next[1]->next[0]->data[0].data, "id") == 0)
                    {
                        if (strcmp(aux_node->next[1]->next[0]->data[1].data, id) == 0)
                        {
                            /* nahraď poslední identifikátor tímto jménem, všechny další identifikátory
                                budou znamenat novou proměnnou až do konce ifu nebo whilu */
                            node_setdata(aux_node->next[1]->next[0], new_id, 1);
                        }
                    }
                }
                // najdeme obor platnosti
                if (strcmp("_global", new_id + strlen(new_id) + 1 - 8) == 0)
                    while (strcmp(aux_node->data[0].data, "<main-list>") != 0)
                    { // globální zastínění
                        aux_node = aux_node->prev;
                    }
                else
                {
                    while (strcmp(aux_node->data[0].data, "<stmt-list>") != 0)
                    { // locální zastínění
                        aux_node = aux_node->prev;
                    }
                }
                if (strcmp(aux_node->data[0].data, "<stmt-list>") == 0)
                {
                    // aux_node == <def-decl-fcall> || <while> || <if>
                    aux_node = aux_node->prev->prev->prev->next[1]; // vnější <stmt-list>
                }
                else if (strcmp(aux_node->data[0].data, "<main-list>") == 0)
                {
                    aux_node = aux_node->next[1];
                }
                field_of_visibility_id_replacement(id, new_id, aux_node);
                return -1;
            }
        }
    }

    else if (strcmp(tree->data[0].data, "id") == 0)
    {
        if (strcmp(tree->data[1].data, id) == 0)
        {
            if (strcmp(tree->prev->data[0].data, "<f-arg>") == 0)
            {
                while (strcmp(tree->data[0].data, "<main-list>") != 0)
                { // global foreshadowing
                    tree = tree->prev;
                }
                tree = tree->next[1];
                field_of_visibility_id_replacement(id, new_id, tree);
                return -1;
            }
            tree->data[1].data = realloc(tree->data[1].data, strlen(new_id) + 1);
            if (tree->data[1].data == NULL)
            {
                free_memory_then_quit(99);
            }
            strcpy(tree->data[1].data, new_id);
        }
    }
    int result = 0;
    for (int i = 0; i < tree->next_count; i++)
    {
        if (tree->next[i] != NULL)
        {
            result = field_of_visibility_id_replacement(id, new_id, tree->next[i]);
            if (result == -1)
            {
                return -1;
            }
        }
    }
    return 0;
}

t_node *check_prog_node(t_node *prog_node)
{
    if (strcmp(prog_node->next[0]->data[1].data, "require") != 0)
    {
        return NULL;
    }
    if (strcmp(prog_node->next[1]->data[1].data, "\"ifj21\"") != 0)
    {
        return NULL;
    }
    if (strcmp(prog_node->next[2]->data[0].data, "<main-list>") != 0)
    {
        return NULL;
    }
    return prog_node->next[2];
}

void def_declare_fcall_crossroad(code_t *code, t_node *main_node)
{ // main_node = <def-declare-fcall>
    if (strcmp(main_node->next[0]->data[1].data, "global") == 0)
    {
        const char *id = main_node->next[1]->data[1].data; // id = id.attribute
        create_global_variable(code, main_node->next[3], id);
    }
    else if (strcmp(main_node->next[0]->data[1].data, "function") == 0)
    {
        strcat_format_realloc(&code->text, "\n#------------------------------\nJUMP END_%s\n", main_node->next[1]->data[1].data);
        END_IF_FAIL((&code->text));
        function_gen(code, main_node);
        strcat_format_realloc(&code->text, "\n#------------------------------\nLABEL END_%s\n", main_node->next[1]->data[1].data);
        END_IF_FAIL((&code->text));
    }
    else
    {
        function_call_gen(code, main_node, true);
    }
}

void function_call_gen(code_t *code, t_node *fcall_node, bool createframe)
{
    strcat_format_realloc(&code->text, "\n# -- call of %s\n", fcall_node->next[0]->data[1].data);
    END_IF_FAIL((&code->text));

    // má funkce vyvářet dočasný rámec a definovat návratové proměnné?
    // přiřazní to totiž dělá samo podle počtu identifikátorů
    if (createframe)
    {
        strcat_realloc(&code->text, "CREATEFRAME\n");
        END_IF_FAIL((&code->text));

        // definice byť nepoužitých, avšak potřebných návratových proměnných

        char *function_bare = malloc(strlen(fcall_node->next[0]->data[1].data) + 1);
        strcpy(function_bare, fcall_node->next[0]->data[1].data);
        function_bare[strlen(function_bare) - 3] = '\0';

        int return_values = 0;
        get_f_return_count(function_bare, global_sym_table, &return_values);

        for (int i = 1; i < return_values + 1; i++)
        {
            strcat_format_realloc(&code->text, "DEFVAR TF@%%%dr\nMOVE TF@%%%dr nil@nil\n", i, i);
            END_IF_FAIL((&code->text));
        }
        free(function_bare);
    }

    // průchod item-listem
    int index = 0;
    int i = 0;
    buffer_t temp; // na vytvoření názvu návratové proměnné
    buffer_init(&temp);
    int var_id = 1;

    t_node *items;
    if (strcmp(fcall_node->data[0].data, "<def-decl-fcall>") == 0)
    {
        items = fcall_node->next[2]->next[0];
    }
    else
    {
        items = fcall_node->next[1]->next[1]->next[0];
    }
    if (strcmp(items->data[0].data, "eps") != 0)
    {
        for (; strcmp(items->next[0]->data[0].data, "eps") != 0; items = items->next[index + 1])
        {
            // podmínka pro průchod item-listem dle gramatiky
            if (++i == 2)
            {
                index++;
            }

            temp.data[0] = '\0';
            strcat_format_realloc(&temp, "%%%d", var_id++);
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "DEFVAR TF@%s\n", temp.data);
            END_IF_FAIL((&code->text));
            move_item_to_var(code, "TF", temp.data, items->next[index]);
        }
    }

    // volání funkce
    strcat_format_realloc(&code->text, "CALL %s\n", fcall_node->next[0]->data[1].data);
    buffer_destroy(&temp);
}

void function_gen(code_t *code, t_node *function_node)
{
    strcat_format_realloc(&code->text, "\nLABEL %s\n", function_node->next[1]->data[1].data);
    END_IF_FAIL((&code->text));
    strcat_realloc(&code->text, "PUSHFRAME\n");
    END_IF_FAIL((&code->text));

    int i = 0;
    int index = 0;

    // zpracování argumentů
    int var_id = 1;
    for (t_node *args = function_node->next[3]; strcmp(args->next[0]->data[0].data, "eps") != 0; args = args->next[index + 1])
    {
        // podmínka zaručující průchod listem podle gramatiky
        if (++i == 2)
        {
            index++;
        }

        strcat_format_realloc(&code->text, "DEFVAR LF@%s\nMOVE LF@%s LF@%%%d\n", args->next[index]->next[0]->data[1].data, args->next[index]->next[0]->data[1].data, var_id++);
        END_IF_FAIL((&code->text));
    }

    // zpracování příkázů funkce
    stmt_list_crossroad(code, function_node->next[6], function_node->next[1]->data[1].data);

    strcat_realloc(&code->text, "POPFRAME\nRETURN\n");
    END_IF_FAIL((&code->text));
}

void stmt_list_crossroad(code_t *code, t_node *stmt_list, char *fc_from)
{
    // rekurzivní podmínky pro ukončení průchodu statement listem
    if (stmt_list->next[0] == NULL)
    {
        return;
    }
    if (strcmp(stmt_list->next[0]->data[0].data, "eps") == 0)
    {
        return;
    }

    // identifikace typu statementu a následná generace konkrétního typu kódu
    t_node *stmt = stmt_list->next[0];
    if (strcmp(stmt->next[0]->data[0].data, "id") == 0)
    {
        if (strcmp(stmt->next[1]->next[0]->data[0].data, "(") == 0)
        {
            function_call_gen(code, stmt, true);
        }
        else
        {
            generate_assignment(code, stmt);
        }
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<decl-local>") == 0)
    {
        generate_local_decl(code, stmt->next[0]);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<while>") == 0)
    {
        generate_while(code, stmt->next[0], fc_from);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<if>") == 0)
    {
        generate_if(code, stmt->next[0], fc_from);
    }
    else if (strcmp(stmt->next[0]->data[0].data, "<return>") == 0)
    {
        generate_return(code, stmt->next[0]);
    }

    // rekurzivní volání
    stmt_list_crossroad(code, stmt_list->next[1], fc_from);
}

void generate_local_decl(code_t *code, t_node *local_dec)
{
    char *id = local_dec->next[1]->data[1].data;
    strcat_format_realloc(&code->text, "\n# -- local declaration of variable %s\nDEFVAR LF@%s\nMOVE LF@%s nil@nil\n\n", id, id, id);
    END_IF_FAIL((&code->text));

    if (strcmp(local_dec->next[4]->next[0]->data[0].data, "eps") != 0)
    {
        t_node *aux_node = local_dec->next[4]->next[1]; //<f_or_item>
        if (strcmp(aux_node->next[0]->data[0].data, "expr") == 0)
        {
            // vyčíslení expressionu a přesunutí do proměnné
            eval_expression(code, aux_node->next[0]);
            strcat_format_realloc(&code->text, "POPS LF@%s\n", local_dec->next[1]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else if (aux_node->next[1] != NULL)
        {
            // volání funkce a vložení návratové hodnoty do proměnné
            strcat_format_realloc(&code->text, "CREATEFRAME\n DEFVAR TF@%%1r\nMOVE TF@%%1r nil@nil");
            END_IF_FAIL((&code->text));
            function_call_gen(code, aux_node, false);
            strcat_format_realloc(&code->text, "MOVE LF@%s TF@%%1r\n", local_dec->next[1]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else
        { // vložení hodnoty identifikátoru do proměnné
            char frame[3];
            if (is_global(aux_node[0].data[1].data))
            {
                strcpy(frame, "GF");
            }
            else
            {
                strcpy(frame, "LF");
            }
            strcat_format_realloc(&code->text, "MOVE LF@%s %s@%s\n", local_dec->next[1]->data[1].data, frame, aux_node->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
    }
}

void generate_while(code_t *code, t_node *while_node, char *fc)
{
    buffer_t while_label;
    buffer_init(&while_label);
    buffer_t end_label;
    buffer_init(&end_label);

    // generace názvů návěští
    strcat_format_realloc(&while_label, "WHILE%d_%s", code->total_conditionals_count, fc);
    strcat_format_realloc(&end_label, "END_WHILE%d_%s", code->total_conditionals_count, fc);

    code->total_conditionals_count++;

    // předsunutí deklarací proměnných uvnitř whilu
    predefine_vars_of_stmt_list(code, while_node->next[3]);

    // vyčíslení podmínky
    strcat_format_realloc(&code->text, "\n # --- while start\nLABEL %s\n", while_label.data);
    eval_condition(code, while_node->next[1]);
    strcat_format_realloc(&code->text, "JUMPIFNEQ %s GF@COMP_RES bool@true\n", end_label.data);

    // generování těla whilu
    stmt_list_crossroad(code, while_node->next[3], fc);

    strcat_format_realloc(&code->text, "\n\nJUMP %s\nLABEL %s\n", while_label.data, end_label.data);

    buffer_destroy(&while_label);
    buffer_destroy(&end_label);
}

void predefine_vars_of_stmt_list(code_t *code, t_node *stmt_list)
{
    t_node *new_nodes[NEW_NODES_FOR_ASSIGNMENT];
    bool already_shifted = false; // někdy je potřeba se posunout ve statement listu ještě před začátkem nového cyklu

    for (; strcmp(stmt_list->next[0]->data[0].data, "eps") != 0; already_shifted ? stmt_list = stmt_list : (stmt_list = stmt_list->next[1]))
    {
        already_shifted = false;
        if (strcmp(stmt_list->next[0]->next[0]->data[0].data, "<decl-local>") == 0)
        {
            generate_local_decl(code, stmt_list->next[0]->next[0]);
            if (strcmp(stmt_list->next[0]->next[0]->next[4]->data[0].data, "eps") == 0)
            {
                // najití ukazatele na stmt-list, který se chystám odstranit
                t_node **pointer_to_stmt_list = NULL;
                t_node *insert_after = stmt_list->prev;
                for (int k = 0; k < insert_after->prev->next_count; k++)
                {
                    if (insert_after == insert_after->prev->next[k])
                    {
                        pointer_to_stmt_list = &insert_after->prev->next[k];
                    }
                }
                // nahrazení přechůdce
                if (pointer_to_stmt_list != NULL)
                {
                    *pointer_to_stmt_list = stmt_list->next[1];
                    stmt_list->next[1]->prev = *pointer_to_stmt_list;
                }
                t_node *aux = stmt_list;
                stmt_list = stmt_list->next[1];
                already_shifted = true;
                tree_delete(aux);
            }

            else
            {
                for (int k = 0; k < NEW_NODES_FOR_ASSIGNMENT; k++)
                {
                    new_nodes[k] = malloc(sizeof(t_node));
                    if (new_nodes[k] == NULL)
                    {
                        free_memory_then_quit(99);
                    }
                    node_init(new_nodes[k]);
                }

                // vytvořění simulace stromu přiřazení na místě lokální deklarace
                // po dosažení f-or-item-list se větví na více případů podle typu přiřazení
                node_setdata(new_nodes[0], stmt_list->next[0]->next[0]->next[1]->data[0].data, 0);
                node_setdata(new_nodes[0], stmt_list->next[0]->next[0]->next[1]->data[1].data, 1);

                node_setdata(new_nodes[1], "<assign-or-fcall>", 0);
                node_setdata(new_nodes[1], "", 1);

                node_setdata(new_nodes[2], "<id-list>", 0);
                node_setdata(new_nodes[2], "", 1);
                node_addnext(new_nodes[1], new_nodes[2]);

                node_setdata(new_nodes[3], "eps", 0);
                node_setdata(new_nodes[3], "", 1);
                node_addnext(new_nodes[2], new_nodes[3]);

                node_setdata(new_nodes[4], "assign", 0);
                node_setdata(new_nodes[4], "=", 1);
                node_addnext(new_nodes[1], new_nodes[4]);

                node_setdata(new_nodes[5], "<f-or-item-list>", 0);
                node_setdata(new_nodes[5], "", 1);
                node_addnext(new_nodes[1], new_nodes[5]);

                //<f-or-item> == expr
                t_node *f_or_item = stmt_list->next[0]->next[0]->next[4]->next[1];
                if (strcmp(f_or_item->next[0]->data[0].data, "expr") == 0)
                {
                    t_node *expr_assignment_nodes[3];
                    for (int n = 0; n < 3; n++)
                    {
                        expr_assignment_nodes[n] = malloc(sizeof(t_node));
                        if (expr_assignment_nodes[n] == NULL)
                        {
                            free_memory_then_quit(99);
                        }
                        node_init(expr_assignment_nodes[n]);
                    }
                    // ukradení expr z lokální deklarace
                    free(new_nodes[5]->next[0]);
                    new_nodes[5]->next[0] = f_or_item->next[0];
                    new_nodes[5]->next[0]->prev = &(*new_nodes[5]);
                    new_nodes[5]->next_count++;

                    // zaplnění uzlu, z kama jsem ukradl expr
                    node_setdata(expr_assignment_nodes[0], "", 0);
                    node_setdata(expr_assignment_nodes[0], "", 1);
                    f_or_item->next[0] = expr_assignment_nodes[0];
                    f_or_item->next[0]->prev = &(*f_or_item->next[0]);

                    node_setdata(expr_assignment_nodes[1], "<item-another>", 0);
                    node_setdata(expr_assignment_nodes[1], "", 1);
                    node_addnext(new_nodes[5], expr_assignment_nodes[1]);

                    node_setdata(expr_assignment_nodes[2], "eps", 0);
                    node_setdata(expr_assignment_nodes[2], "", 1);
                    node_addnext(expr_assignment_nodes[1], expr_assignment_nodes[2]);
                }
                else if (strcmp(f_or_item->next[0]->data[0].data, "id") == 0 && f_or_item->next[1] == NULL)
                { // přiřazení identifikátoru
                    t_node *id_assign_nodes[4];
                    for (int n = 0; n < 4; n++)
                    {
                        id_assign_nodes[n] = malloc(sizeof(t_node));
                        if (id_assign_nodes[n] == NULL)
                        {
                            free_memory_then_quit(99);
                        }
                        node_init(id_assign_nodes[n]);
                    }

                    node_setdata(id_assign_nodes[0], f_or_item->next[0]->data[0].data, 0);
                    node_setdata(id_assign_nodes[0], f_or_item->next[0]->data[1].data, 1);
                    node_addnext(new_nodes[5], id_assign_nodes[0]);

                    node_setdata(id_assign_nodes[1], "<f-or-item-list>", 0);
                    node_setdata(id_assign_nodes[1], "", 0);
                    node_addnext(new_nodes[5], id_assign_nodes[1]);

                    node_setdata(id_assign_nodes[2], "<item-another>", 0);
                    node_setdata(id_assign_nodes[2], "", 0);
                    node_addnext(id_assign_nodes[1], id_assign_nodes[2]);

                    node_setdata(id_assign_nodes[3], "eps", 0);
                    node_setdata(id_assign_nodes[3], "", 0);
                    node_addnext(id_assign_nodes[2], id_assign_nodes[3]);
                }
                else
                { // přiřazení výsledku funkce
                    t_node *fc_assign_nodes[2];
                    for (int n = 0; n < 2; n++)
                    {
                        fc_assign_nodes[n] = malloc(sizeof(t_node));
                        if (fc_assign_nodes[n] == NULL)
                        {
                            free_memory_then_quit(99);
                        }
                        node_init(fc_assign_nodes[n]);
                    }

                    node_setdata(fc_assign_nodes[0], f_or_item->next[0]->data[0].data, 0);
                    node_setdata(fc_assign_nodes[0], f_or_item->next[0]->data[1].data, 1);
                    node_addnext(new_nodes[5], fc_assign_nodes[0]);

                    // ukradení parametrů funkce
                    free(new_nodes[5]->next[1]);
                    new_nodes[5]->next[1] = f_or_item->next[1];
                    new_nodes[5]->next_count++;
                    new_nodes[5]->next[1]->prev = &(*new_nodes[5]);

                    node_setdata(f_or_item->next[1], "<fcall-or-item-list>", 0);
                    node_setdata(f_or_item->next[1], "", 0);

                    node_setdata(fc_assign_nodes[1], "", 0);
                    node_setdata(fc_assign_nodes[1], "", 1);
                    f_or_item->next[1] = fc_assign_nodes[1];
                    f_or_item->next[1]->prev = &(*f_or_item);
                }
                tree_delete(stmt_list->next[0]->next[0]);
                stmt_list->next[0]->next[0] = &(*new_nodes[0]);
                stmt_list->next[0]->next[1] = &(*new_nodes[1]);
                stmt_list->next[0]->next[0]->prev = stmt_list->next[0];
                stmt_list->next[0]->next[1]->prev = stmt_list->next[0];

                stmt_list->next[0]->next_count = 2;
            }
        }
    }
}

void generate_if(code_t *code, t_node *if_node, char *fc)
{
    buffer_t else_label;
    buffer_init(&else_label);
    buffer_t end_label;
    buffer_init(&end_label);

    strcat_format_realloc(&else_label, "ELSE%d_%s", code->total_conditionals_count, fc);
    strcat_format_realloc(&end_label, "ENDIF%d_%s", code->total_conditionals_count, fc);

    eval_condition(code, if_node->next[1]);
    // result of condition will be in the GF@COMP_RES
    code->total_conditionals_count++;

    strcat_format_realloc(&code->text, "JUMPIFNEQ %s GF@COMP_RES bool@true\n", else_label.data);
    stmt_list_crossroad(code, if_node->next[3], fc);
    strcat_format_realloc(&code->text, "JUMP %s\n", end_label.data);
    strcat_format_realloc(&code->text, "LABEL %s\n", else_label.data);
    stmt_list_crossroad(code, if_node->next[5], fc);
    strcat_format_realloc(&code->text, "LABEL %s\n", end_label.data);

    buffer_destroy(&else_label);
    buffer_destroy(&end_label);
}

void generate_assignment(code_t *code, t_node *assignment)
{
    char frame[3];
    frame[0] = '\0';
    if (is_global(assignment->next[0]->data[1].data))
    {
        strcpy(frame, "GF");
    }
    else
    {
        strcpy(frame, "LF");
    }

    // pro lepší přehlednost práce s gramatikou
    t_node *id_list = assignment->next[1]->next[0]; // id_list = rest of the ids to assign to
    t_node *aux = assignment->next[1]->next[2];     // aux = <f-or-item-list>

    if (strcmp(aux->next[0]->data[0].data, "id") == 0)
    {
        if (strcmp(aux->next[1]->next[0]->data[0].data, "(") == 0)
        { // VOLÁNÍ FUNKCE

            // nadefinuje návratové proměnné v TF
            strcat_realloc(&code->text, "\nCREATEFRAME\n");
            END_IF_FAIL((&code->text));

            define_return_variables(code, assignment);

            char retval[25];
            retval[0] = '\0';
            int var_nb = 1;

            function_call_gen(code, aux, false);

            sprintf(retval, "%%%dr", var_nb++);

            strcat_format_realloc(&code->text, "\n# -- return values from fc\nMOVE %s@%s TF@%s\n", frame, assignment->next[0]->data[1].data, retval);
            if (strcmp(id_list->next[0]->data[0].data, "eps") != 0)
            {
                do // assign other variables
                {
                    if (is_global(id_list->next[1]->data[1].data))
                    {
                        strcpy(frame, "GF");
                    }
                    else
                    {
                        strcpy(frame, "LF");
                    }

                    sprintf(retval, "%%%dr", var_nb++);
                    strcat_format_realloc(&code->text, "MOVE %s@%s TF@%s\n", frame, id_list->next[1]->data[1].data, retval);
                    END_IF_FAIL((&code->text));
                    id_list = id_list->next[2];
                } while (strcmp(id_list->next[0]->data[0].data, "eps") != 0);
            }
            // následující sekce může pracovat stejně pro expr i id, proto return
            return;
        }
    } // if

    // expr and id
    if (is_global(assignment->next[0]->data[1].data))
    {
        strcpy(frame, "GF");
    }
    else
    {
        strcpy(frame, "LF");
    }
    t_node ref;
    node_init(&ref);
    node_addnext(&ref, aux->next[0]); // kvůli kompatibilitě s funkcí move_item_to_var
    move_item_to_var(code, frame, assignment->next[0]->data[1].data, &ref);
    free(ref.next);
    tree_item_delete(&ref.data[0]);
    tree_item_delete(&ref.data[1]);
    free(ref.data);

    if (strcmp(aux->next[0]->data[0].data, "expr") == 0)
    {
        aux = aux->next[1];
    }
    else
    {
        aux = aux->next[1]->next[0];
    }

    // aux = <item-another>

    while (strcmp(id_list->next[0]->data[0].data, "eps") != 0)
    {
        if (is_global(id_list->next[1]->data[1].data))
        {
            strcpy(frame, "GF");
        }
        else
        {
            strcpy(frame, "LF");
        }
        move_item_to_var(code, frame, id_list->next[1]->data[1].data, aux->next[1]);
        aux = aux->next[2];
        id_list = id_list->next[2];
    }
}

void define_return_variables(code_t *code, t_node *assignment)
{
    t_node *id_list = assignment->next[1]->next[0];

    strcat_format_realloc(&code->text, "\n#---- Definice návratových proměnných\n\n");
    END_IF_FAIL((&code->text));

    strcat_format_realloc(&code->text, "DEFVAR TF@%%1r\nMOVE TF@%%1r nil@nil\n");
    END_IF_FAIL((&code->text));

    int retval_count = 2; // 1 už byla nadefinována

    if (id_list != NULL)
    {
        while (strcmp(id_list->next[0]->data[0].data, "eps") != 0)
        {
            strcat_format_realloc(&code->text, "DEFVAR TF@%%%dr\nMOVE TF@%%%dr nil@nil\n", retval_count, retval_count);
            END_IF_FAIL((&code->text));
            retval_count++;

            id_list = id_list->next[2];
        }
    }

    // doplnění o definici return proměnných, které se nepoužívají
    int return_counts = 0;
    char *fc_name = malloc(strlen(assignment->next[1]->next[2]->next[0]->data[1].data) + 1);
    strcpy(fc_name, assignment->next[1]->next[2]->next[0]->data[1].data);
    fc_name[strlen(assignment->next[1]->next[2]->next[0]->data[1].data) - 3] = '\0';
    get_f_return_count(fc_name, global_sym_table, &return_counts);

    // pokud je počet proměnných, které funkce vrací větší než počet již nainicializovaných proměnných
    while (return_counts > retval_count - 1)
    {
        strcat_format_realloc(&code->text, "DEFVAR TF@%%%dr\nMOVE TF@%%%dr nil@nil\n", retval_count, retval_count);
        END_IF_FAIL((&code->text));
        retval_count++;

        id_list = id_list->next[2];
    }

    free(fc_name);
}

void eval_condition(code_t *code, t_node *condition)
{
    // podmínka na bázi výrazu
    if (condition->next[1]->next_count == 0)
    {
        strcat_format_realloc(&code->text, "\n# --- vyhodnocení nelogického výrazu\n\n");
        END_IF_FAIL((&code->text));
        if (strcmp(condition->next[0]->data[0].data, "id") == 0)
        {
            if (is_global(condition->next[0]->data[1].data))
            {
                strcat_format_realloc(&code->text, "MOVE GF@COMP_RES GF@%s\n", condition->next[0]->data[1].data);
                END_IF_FAIL((&code->text));
            }
            else
            {
                strcat_format_realloc(&code->text, "MOVE GF@COMP_RES LF@%s\n", condition->next[0]->data[1].data);
                END_IF_FAIL((&code->text));
            }
        }
        else
        {
            eval_expression(code, condition->next[0]->next[0]);
            strcat_format_realloc(&code->text, "POPS GF@COMP_RES\n");
            END_IF_FAIL((&code->text));
        }

        // v COMP_RES je výsledek výrazu, nyní musíme převést nil na false a vše ostatní na true
        strcat_format_realloc(&code->text, "JUMPIFEQ EXPR_NIL%d nil@nil GF@COMP_RES\n", code->total_conditionals_count);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "MOVE GF@COMP_RES bool@true\nJUMP EXPR_END%%%d\n", code->total_conditionals_count);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "LABEL EXPR_NIL%d\nMOVE GF@COMP_RES bool@false\nLABEL EXPR_END%%%d\n", code->total_conditionals_count, code->total_conditionals_count);
        END_IF_FAIL((&code->text));

        code->total_conditionals_count++;
    }
    // podmínka na bázi logického porovnávání
    else
    {
        int op = 0; // nabyde hodnoty 2 při druhém průchodu cyklu
        for (int i = 0; i < 2; i++)
        {
            if (strcmp(condition->next[op]->next[0]->data[0].data, "id") == 0)
            {
                char frame[3];
                if (is_global(condition->next[op]->next[0]->data[1].data))
                {
                    strcpy(frame, "GF");
                }
                else
                {
                    strcpy(frame, "LF");
                }
                strcat_format_realloc(&code->text, "MOVE GF@EXPR%d %s@%s\n", i + 1, frame, condition->next[op]->next[0]->data[1].data);
                END_IF_FAIL((&code->text));
            }
            else
            {
                eval_expression(code, condition->next[op]->next[0]);
                strcat_format_realloc(&code->text, "POPS GF@EXPR%d\n\n", i + 1);
                END_IF_FAIL((&code->text));
            }
            op = 2;
        }
        // kód pro případnou implicitní konverzi
        strcat_format_realloc(&code->text, "\n# --- CHECK FOR ARITHMETIC COMPARISON\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "PUSHS GF@EXPR1\nCALL IS_ARITHMETIC\nMOVE GF@COMP_RES GF@IS_ARITH_RES\nPOPS GF@EXPR1\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "PUSHS GF@EXPR2\nCALL IS_ARITHMETIC\nMOVE GF@COMP_RES2 GF@IS_ARITH_RES\nPOPS GF@EXPR2\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "OR GF@COMP_RES GF@COMP_RES GF@COMP_RES2\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "JUMPIFNEQ LBL_NOT_ARITH%d GF@COMP_RES bool@true\n", code->total_conditionals_count);
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "\n#---------------IMPLICITE CONVERTION IF NEEDED\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "PUSHS GF@EXPR1\nPUSHS GF@EXPR2\nCALL ARITHMETIC_TYPE_CHECK\nPOPS GF@EXPR2\nPOPS GF@EXPR1\n");
        END_IF_FAIL((&code->text));
        strcat_format_realloc(&code->text, "\nLABEL LBL_NOT_ARITH%d\n", code->total_conditionals_count);
        END_IF_FAIL((&code->text));
        code->total_conditionals_count++;

        t_node *cond_operator = condition->next[1]->next[0];

        if (strcmp(cond_operator->data[0].data, "==") == 0)
        {
            strcat_format_realloc(&code->text, "\n# -- comparison ==\nEQ GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            return; // nemusíme řešit nil
        }
        else if (strcmp(cond_operator->data[0].data, "~=") == 0)
        {
            strcat_format_realloc(&code->text, "\n# -- comparison =~\nEQ GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "NOT GF@COMP_RES GF@COMP_RES\n");
            END_IF_FAIL((&code->text));
            return;
        }

        // ani jeden z operandů nesmí být nil
        strcat_realloc(&code->text, "\nJUMPIFEQ ERROR_8 nil@nil GF@EXPR1\n");
        END_IF_FAIL((&code->text));
        strcat_realloc(&code->text, "JUMPIFEQ ERROR_8 nil@nil GF@EXPR2\n");
        END_IF_FAIL((&code->text));

        if (strcmp(cond_operator->data[0].data, "<") == 0)
        {
            strcat_format_realloc(&code->text, "\n# -- comparison <\nLT GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
        }
        else if (strcmp(cond_operator->data[0].data, ">") == 0)
        {
            strcat_format_realloc(&code->text, "\n# -- comparison >\nGT GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
        }
        else if (strcmp(cond_operator->data[0].data, "<=") == 0)
        {
            strcat_format_realloc(&code->text, "EQ GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "LT GF@COMP_RES2 GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "OR GF@COMP_RES GF@COMP_RES GF@COMP_RES2\n");
            END_IF_FAIL((&code->text));
        }
        else if (strcmp(cond_operator->data[0].data, ">=") == 0)
        {
            strcat_format_realloc(&code->text, "EQ GF@COMP_RES GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "GT GF@COMP_RES2 GF@EXPR1 GF@EXPR2\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "OR GF@COMP_RES GF@COMP_RES GF@COMP_RES2\n");
            END_IF_FAIL((&code->text));
        }
    }
}

void generate_return(code_t *code, t_node *return_node)
{
    char ret_var[40];
    ret_var[0] = '\0';
    int index = 1;

    if (strcmp(return_node->next[1]->next[0]->data[0].data, "eps") != 0)
    {
        return_node = return_node->next[1]->next[0]; //<return-f-or-items>
        do
        {
            sprintf(ret_var, "LF@%%%dr", index++);

            // strcat_format_realloc(&code->text, "DEFVAR %s\n", ret_var);
            // END_IF_FAIL((&code->text));

            // EXPR
            if (strcmp(return_node->next[0]->data[0].data, "expr") == 0)
            {
                eval_expression(code, return_node->next[0]);
                strcat_format_realloc(&code->text, "POPS %s\n", ret_var);
                END_IF_FAIL((&code->text));
                return_node = return_node->next[1]->next[0];
            }
            // ID
            else if ((strcmp(return_node->next[1]->next[0]->data[0].data, "<return-f-or-items'>") == 0) || (strcmp(return_node->next[1]->next[0]->data[0].data, "eps") == 0))
            {
                if (is_global(return_node->next[1]->data[1].data))
                {
                    strcat_format_realloc(&code->text, "MOVE %s GF@%s\n", ret_var, return_node->next[0]->data[1].data);
                    END_IF_FAIL((&code->text));
                }
                else
                {
                    strcat_format_realloc(&code->text, "MOVE %s LF@%s\n", ret_var, return_node->next[0]->data[1].data);
                    END_IF_FAIL((&code->text));
                }

                if (strcmp(return_node->next[1]->next[0]->data[0].data, "eps") == 0)
                {
                    return_node = return_node->next[1]->next[0];
                }
                else
                {
                    return_node = return_node->next[1]->next[0]->next[0];
                }
            }
            // FC
            else
            {
                strcat_format_realloc(&code->text, "CREATEFRAME\nDEFVAR TF@%%1r\nMOVE TF@%%1r nil@nil\n", ret_var);
                END_IF_FAIL((&code->text));
                function_call_gen(code, return_node, false);
                strcat_format_realloc(&code->text, "MOVE %s TF@%%1r\n", ret_var);
                END_IF_FAIL((&code->text));
                return_node = return_node->next[1]->next[3]->next[0];
            }
            if (strcmp(return_node->data[0].data, "eps") == 0)
            {
                break;
            }
            return_node = return_node->prev->next[1];
        } while (true);
        strcat_format_realloc(&code->text, "POPFRAME\nRETURN\n", ret_var);
        END_IF_FAIL((&code->text));
    }
}

void move_item_to_var(code_t *code, char *dest_frame, const char *dest_id, t_node *item)
{
    // pokud je uvnitř itemu identifikátor
    if (strcmp(item->next[0]->data[0].data, "id") == 0)
    {
        if (is_global(item->next[0]->data[1].data))
        {
            strcat_format_realloc(&code->text, "MOVE %s@%s GF@%s\n", dest_frame, dest_id, item->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else
        {
            strcat_format_realloc(&code->text, "MOVE %s@%s LF@%s\n", dest_frame, dest_id, item->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
    }
    else // uvnitř je expr
    {
        eval_expression(code, item->next[0]);
        strcat_format_realloc(&code->text, "POPS %s@%s\n", dest_frame, dest_id);
        END_IF_FAIL((&code->text));
    }
}

bool is_global(char *id)
{
    if (strlen(id) < 6)
    {
        return false;
    }
    if (strcmp("_global", (char *)(id + strlen(id) + 1 - 8)) == 0)
    {
        return true;
    }
    else
        return false;
}

void create_global_variable(code_t *code, t_node *declaration_node, const char *id)
{
    // declaration_node = function(<types>)<return-types> OR <type><declare-assign>
    if (strcmp(declaration_node->next[0]->data[0].data, "<type>") == 0)
    {
        strcat_format_realloc(&code->text, "DEFVAR GF@%s\n", id);
        END_IF_FAIL((&code->text));

        t_node *assignment_node;
        assignment_node = declaration_node->next[1]; // <declare-assign>
        if (strcmp(assignment_node->next[0]->data[0].data, "eps") != 0)
        {
            move_item_to_var(code, "GF", id, assignment_node->next[1]);
        }
    }
}

void convert_strings(t_node *tree)
{
    if (tree->next_count == 0)
    {
        if (strcmp(tree->data[0].data, "string") == 0)
        {
            if (strcmp(tree->data[1].data, "\"ifj21\"") != 0)
            {
                buffer_t buffer;
                buffer_init(&buffer);
                strcpy_realloc(&buffer, tree->data[1].data);
                END_IF_FAIL((&buffer));

                replace_all_strings_by_string(&buffer, "\\\t", "\\009");
                replace_all_strings_by_string(&buffer, "\\\n", "\\010");
                replace_all_strings_by_string(&buffer, "\"", "\\034");
                replace_all_strings_by_string(&buffer, "\\\\", "\\092");
                replace_all_strings_by_string(&buffer, " ", "\\032");

                char sequence[10];
                char three_numbers[6];
                for (int i = 0; i < 255; i++)
                {
                    sprintf(three_numbers, "\\%03d", i);
                    sprintf(sequence, "\\092%03d", i);
                    replace_all_strings_by_string(&buffer, sequence, three_numbers);
                }

                node_setdata(tree, buffer.data, 1);

                buffer_destroy(&buffer);
            }
        }
    }
    else
    {
        for (int i = 0; i < tree->next_count; i++)
        {
            if (tree->next[i] != NULL)
            {
                convert_strings(tree->next[i]);
            }
        }
    }
}

void eval_expression(code_t *code, t_node *expr)
{
    if (expr->next_count != 1)
    {
        if (expr->next_count == 3)
        { // binární operátory
            eval_expression(code, expr->next[2]);
            eval_expression(code, expr->next[0]);

            if (strcmp(expr->next[1]->data[1].data, "..") != 0)
            {
                // aritmetické operátory

                strcat_format_realloc(&code->text, "CALL ARITHMETIC_TYPE_CHECK\n");
                END_IF_FAIL((&code->text));

                if (strcmp(expr->next[1]->data[1].data, "+") == 0)
                {
                    strcat_format_realloc(&code->text, "ADDS\n");
                    END_IF_FAIL((&code->text));
                }
                else if (strcmp(expr->next[1]->data[1].data, "-") == 0)
                {
                    strcat_format_realloc(&code->text, "SUBS\n");
                    END_IF_FAIL((&code->text));
                }
                else if (strcmp(expr->next[1]->data[1].data, "*") == 0)
                {
                    strcat_format_realloc(&code->text, "MULS\n");
                    END_IF_FAIL((&code->text));
                }
                else if (strcmp(expr->next[1]->data[1].data, "/") == 0)
                {
                    strcat_format_realloc(&code->text, "DIVS\n");
                    END_IF_FAIL((&code->text));
                }
                else if (strcmp(expr->next[1]->data[1].data, "//") == 0)
                {
                    strcat_format_realloc(&code->text, "IDIVS\n");
                    END_IF_FAIL((&code->text));
                }
            }
            else
            { // týká se operátoru ..
              // oddělený, jelikož potřebuje zkontrolovat, zda pracuje se stringy
                strcat_format_realloc(&code->text, "CALL IS_STRING\nPOPS GF@CONVERTION2\n");
                END_IF_FAIL((&code->text));
                strcat_format_realloc(&code->text, "CALL IS_STRING\nPOPS GF@CONVERTION1\n");
                END_IF_FAIL((&code->text));
                strcat_format_realloc(&code->text, "CONCAT GF@CONVERTION1 GF@CONVERTION1 GF@CONVERTION2\n");
                END_IF_FAIL((&code->text));
                strcat_format_realloc(&code->text, "PUSHS GF@CONVERTION1\n");
            }
        }
        else if (expr->next_count == 2)
        { // unární operátory
            eval_expression(code, expr->next[0]);
            strcat_format_realloc(&code->text, "CALL IS_STRING\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "POPS GF@CONVERTION1\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "STRLEN GF@CONVERTION1 GF@CONVERTION1\n");
            END_IF_FAIL((&code->text));
            strcat_format_realloc(&code->text, "PUSHS GF@CONVERTION1\n");
            END_IF_FAIL((&code->text));
        }
    }
    else
    {
        if (strcmp(expr->next[0]->data[0].data, "id") == 0)
        { // na nejspodnější úrovni je identifikátor
            char frame[3];
            if (is_global(expr->next[0]->data[1].data))
            {
                strcpy(frame, "GF");
            }
            else
            {
                strcpy(frame, "LF");
            }
            strcat_format_realloc(&code->text, "PUSHS %s@%s\n", frame, expr->next[0]->data[1].data);
            END_IF_FAIL((&code->text));
        }
        else
        { // na nejspodnější úrovni je hodnota
            if (strcmp(expr->next[0]->data[1].data, "nil") == 0)
            {
                strcat_format_realloc(&code->text, "PUSHS nil@nil\n");
                END_IF_FAIL((&code->text));
            }
            else if (strcmp(expr->next[0]->data[0].data, "integer") == 0)
            {
                strcat_format_realloc(&code->text, "PUSHS int@%s\n", expr->next[0]->data[1].data);
                END_IF_FAIL((&code->text));
            }
            else if (strcmp(expr->next[0]->data[0].data, "number") == 0)
            {
                strcat_format_realloc(&code->text, "PUSHS float@%a\n", strtod(expr->next[0]->data[1].data, NULL));
                END_IF_FAIL((&code->text));
            }
            else if (strcmp(expr->next[0]->data[0].data, "string") == 0)
            {
                strcat_format_realloc(&code->text, "PUSHS string@%s\n", expr->next[0]->data[1].data);
                END_IF_FAIL((&code->text));
            }
        }
    }
}

void free_memory_then_quit(int return_code)
{
    tree_delete(global_tree);
    buffer_destroy(global_code_buffer);
    exit(return_code);
}

void init_code(code_t *code)
{
    buffer_init(&code->text);
    END_IF_FAIL((&code->text));
    code->total_conditionals_count = 0;
    code->total_expr_count = 0;
}
