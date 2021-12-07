#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "table.h"
#include "tree.h"
#include "lexikon.h"

int bottom_up(t_table precedence_table, t_node *return_node, tToken *token, tToken *token_backup, char *backup, bool bracket)
{
    if(!strcmp(token->type, ")"))
        return 2;

    t_stack stack;
    stack_init(&stack);
    stack_push(&stack, "$");

    t_stack nonterminals;
    stack_init(&nonterminals);

    int handles_count = 0;
    int *handles = malloc(sizeof(int) * 20);

    int brackets = 0;
    int return_code = 0;

    t_node **node_buffer = malloc(20 * sizeof(t_node));
    for(int i = 0; i < 20; i++)
        node_buffer[i] = NULL;
    int node_buffer_count = 0;

    while(brackets >= 0)
    {
        if(return_code != 0)
        {
            break;
        }
        if(strcmp(stack_top(stack), "expr"))
            stack_push(&nonterminals, stack_top(stack));
        char *item = (!strcmp(token->type, "string") || !strcmp(token->type, "number") || !strcmp(token->type, "id") || !strcmp(token->type, "integer") || !strcmp(token->type, "nil")) ? "id" : token->attribute;
        char *operator = table_find(precedence_table, stack_top(nonterminals), item);
        
        if(!strcmp(token->type, "(") && strcmp(operator, ""))
        {
            if(!strcmp(stack_top(nonterminals), "#"))
                handles[handles_count - 1]++;
            t_node *new_node = malloc(sizeof(t_node));
            scanner(token);
            if(!strcmp(token->attribute, "nil"))
                strcpy(token->type, "nil");
            if(!strcmp(token->attribute, ")"))
            {
                free(new_node);
                return_code = 2;
                continue;
            }
            return_code = bottom_up(precedence_table, new_node, token, token_backup, backup, true);
            scanner(token);
            if(!strcmp(token->attribute, "nil"))
                strcpy(token->type, "nil");
            if(return_code == 0)
            {
                node_buffer[node_buffer_count] = new_node;
                node_buffer_count++;
                stack_push(&stack, "expr");
            }
            else
                free(new_node);
            continue;
        }
        if (!strcmp(token->type, ")") && strcmp(operator, ""))
            break;
        if(!strcmp(item, "id") && (!strcmp(stack_top(stack), "expr")))
            break;
        if(!strcmp(operator,"<"))
        {
            if(strcmp(item, "(") && strcmp(item, ")"))
            {
                t_node *new_node;
                new_node = malloc(sizeof (t_node));
                node_init(new_node);
                node_setdata(new_node, token->type, 0);
                node_setdata(new_node, token->attribute, 1);
                new_node->line = token->line;
                node_buffer[node_buffer_count] = new_node;
                node_buffer_count++;
            }
            stack_push(&stack, item);
            handles[handles_count] = stack_topindex(stack);
            if(strcmp(stack_top(stack), "id") && strcmp(stack_top(stack), "expr"))
                handles[handles_count]--;
            
            handles_count++;
            if(strcmp(backup, ""))
            {
                strcpy(token->attribute, token_backup->attribute);
                strcpy(token->type, token_backup->type);
                backup = "";
            }
            else
            {
                return_code = scanner(token);
                if(!strcmp(token->attribute, "nil"))
                    strcpy(token->type, "nil");
            }
        }
        else if(!strcmp(operator,">"))
        {
            if(brackets < 0)
                break;
            t_node *new_node;
            new_node = malloc(sizeof (t_node));
            node_init(new_node);
            node_setdata(new_node, "expr", 0);
            for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
            {
                if(strcmp(stack_top(stack), "expr"))
                    stack_pop(&nonterminals);
                stack_pop(&stack);
                node_buffer_count--;
                node_addnext(new_node, node_buffer[node_buffer_count]);
                if(node_buffer_count == 0)
                    break;
            }
            node_buffer[node_buffer_count] = new_node;
            node_buffer_count++;
            handles_count--;
            if(!strcmp(stack_top(nonterminals), "#"))
            {
                new_node = malloc(sizeof (t_node));
                node_init(new_node);
                node_setdata(new_node, "expr", 0);
                node_buffer_count--;
                node_addnext(new_node, node_buffer[node_buffer_count]);
                node_buffer_count--;
                node_addnext(new_node, node_buffer[node_buffer_count]);
                node_buffer[node_buffer_count] = new_node;
                node_buffer_count++;
                handles_count--;
                stack_pop(&stack);
                stack_pop(&nonterminals);
            }
            stack_push(&stack, "expr");
        }
        else
        {
            break;
        }
        
    }
    
    while(handles_count != 0 && return_code == 0)
    {
        if(stack_topindex(stack) == 1 && !strcmp(stack_top(stack), "expr"))
            break;
        if(return_code != 0)
            break;
        t_node *new_node;
        new_node = malloc(sizeof (t_node));
        node_init(new_node);
        node_setdata(new_node, "expr", 0);

        for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
        {
            if(strcmp(stack_top(stack), "expr"))
                stack_pop(&nonterminals);
            stack_pop(&stack);
            node_buffer_count--;
            node_addnext(new_node, node_buffer[node_buffer_count]);
            if(node_buffer_count == 0)
                break;
        }
        node_buffer[node_buffer_count] = new_node;
        node_buffer_count++;
        handles_count--;
        if(handles_count == 0)
            break;
        if(!strcmp(stack_top(nonterminals), "#"))
        {
            new_node = malloc(sizeof (t_node));
            node_init(new_node);
            node_setdata(new_node, "expr", 0);
            node_buffer_count--;
            node_addnext(new_node, node_buffer[node_buffer_count]);
            node_buffer_count--;
            node_addnext(new_node, node_buffer[node_buffer_count]);
            node_buffer[node_buffer_count] = new_node;
            node_buffer_count++;
            handles_count--;
            stack_pop(&stack);
            stack_pop(&nonterminals);
            stack_push(&stack, "expr");
        }
        stack_push(&stack, "expr");
    }
    if(node_buffer_count > 0)
    {
        node_buffer[node_buffer_count - 1]->prev = return_node->prev;
        if(!bracket)
        node_delete(return_node);
        *return_node = *(node_buffer[node_buffer_count - 1]);
        free(node_buffer[node_buffer_count - 1]);
    }
        
    free(node_buffer);
    free(handles);
    stack_delete(&nonterminals);
    stack_delete(&stack);
    return return_code;
}

