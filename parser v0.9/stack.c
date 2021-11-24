#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "string.h"

int item_init(t_item *item)
{
    item->capacity = 10;
    item->length = 0;
    item->data = malloc(item->capacity);
    if(item->data == NULL)
        return 1;
    return 0;
}

int item_addchar(t_item *item, char c)
{
    item->data[item->length] = c;
    item->length++;
    if(item->length == item->capacity)
    {
        item->capacity = item->capacity * 2;
        char *newdata = realloc(item->data, sizeof(char) * item->capacity);
        if(newdata == NULL)
            return 1;
        item->data = newdata;
    }
    return 0;
}

int item_copy(t_item *item1, char *data)
{
    for(size_t i = 0; i < strlen(data); i++)
    {
        item_addchar(item1, data[i]);
    }
    item_addchar(item1, '\0');
    if(item1->data==NULL)
        return 1;
    return 0;
}

void item_delete(t_item *item)
{
    free(item->data);
}

void print_item(t_item item)
{
    for(int i = 0; i < item.length; i++)
        printf("%c", item.data[i]);
}

int stack_init(t_stack *stack)
{
    stack->capacity = 10;
    stack->top = -1;
    stack->data = malloc(sizeof(t_item) * stack->capacity);
    if(stack->data == NULL)
        return 1;
    return 0;
}

int stack_push(t_stack *stack, char *data)
{
    stack->top++;
    t_item new_item;
    item_init(&new_item);
    item_copy(&new_item, data);
    stack->data[stack->top] = new_item;
    if(stack->top + 1 == stack->capacity)
    {
        stack->capacity = stack->capacity * 2;
        t_item *newdata = realloc(stack->data, sizeof(t_item) * stack->capacity);
        if(newdata == NULL)
            return 1;
        stack->data = newdata;
    }
    return 0;
}

void stack_pop(t_stack *stack)
{
    if(stack->top != -1)
    {
        item_delete(&stack->data[stack->top]);
        stack->top--;
    }
}

char *stack_top(t_stack stack)
{
    return stack.data[stack.top].data;
}

void stack_delete(t_stack *stack)
{
    for(int i = 0; i <= stack->top; i++)
        item_delete(&stack->data[i]);
    free(stack->data);
}

void stack_print(t_stack stack)
{
    for(int i = 0; i <= stack.top; i++)
    {
        print_item(stack.data[i]);
        printf("\n");
    }
}

int stack_topindex(t_stack stack)
{
    return stack.top;
}
/*
int main()
{
    t_item item;
    item_init(&item);
    item_addchar(&item, 'c');
    item_addchar(&item, 'c');
    item_addchar(&item, 'c');
    item_addchar(&item, '\0');

    t_stack stack;
    stack_init(&stack);
    stack_push(&stack, item);
    item.data[1] = 'a';
    stack_push(&stack, item);
    stack_push(&stack, item);
    stack_push(&stack, item);
    item.data[1] = 'l';
    stack_push(&stack, item);
    stack_push(&stack, item);
    stack_push(&stack, item);

    print_stack(&stack);

    stack_pop(&stack);
    stack_pop(&stack);
    stack_pop(&stack);
    printf("\n\n");
    print_stack(&stack);

    printf("\n\n");
    char *top = stack_top(&stack);
    top[1] = 'p';
    printf("%s\n", top);
    print_stack(&stack);

    stack_delete(&stack);
    item_delete(&item);


    return 0;
}*/