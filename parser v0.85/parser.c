#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "table.h"
#include "tree.h"
#include "lexikon.h"

char *token_get(int i)
{
    switch(i)
    {
        case 1:
        return "require";
        break;

        case 2:
        return "ifj21";
        break;

        case 3:
        return "stat";
        break;


        case 4:
        return "stat";
        break;
        case 5:
        return "stat";
        break;




        case 6:
        return "bruh";
        break;

        case 7:
        return "stat";
        break;
                
        case 8:
        return "stat";
        break;

        case 9:
        return "end";
        break;
    }
    return "id";
}

char *token_get2(int i)
{
    switch(i)
    {
        case 1:
        return "(";
        break;

        case 2:
        return "i";
        break;

        case 3:
        return "+";
        break;


        case 4:
        return "i";
        break;
        case 5:
        return "*";
        break;




        case 6:
        return "i";
        break;

        case 7:
        return ")";
        break;
                
        case 8:
        return "*";
        break;

        case 9:
        return "i";
        break;

        case 10:
        return "i";
        break;
                
        case 11:
        return ")";
        break;

        case 13:
        return "+";
        break;
        case 14:
        return "(";
        break;
        case 15:
        return ")";
        break;

        case 16:
        return "end";
        break;
    }
    return "end";
}

int find_rule(t_table table, t_stack stack, char *token)
{
    if(strlen(table_find(table, stack_top(stack), token)) == 0)
        return -1;
    else
        return atoi(table_find(table, stack_top(stack), token));
}

int is_terminal(char *string)
{
    if(string[0] == '<' && string[strlen(string) - 1] == '>')
        return 1;
    else
        return 0;
}

int bottom_up(t_table precedence_table)
{
    t_stack stack;
    stack_init(&stack);
    stack_push(&stack, "$");

    t_stack nonterminals;
    stack_init(&nonterminals);

    t_node node;
    node_init(&node);
    
    int handles_count = 0;
    int handles_capacity = 10;
    int *handles = malloc(sizeof(int) * 20);

    char *token = token_get2(1);
    int tokencount = 2;

    t_node *node_buffer = malloc(20 * sizeof(t_node));
    int node_buffer_count = 0;

    while(strcmp(token, "end"))
    {
        printf("lololchvpisdfoisfsoidv________\n\n\n\n");
        stack_print(stack);
        if(strcmp(stack_top(stack), "E"))
            stack_push(&nonterminals, stack_top(stack));
        char *operator = table_find(precedence_table, stack_top(nonterminals), token);
        printf("\nahoj %s   %s   %s    \n", operator, stack_top(nonterminals), token);
        if(node_buffer_count != 0)
            tree_print(node_buffer[0], 0);
        if(!strcmp(operator,"<"))
        {
            if(strcmp(token, "("))
            {
                t_node new_node;
                node_init(&new_node);
                node_setdata(&new_node, token, 0);
                node_buffer[node_buffer_count] = new_node;
                node_buffer_count++;
            }

            stack_print(stack);
            stack_push(&stack, token);
            handles[handles_count] = stack_topindex(stack);
            if(strcmp(stack_top(stack), "i") && strcmp(stack_top(stack), "e") && strcmp(stack_top(stack), "E"))
                handles[handles_count]--;

            handles_count++;
            token = token_get2(tokencount);



                printf("PICOOOOO");
            tokencount++;
        }
        else if(!strcmp(operator,">"))
        {
            t_node new_node;
            node_init(&new_node);
            node_setdata(&new_node, "E", 0);
            
             printf("\n\n");
            tree_print(node_buffer[0], 0);
            printf("\n\n");
            for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
            {
                if(strcmp(stack_top(stack), "E"))
                    stack_pop(&nonterminals);
                stack_pop(&stack);
                node_buffer_count--;
                node_addnext(&new_node,  &node_buffer[node_buffer_count]);
            }
            
            printf("\n\n");
            tree_print(node_buffer[0], 0);
            printf("\n\n");
            node_buffer[node_buffer_count] = new_node;
            node_buffer_count++;
            handles_count--;
            stack_push(&stack, "E");
        }
        else if(!strcmp(operator,"="))
        {
            stack_print(stack);
            token = token_get2(tokencount);
            tokencount++;
            stack_pop(&nonterminals);
            handles_count--;
            if(!strcmp(stack_top(stack), "E"))
            {
                stack_pop(&stack);
                stack_pop(&stack);
                stack_push(&stack, "E");
            }
            else
                stack_pop(&stack);
            printf("\n\n");
            stack_print(nonterminals);
            printf("\n\n");
            stack_print(stack);
            printf("\n\n");
        }
        else
            return 1;
    }

    while(handles_count != 0)
    {
        printf("\n\n");
        tree_print(node_buffer[0], 0);
        printf("\n\n");
        stack_print(stack);
        printf("\n\n");
        t_node new_node;
        node_init(&new_node);
        node_setdata(&new_node, "E", 0);
        for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
        {
            if(strcmp(stack_top(stack), "E"))
                stack_pop(&nonterminals);
            stack_pop(&stack);
            node_buffer_count--;
            node_addnext(&new_node, &node_buffer[node_buffer_count]);
        }
        node_buffer[node_buffer_count] = new_node;
        node_buffer_count++;
        stack_push(&stack, "E");
        handles_count--;
    }
    free(handles);
    stack_print(stack);
    tree_print(node_buffer[node_buffer_count-1], 0);
    return 0;
}

