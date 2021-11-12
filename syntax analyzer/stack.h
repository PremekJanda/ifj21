#ifndef __STACK_H__
#define __STACK_H__

typedef struct item
{
    int capacity;
    int length;
    char *data;
}t_item;

typedef struct stack
{
    int top;
    int capacity;
    t_item *data;
}t_stack;

int stack_init(t_stack *stack);

int stack_push(t_stack *stack, char *data);

void stack_pop(t_stack *stack);

char *stack_top(t_stack stack);

void stack_delete(t_stack *stack);

void stack_print(t_stack stack);

int stack_topindex(t_stack stack);

#endif