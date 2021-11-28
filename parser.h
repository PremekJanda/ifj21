#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "table.h"
#include "tree.h"
#include "lexikon.h"

char *token_get(int i);

int find_rule(t_table table, t_stack stack, char *token);

int is_terminal(char *string);

int bottom_up(t_table precedence_table);

tToken *token_init();

int syntax_analyzer(t_node *tree);