tToken *token_init()
{
    tToken *token = malloc(sizeof(struct Token));
    if(token == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    } 
    int length = TOKEN_LENGTH;
    token->attribute = malloc(length * sizeof(char));
    if(token->attribute == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    }
    token->type = malloc(15 * sizeof(char));
    if(token->type == NULL){
        fprintf(stderr, "ERROR: Failed malloc");
        return NULL;
    }
    token->line = 1;
    return token;
}

int main()
{    
    t_stack stack;
    stack_init(&stack);
    
    t_table ll_table;
    table_init(&ll_table);
    table_readfile(&ll_table, "tabulka.txt", ';');

    t_table rules;
    table_init(&rules);
    table_readfile(&rules, "rules.txt", ';');

    t_table precedence_table;
    table_init(&precedence_table);
    table_readfile(&precedence_table, "precedence.txt", ':');

    stack_push(&stack, "$");
    stack_push(&stack, "<prog>");
    
    tToken *token = token_init();
    scanner(token);
    
    int count = 2;
    int rule = 0;

    t_node node;
    node_init(&node);
    node_setdata(&node, "<prog>", 0);

    t_node *current_node = &node;

    int next_count_count = 0;
    int *next_count_stack1 = malloc(sizeof(int) * 20);
    int *next_count_stack2 = malloc(sizeof(int) * 20);

    while(strcmp(stack_top(stack), "$") && strcmp(token->type, "EOF"))
    {
        if(!strcmp(token->type, "id") || !strcmp(token->type, "integer") || !strcmp(token->type, "string") || !strcmp(token->type, "number"))
            rule = atoi(table_find(ll_table, stack_top(stack), token->type));
        else
            rule = atoi(table_find(ll_table, stack_top(stack), token->attribute));
        if(rule == 0)
            return 1;
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

        while((strcmp(token->type, "id") ? !strcmp(token->attribute, stack_top(stack)) : !strcmp(token->type, stack_top(stack))) || !strcmp("eps", stack_top(stack)))
        {   
            if(!strcmp(token->type, stack_top(stack)))
                node_setdata(current_node, token->attribute, 1);
            next_count_stack2[next_count_count]++;
            while(next_count_stack1[next_count_count] == next_count_stack2[next_count_count] && current_node->prev != NULL)
            {
                current_node = current_node->prev;
                next_count_count--;
                next_count_stack2[next_count_count]++;
            }
            if(current_node->prev != NULL)
            {
                current_node = current_node->prev->next[next_count_stack2[next_count_count]];
            }   
            if(strcmp("eps", stack_top(stack)))
                scanner(token);
            stack_pop(&stack);
        }
    }
    tree_print(node, 0);
    
    node_delete(&node);
    /*
    printf("\n%d\n", node.next[0].next[1].next[3].next[0].next[0].next_count);*/


    //table_print(precedence_table, ':');
    /*bottom_up(precedence_table);*/

    table_delete(&ll_table);
    table_delete(&rules);
    table_delete(&precedence_table);
    stack_delete(&stack);
    free(token->attribute);
    free(token->type);
    free(token);
    free(next_count_stack1);
    free(next_count_stack2);
    return 0;
}

/*
    while(tokencount <= 6)
    {
        if(strcmp(stack_top(stack), "E"))
            stack_push(&nonterminals, stack_top(stack));
        printf("_____________________________\ntoken:%s\n", token);
        stack_print(nonterminals);
        printf("\n");
        stack_print(stack);
        printf("\n\n");
        char *operator = table_find(precedence_table, stack_top(nonterminals), token);
        if(!strcmp(operator,"<"))
        {
            handles[handles_count] = stack_topindex(stack);
            printf(" handle:%d \n", handles[handles_count]);
            handles_count++;
            if(handles_count == handles_capacity)
            {
                handles_capacity = handles_capacity * 2;
                int *newhandles = realloc(handles, handles_capacity);
                handles = newhandles;
            }
            token = token_get2(tokencount);
            tokencount++;
            stack_push(&stack, token);
        }
        else if(!strcmp(operator,">"))
        {
            for(int i = stack_topindex(stack); i > handles[handles_count - 1]; i--)
            {
                stack_pop(&stack);
                if(strcmp(stack_top(stack), "E"))
                    stack_pop(&nonterminals);
            }
            stack_pop(&stack);
            handles_count--;
            stack_push(&stack, "E");
        }
        else if(!strcmp(operator,"=")){}
        else
        {
            return 1;
        }
    }*/