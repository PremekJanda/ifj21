#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "table.h"
#include "tree.h"
#include "lexikon.h"

int bottom_up(t_table precedence_table, t_node *return_node, tToken *token, tToken *token_backup, char *backup, bool bracket)
{
    //when expr starts with right bracket or it containts empty bracket () -> syntax error 
    if(!strcmp(token->type, ")"))
        return 2;

    //stack for all tokens
    t_stack stack;
    if(stack_init(&stack) == 99)
        return 99;
    stack_push(&stack, "$");

    //stack only for operators
    t_stack operators;
    if(stack_init(&operators) == 99)
        return 99;

    //array for handles
    int handles_count = 0;
    int *handles = malloc(sizeof(int) * 20);
    if(handles == NULL)
    {
        stack_delete(&stack);
        stack_delete(&operators);
        return 99;
    }

    int return_code = 0;

    //buffer for nodes
    t_node **node_buffer = malloc(20 * sizeof(t_node));
    if(node_buffer == NULL)
    {
        free(handles);
        stack_delete(&stack);
        stack_delete(&operators);
        return 99;
    }
    for(int i = 0; i < 20; i++)
        node_buffer[i] = NULL;
    int node_buffer_count = 0;

    while(1)
    {
        if(return_code != 0)
            break;

        //pushes operator into the stack
        if(strcmp(stack_top(stack), "expr"))
            stack_push(&operators, stack_top(stack));

        //item to look for in the precedence table, changes all strings, integers etc. to "id"
        char *item = (!strcmp(token->type, "string") || !strcmp(token->type, "number") || !strcmp(token->type, "id") 
                        || !strcmp(token->type, "integer") || !strcmp(token->type, "nil")) ? "id" : token->attribute;

        //finds operator in the precedence table
        char *table_operator = table_find(precedence_table, stack_top(operators), item);
        
        //when left bracket arrives -> recursion
        if(!strcmp(token->type, "(") && strcmp(table_operator, ""))
        {
            //when unary operator arrives, handle needs to be moved to right
            if(!strcmp(stack_top(operators), "#"))
                handles[handles_count - 1]++;

            t_node *new_node = malloc(sizeof(t_node));
            if(new_node == NULL)
            {
                return_code = 99;
                break;
            }
            scanner(token);

            //since nil always arrives as "keyword" (type), inside of an expr it needs to be changed to "nil"
            if(!strcmp(token->attribute, "nil"))
                strcpy(token->type, "nil");

            //empty brackets () -> syntax error
            if(!strcmp(token->attribute, ")"))
            {
                free(new_node);
                return_code = 2;
                continue;
            }

            //recursion
            return_code = bottom_up(precedence_table, new_node, token, token_backup, backup, true);
            if(return_code == 99)
            {
                free(handles);
                stack_delete(&stack);
                stack_delete(&operators);
                free(node_buffer);
                return 99;
            }
            scanner(token);
            
            //empty brackets () -> syntax error
            if(!strcmp(token->attribute, "nil"))
                strcpy(token->type, "nil");

            //when recursive call is successful, adds returned node to the node buffer
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

        //since brackets are solved by a recursion, ")" means return from the function
        if (!strcmp(token->type, ")") && strcmp(table_operator, ""))
            break;
        //possible syntax error, returns from the function
        if(!strcmp(item, "id") && !strcmp(stack_top(stack), "expr"))
            break;

        //processes "<" operator from the table, sets a new handle and gets new token
        if(!strcmp(table_operator,"<"))
        {
            //creates node for current token, copies it's data inside
            t_node *new_node;
            new_node = malloc(sizeof (t_node));
            if(new_node == NULL)
            {
                return_code = 99;
                break;
            }
            node_init(new_node);
            node_setdata(new_node, token->type, 0);
            node_setdata(new_node, token->attribute, 1);
            new_node->line = token->line;
            //new node is added to the node buffer
            node_buffer[node_buffer_count] = new_node;
            node_buffer_count++;

            //pushes item to the stack and sets the handle
            stack_push(&stack, item);
            handles[handles_count] = stack_topindex(stack);

            //if the item is an operator, handle needs to be moved to left
            if(strcmp(stack_top(stack), "id") && strcmp(stack_top(stack), "expr"))
                handles[handles_count]--;
            handles_count++;

            //when backup token is not empty, replaces current token 
            if(strcmp(backup, ""))
            {
                strcpy(token->attribute, token_backup->attribute);
                strcpy(token->type, token_backup->type);
                backup = "";
            }
            //new token
            else
            {
                return_code = scanner(token);
                if(!strcmp(token->attribute, "nil"))
                    strcpy(token->type, "nil");
            }
        }
        //processes ">" operator, creates new "expr" from all the items until the last handle
        else if(!strcmp(table_operator,">"))
        {
            //creates new "expr" node
            t_node *new_node;
            new_node = malloc(sizeof (t_node));
            if(new_node == NULL)
            {
                return_code = 99;
                break;
            }
            node_init(new_node);
            node_setdata(new_node, "expr", 0);

            //pops all items from the stacks until the last handle and adds
            //all the nodes from the node buffer to the new node
            for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
            {
                if(strcmp(stack_top(stack), "expr"))
                    stack_pop(&operators);
                stack_pop(&stack);
                node_buffer_count--;
                node_addnext(new_node, node_buffer[node_buffer_count]);
                if(node_buffer_count == 0)
                    break;
            }
            //adds new node the the node buffer and removes last handle
            node_buffer[node_buffer_count] = new_node;
            node_buffer_count++;
            handles_count--;

            //processes # operator, needs special treatment
            if(!strcmp(stack_top(operators), "#"))
            {
                new_node = malloc(sizeof (t_node));
                if(new_node == NULL)
                {
                    return_code = 99;
                    break;
                }
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
                stack_pop(&operators);
            }

            //pushes new "expr" to the stack
            stack_push(&stack, "expr");
        }
        //end of expression -> leaves the function
        else
            break;
    }
    
    //same code as before (">" section)
    //when last token has arrives, merges all together to create one "expr"
    while(handles_count != 0 && return_code == 0)
    {
        if(stack_topindex(stack) == 1 && !strcmp(stack_top(stack), "expr"))
            break;
        if(return_code != 0)
            break;
        t_node *new_node;
        new_node = malloc(sizeof (t_node));
        if(new_node == NULL)
        {
            return_code = 99;
            break;
        }
        node_init(new_node);
        node_setdata(new_node, "expr", 0);

        for(int i = stack_topindex(stack); i >= handles[handles_count - 1]; i--)
        {
            if(strcmp(stack_top(stack), "expr"))
                stack_pop(&operators);
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
        if(!strcmp(stack_top(operators), "#"))
        {
            new_node = malloc(sizeof (t_node));
            if(new_node == NULL)
            {
                return_code = 99;
                break;
            }
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
            stack_pop(&operators);
            stack_push(&stack, "expr");
        }
        stack_push(&stack, "expr");
    }

    //if analysis was successful, sets return node to the last "expr" made
    if(node_buffer_count > 0 && return_code == 0)
    {
        node_buffer[node_buffer_count - 1]->prev = return_node->prev;
        if(!bracket)
        node_delete(return_node);
        *return_node = *(node_buffer[node_buffer_count - 1]);
        free(node_buffer[node_buffer_count - 1]);
    }
    
    //frees and deletes all
    free(node_buffer);
    free(handles);
    stack_delete(&operators);
    stack_delete(&stack);
    return return_code;
}

int syntax_analyzer(t_node *tree)
{    
    //predictive parser stack
    t_stack stack;
    if(stack_init(&stack) == 99)
        return 99;
    //LL Table
    t_table ll_table;
    table_init(&ll_table);
    if(table_readfile(&ll_table, "tabulka.txt", ';') == 99)
    {
        stack_delete(&stack);
        return 99;
    }

    //table with all rules
    t_table rules;
    table_init(&rules);
    if(table_readfile(&rules, "rules.txt", ' ') == 99)
    {
        stack_delete(&stack);
        table_delete(&ll_table);
        return 99;
    }

    //precedence table
    t_table precedence_table;
    table_init(&precedence_table);
    if(table_readfile(&precedence_table, "precedence.txt", ';') == 99)
    {
        stack_delete(&stack);
        table_delete(&ll_table);
        table_delete(&rules);
        return 99;
    }

    //inits predictive parser stack
    stack_push(&stack, "$");
    stack_push(&stack, "<prog>");
    
    //tells wheter expr is possible, true -> expr is used to find the rule
    bool expr = false;
    
    int rule = 0;
    int return_code = 0;
    
    //inits token and backup token -> used when parser needs to look one token ahead
    tToken *token = token_init();
    if(token == NULL)
    {
        table_delete(&ll_table);
        table_delete(&rules);
        table_delete(&precedence_table);
        stack_delete(&stack);
        return 99;
    }
    tToken *token_backup = token_init();
    if(token == NULL)
    {
        table_delete(&ll_table);
        table_delete(&rules);
        table_delete(&precedence_table);
        stack_delete(&stack);
        deleteToken(token_backup);
        return 99;
    }
    char *backup = "";
    return_code = scanner(token);

    node_setdata(tree, "<prog>", 0);
    t_node *current_node = tree;

    //next_count_count -> current layer of next_count_stacks
    //next_count_stack1 -> number of processed successors of a current node
    //next_count_stack2 -> number of successors that needs to be processed
    int next_count_count = 0;
    int *next_count_stack1 = malloc(sizeof(int) * 200);
    if(next_count_stack1 == NULL)
    {
        table_delete(&ll_table);
        table_delete(&rules);
        table_delete(&precedence_table);
        stack_delete(&stack);
        deleteToken(token);
        deleteToken(token_backup);
        return 99;
    }
    int *next_count_stack2 = malloc(sizeof(int) * 200);
    if(next_count_stack2 == NULL)
    {
        table_delete(&ll_table);
        table_delete(&rules);
        table_delete(&precedence_table);
        stack_delete(&stack);
        deleteToken(token);
        deleteToken(token_backup);
        free(next_count_stack1);
        return 99;
    }

    while(strcmp(stack_top(stack), "$") && strcmp(token->type, "EOF"))
    {
        if(return_code != 0)
            break;

        //decide when nil token has to change his type to "id" from "keyword"
        if(!strcmp(token->attribute, "nil") && (!strcmp(stack_top(stack), "<f-or-item-list>") 
            || !strcmp(stack_top(stack), "<f-or-item>") || !strcmp(stack_top(stack), "<cond>") || !strcmp(stack_top(stack), "<item>")
            || !strcmp(stack_top(stack), "<return-list>") || !strcmp(stack_top(stack), "<f-or-item>") || !strcmp(stack_top(stack), "<cond>")
            || !strcmp(stack_top(stack), "<item>") || !strcmp(stack_top(stack), "<return-f-or-items>") || !strcmp(stack_top(stack), "<param-list>")))
            strcpy(token->type, "nil");

        //when expr is possible -> expr = true, also when token type is a data type -> token type needs to be used in the table
        if(!strcmp(token->type, "id") || !strcmp(token->type, "integer") || !strcmp(token->type, "nil") || !strcmp(token->type, "string")
            || !strcmp(token->type, "length") || !strcmp(token->type, "number") || !strcmp(token->type, "("))
        {
            rule = atoi(table_find(ll_table, stack_top(stack), token->type));
    	    expr = true;
        }
        else
        {
            rule = atoi(table_find(ll_table, stack_top(stack), token->attribute));
            expr = false;
        }

        //when parser isn't sure wheter to use expr or id -> token_backup is used to look ahead
        if(!strcmp(token->type, "id") && (atoi(table_find(ll_table, stack_top(stack), "expr")) != 0 
            && table_find(ll_table, stack_top(stack), "id") != 0))
        {
            //if token_backup is empty, calls the scanner
            if(!strcmp(backup, ""))
            {
                return_code = scanner(token_backup);
                backup = token_backup->attribute;
            }
            //decides whether to use expr or id in the table
            if(strcmp(backup, ",") && strcmp(backup, "("))
                rule = atoi(table_find(ll_table, stack_top(stack), "expr"));
            else
                rule = atoi(table_find(ll_table, stack_top(stack), "id"));
        }
        //if expr is possible and rule still hasn't been find, tries to find expr in the table
        if(expr == true && rule == 0)
            rule = atoi(table_find(ll_table, stack_top(stack), "expr"));
        //if rule wasn't found -> syntax error
        if(rule == 0)
        {
            return_code = 2;
            break;
        }
        //pops stack
        stack_pop(&stack);
        //uses the rule -> adds all non/terminals from the rule to the current node and pushes it to the stack 
        for(int i = table_linelength(rules, rule) - 1; i >= 0; i--)
        {
            t_node *new_node = malloc(sizeof(t_node));
            if(new_node == NULL)
                return_code = 99;
            node_init(new_node);
            node_setdata(new_node, table_getdata(rules, rule, table_linelength(rules, rule) - 1 - i), 0);
            node_addnext(current_node, new_node);
            stack_push(&stack, table_getdata(rules, rule, i));
        }
        if(return_code == 99)
            break;
        //sets new layer of next count stacks
        next_count_count++;
        next_count_stack1[next_count_count] = table_linelength(rules, rule);
        next_count_stack2[next_count_count] = 0;
        current_node = current_node->next[next_count_stack2[next_count_count]];

        //processes and checks all created nodes until new rule needs to be used
        while((strcmp(token->type, "id") ? !strcmp(token->attribute, stack_top(stack)) : !strcmp(token->type, stack_top(stack))) 
            || !strcmp("eps", stack_top(stack)) || !strcmp("expr", stack_top(stack)))
        {   
            //when expr is on the top of the stack, bottom-up analysis is called
            if(!strcmp("expr", stack_top(stack)))
            {
                return_code = bottom_up(precedence_table, current_node, token, token_backup, backup, false);
                if(return_code != 0)
                    break;
                backup = "";
            }
            //if current node is terminal, copies current token into the node
            if(!strcmp(token->type, stack_top(stack))  || (!strcmp(token->type, "keyword") && strcmp(current_node->data[0].data, "expr")
                && strcmp(current_node->data[0].data, "eps")) || !strcmp(token->attribute, "\"ifj21\""))
            {
                node_setdata(current_node, token->attribute, 1);
                node_setdata(current_node, token->type, 0);
                current_node->line = token->line;
            }
            //current node is proccesed
            next_count_stack2[next_count_count]++;
            //when all successor nodes of the current layer are processed, current node goes higher in the tree
            while(next_count_stack1[next_count_count] == next_count_stack2[next_count_count] && current_node->prev != NULL)
            {
                current_node = current_node->prev;
                next_count_count--;
                next_count_stack2[next_count_count]++;
            }
            //current node moves to the next node that needs to be processed
            if(current_node->prev != NULL)
                current_node = current_node->prev->next[next_count_stack2[next_count_count]];
            //gets new token if "eps" and "expr" are not in the top of the stack
            if(strcmp("eps", stack_top(stack)) && strcmp(stack_top(stack), "expr"))
            {
                //if token_backup was used, new token gets all data from the backup
                if(strcmp(backup, ""))
                {
                    strcpy(token->attribute, token_backup->attribute);
                    strcpy(token->type, token_backup->type);
                    backup = "";
                }
                //new token
                else
                    return_code = scanner(token);
            }
            //current node is proccessed -> pops stack
            stack_pop(&stack);
            //precesses if with no relation operators, for example: if 1 then ...
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
    //if stack is not empty or the first node of the tree has no successors (empty program) -> syntax error
    if ((stack_topindex(stack) != 1 || tree->next_count == 0) && return_code != 1)
        return_code = 2;
    //error message
    if(return_code == 2)
        fprintf(stderr, "Syntax error\nToken: %s  %s    Line: %d\n", token->attribute, token->type, token->line);

    //deletes and frees all
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