int syntax_analyzer(t_node *tree)
{    
    t_stack stack;
    stack_init(&stack);
    
    t_table ll_table;
    table_init(&ll_table);
    table_readfile(&ll_table, "tabulka.txt", ';');

    t_table rules;
    table_init(&rules);
    table_readfile(&rules, "rules.txt", ' ');

    t_table precedence_table;
    table_init(&precedence_table);
    table_readfile(&precedence_table, "precedence.txt", ';');

    stack_push(&stack, "$");
    stack_push(&stack, "<prog>");
    
    bool expr = false;
    //int count = 2;
    int rule = 0;
    int return_code = 0;
    
    tToken *token = token_init();
    tToken *token_backup = token_init();
    char *backup = "";
    return_code = scanner(token);

    node_setdata(tree, "<prog>", 0);

    t_node *current_node = tree;

    int next_count_count = 0;
    int *next_count_stack1 = malloc(sizeof(int) * 200);
    int *next_count_stack2 = malloc(sizeof(int) * 200);

    while(strcmp(stack_top(stack), "$") && strcmp(token->type, "EOF"))
    {
        if(return_code != 0)
            break;
        if(!strcmp(token->attribute, "nil") && !strcmp(token->type, "keyword") && (!strcmp(stack_top(stack), "<f-or-item-list>") || !strcmp(stack_top(stack), "<f-or-item>") || !strcmp(stack_top(stack), "<cond>") || !strcmp(stack_top(stack), "<item>") || !strcmp(stack_top(stack), "<return-list>") || !strcmp(stack_top(stack), "<f-or-item>") || !strcmp(stack_top(stack), "<cond>") || !strcmp(stack_top(stack), "<item>") || !strcmp(stack_top(stack), "<return-f-or-items>") || !strcmp(stack_top(stack), "<param-list>")))
            strcpy(token->type, "nil");
        if(!strcmp(token->type, "id") || !strcmp(token->type, "integer") || !strcmp(token->type, "nil") || !strcmp(token->type, "string") || !strcmp(token->type, "length") || !strcmp(token->type, "number") || !strcmp(token->type, "("))
        {
            rule = atoi(table_find(ll_table, stack_top(stack), token->type));
    	    expr = true;
        }
        else
        {
            rule = atoi(table_find(ll_table, stack_top(stack), token->attribute));
            expr = false;
        }
        if(!strcmp(token->type, "id") && (atoi(table_find(ll_table, stack_top(stack), "expr")) != 0 && table_find(ll_table, stack_top(stack), "id") != 0))
        {
            if(!strcmp(backup, ""))
            {
                return_code = scanner(token_backup);
                backup = token_backup->attribute;
            }
            if(strcmp(backup, ",") && strcmp(backup, "("))
                rule = atoi(table_find(ll_table, stack_top(stack), "expr"));
            else
                rule = atoi(table_find(ll_table, stack_top(stack), "id"));
        }
        if(expr == true && rule == 0)
            rule = atoi(table_find(ll_table, stack_top(stack), "expr"));
        if(rule == 0)
        {
            return_code = 2;
            break;
        }
        stack_pop(&stack);
        for(int i = table_linelength(rules, rule) - 1; i >= 0; i--)
        {
            t_node *new_node = malloc(sizeof(t_node));
            node_init(new_node);
            node_setdata(new_node, table_getdata(rules, rule, table_linelength(rules, rule) - 1 - i), 0);
            node_addnext(current_node, new_node);
            stack_push(&stack, table_getdata(rules, rule, i));
        }
        next_count_count++;
        next_count_stack1[next_count_count] = table_linelength(rules, rule);
        next_count_stack2[next_count_count] = 0;
        current_node = current_node->next[next_count_stack2[next_count_count]];

        while((strcmp(token->type, "id") ? !strcmp(token->attribute, stack_top(stack)) : !strcmp(token->type, stack_top(stack))) || !strcmp("eps", stack_top(stack)) || !strcmp("expr", stack_top(stack)))
        {   
            if(!strcmp("expr", stack_top(stack)))
            {
                return_code = bottom_up(precedence_table, current_node, token, token_backup, backup, false);
                backup = "";
            }
            if(!strcmp(token->type, stack_top(stack))  || (!strcmp(token->type, "keyword") && strcmp(current_node->data[0].data, "expr") && strcmp(current_node->data[0].data, "eps")) || !strcmp(token->attribute, "\"ifj21\""))
            {
                node_setdata(current_node, token->attribute, 1);
                node_setdata(current_node, token->type, 0);
                current_node->line = token->line;
            }
            next_count_stack2[next_count_count]++;
            while(next_count_stack1[next_count_count] == next_count_stack2[next_count_count] && current_node->prev != NULL)
            {
                current_node = current_node->prev;
                next_count_count--;
                next_count_stack2[next_count_count]++;
            }
            if(current_node->prev != NULL)
                current_node = current_node->prev->next[next_count_stack2[next_count_count]];
            if(strcmp("eps", stack_top(stack)) && strcmp(stack_top(stack), "expr"))
            {
                if(strcmp(backup, ""))
                {
                    strcpy(token->attribute, token_backup->attribute);
                    strcpy(token->type, token_backup->type);
                    backup = "";
                }
                else
                {
                    return_code = scanner(token);
                }
            }
            stack_pop(&stack);
            if(!strcmp(stack_top(stack), "<cond-oper>") && (!strcmp(token->attribute, "then")  || !strcmp(token->attribute, "do")))
            {
                next_count_stack2[next_count_count]++; 
                next_count_stack2[next_count_count]++; 
                next_count_count--;
                current_node = current_node->prev;
                next_count_stack2[next_count_count]++; 
                current_node = current_node->prev->next[next_count_stack2[next_count_count]];
                stack_pop(&stack);           
                stack_pop(&stack);             
            }
        }
    }
    if(stack_topindex(stack) != 1 || tree->next_count == 0)
        return_code = 2;
    if(return_code == 2)
    {
        fprintf(stderr, "Syntax error\nToken: %s  %s    Line: %d\n", token->attribute, token->type, token->line);
    }
    table_delete(&ll_table);
    table_delete(&rules);
    table_delete(&precedence_table);
    stack_delete(&stack);
    free(token_backup->attribute);
    free(token_backup->type);
    free(token_backup);
    free(token->attribute);
    free(token->type);
    free(token);
    free(next_count_stack1);
    free(next_count_stack2);
    return return_code;
}