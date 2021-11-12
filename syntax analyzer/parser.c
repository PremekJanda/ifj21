#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "table.h"
#include "tree.h"

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
        return "stat";
        break;


        case 7:
        return "end";
        break;

        case 8:
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
        return "i";
        break;

        case 2:
        return "*";
        break;

        case 3:
        return "i";
        break;

        case 4:
        return "+";
        break;
        
        case 5:
        return "i";
        break;

        case 6:
        return "*";
        break;

        case 7:
        return "i";
        break;

        case 8:
        return "end";
        break;
    }
    return "id";
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
    
    int handles_count = 0;
    int handles_capacity = 10;
    int *handles = malloc(sizeof(int) * 10);

    char *token = token_get2(1);
    int tokencount = 2;

    while(strcmp(token, "end"))
    {
        if(strcmp(stack_top(stack), "E"))
            stack_push(&nonterminals, stack_top(stack));
        char *operator = table_find(precedence_table, stack_top(nonterminals), token);
        printf("\n%s %d    non:%s ter:%s  \n ",operator, tokencount, stack_top(nonterminals), token );
        if(!strcmp(operator,"<"))
        {
            stack_push(&stack, token);
            handles[handles_count] = stack_topindex(stack);
            if(strcmp(stack_top(stack), "i") && strcmp(stack_top(stack), "E"))
                handles[handles_count]--;
            printf("jdi do pici");
            printf("\nlasthan:%d\n",handles[handles_count]);

            handles_count++;
            token = token_get2(tokencount);
            tokencount++;
        }
        else if(!strcmp(operator,">"))
        {
            
            printf("\n-----------------------------\n");
            stack_print(stack);
            printf("\n\n");
            stack_print(nonterminals);
            printf("\n\n");
            printf("\ntu lasthan:%d\n",handles[handles_count - 1]);
            for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
            {
                if(strcmp(stack_top(stack), "E"))
                    stack_pop(&nonterminals);
                stack_pop(&stack);
                printf("\ntu lasthan: %d  %d  %d\n", handles_count ,handles[handles_count - 1], i);
            }
            printf("kokokoko");
            handles_count--;
            stack_push(&stack, "E");
        }
        else
        {
            return 1;
        }
    }
    while(handles_count != 0)
    {
        printf("\n\n");
        stack_print(stack);
        printf("\n\n");
        for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
        {
            stack_pop(&stack);
            if(strcmp(stack_top(stack), "E"))
                stack_pop(&nonterminals);
        }
        handles_count--;
        stack_push(&stack, "E");
    }
    free(handles);
    printf("jdi do pici");
    printf("\n\n");
    stack_print(stack);
    printf("\n\n");
    return 0;
}


int main()
{    
    t_stack stack;
    stack_init(&stack);
    
    t_table ll_table;
    table_init(&ll_table);
    table_readfile(&ll_table, "lltable.txt", ':');

    t_table rules;
    table_init(&rules);
    table_readfile(&rules, "rules.txt", ':');

    t_table precedence_table;
    table_init(&precedence_table);
    table_readfile(&precedence_table, "precedence.txt", ':');

    t_tree tree;
    tree_init(&tree);

    stack_push(&stack, "$");
    stack_push(&stack, "<prog>");
    char *token = token_get(1);
    int count = 2;
    int rule = 0;
    /*
    t_node node;
    node_init(&node);
    node_setdata(&node, stack_top(stack), 0);
    tree_setfirst(&node);*/

    t_node node;
    node_init(&node);
    node_setdata(&node, "<prog>", 0);
    
    t_node node5;
    node_init(&node5);
    node_setdata(&node5, "kokotchlupaty", 0);

    t_node *current_node = &node;
    printf("\n\nvylizte mi prdel: %s\n", node.data[0].data);

    table_print(ll_table,':');
    t_tree syntax_tree;

    tree_setfirst(&syntax_tree, &node);
    
    printf("%s", syntax_tree.first->data[0].data);
    printf("------------------------------");
    printf("\n");
    
    printf("kokooot%s", syntax_tree.first->data[0].data);
    
    int next_count_count = 0;
    int *next_count_stack1 = malloc(sizeof(int) * 100000000);
    int *next_count_stack2 = malloc(sizeof(int) * 100000000);

    while(strcmp(stack_top(stack), "$"))
    {
        stack_print(stack);
        printf("\n\n");
        rule = atoi(table_find(ll_table, stack_top(stack), token));
        if(rule == 0)
        {
            printf("sytax error\n");
            return 1;
        }
        stack_pop(&stack);
        printf("\n\n");
        for(int i = table_linelength(rules, rule) - 1; i >= 0; i--)
        {
            printf("%d:", i);
            t_node new_node;
            node_init(&new_node);
            node_setdata(&new_node, table_getdata(rules, rule, table_linelength(rules, rule) - 1 - i), 0);
            node_addnext(current_node, &new_node);
            printf("[]");

            stack_push(&stack, table_getdata(rules, rule, i));
        }
        printf("\ntooo: %s", current_node->data[0].data);
        next_count_count++;
        next_count_stack1[next_count_count] = table_linelength(rules, rule);
        next_count_stack2[next_count_count] = 0;
        current_node = &current_node->next[next_count_stack2[next_count_count]];
        printf("\ntooo2: %s\n\n", current_node->data[0].data);
        while(!strcmp(token, stack_top(stack)))
        {
            next_count_stack2[next_count_count]++;
            printf("\n\nxddd: %d    %d\n\n", next_count_stack1[next_count_count],next_count_stack2[next_count_count]);
            while(next_count_stack1[next_count_count] == next_count_stack2[next_count_count] && next_count_count > 0 && current_node->prev != NULL)
            {
                printf("ahojj");
                current_node = current_node->prev;
                next_count_count--;
                next_count_stack2[next_count_count]++;
                printf("\ntooo3: %s", current_node->data[0].data);
                printf("\nodporny kokot  :%d:  %d  \n \n\n\n", next_count_stack1[next_count_count],next_count_stack2[next_count_count]);
            }
            if(current_node->prev != NULL)
                current_node = &current_node->prev->next[next_count_stack2[next_count_count]];
            token = token_get(count);
            count++;
            stack_pop(&stack);
        } 
    }

    printf("------------------------------");
    printf("\n");
    
    tree_print(node);
    
    /*
    printf("\n%d\n", node.next[0].next[1].next[3].next[0].next[0].next_count);*/


    //table_print(precedence_table, ':');
    /*bottom_up(precedence_table);*/

    table_delete(&ll_table);
    table_delete(&rules);
    table_delete(&precedence_table);
    stack_delete(&stack);

    printf("\n");

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