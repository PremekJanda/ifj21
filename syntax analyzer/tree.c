
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

void tree_item_init(t_tree_item *tree_item)
{
    tree_item->length = 0;
    tree_item->data = NULL;
}

void tree_item_delete(t_tree_item *tree_item)
{
    if(tree_item->data != NULL)
        free(tree_item->data);
}

void node_init(t_node *node)
{
    node->next_capacity = 5;
    node->next_count = 0;
    node->next = malloc(sizeof(t_node) * node->next_capacity);
    node->prev = NULL;
    node->data = malloc(sizeof(t_tree_item) * 2);
    tree_item_init(&node->data[0]);
    tree_item_init(&node->data[1]);
}

int node_setdata(t_node *node, char *data, int index)
{
    if(index != 0 && index != 1)
        return 0;
    if(node->data[index].data != NULL)
        free(node->data[index].data);
    node->data[index].data = malloc(strlen(data) + 1);
    node->data[index].length = strlen(data);
    if(node->data[index].data == NULL)
        return 1;
    strcpy(node->data[index].data, data);
    return 0;
}

int node_copy(t_node *node1, t_node *node2)
{
    node1->next_count = node2->next_count;
    if(node1->next_capacity != node2->next_capacity)
    {
        node1->next_capacity = node2->next_capacity;
        t_node *newnext = realloc(node1->next, sizeof(t_node) * node1->next_capacity);
        if(newnext == NULL)
            return 1;
        node1->next = newnext;
    }
    for(int i = 0; node2->next_count; i++)
        node1->next[i] = node2->next[i];
    tree_item_init(&node1->data[0]);
    tree_item_init(&node1->data[1]);
    if(node2->data[0].length != 0)
        node_setdata(node1, node2->data[0].data, 0);
    if(node2->data[1].length != 0)
        node_setdata(node1, node2->data[1].data, 1);
    return 0;
}

void node_free(t_node *node)
{
    free(node->next);
    tree_item_delete(&node->data[0]);
    tree_item_delete(&node->data[1]);
    free(node->data);
}

void node_setprev(t_node *node, t_node *prev)
{
    node->prev = prev;
}

void node_addnext(t_node *node, t_node *next)
{
    t_node new_node;
    node_init(&new_node);
    node_copy(&new_node, next);
    node_setprev(&new_node, node);
    node->next[node->next_count] = new_node;
    node->next_count++;
    if(node->next_count == node->next_capacity)
    {
        node->next_capacity = node->next_capacity * 2;
        t_node *newnext = realloc(node->next, node->next_capacity);
        node->next = newnext;
    }
}

void node_print(t_node node)
{
    printf("\nh");
    printf("\n%s     %s   %d", node.data[0].data, node.data[1].data, node.next_count);
    printf("\n");
}

void node_delete(t_node *node)
{
    for(; node->next_count > 0; node->next_count--)
    {
        node_delete(&node->next[node->next_count - 1]);
    }
    if(node->next_count == 0)
    {
        node_free(node);
        return;
    }
}

void tree_print(t_node node)
{
    for(int i = 0; i < node.next_count; i++)
    {
        tree_print(node.next[i]);
    }
    node_print(node);
}

void tree_init(t_tree *tree)
{
    tree->first = NULL;
    tree->active = NULL;
}

void tree_setfirst(t_tree *tree, t_node *node)
{
    t_node new_node;
    node_init(&new_node);
    node_copy(&new_node, node);
    tree->first = &new_node;
}

void tree_delete(t_tree *tree)
{
    node_delete(tree->first);
}

/*
int main()
{
    printf("smrdi ti odporny kokot\n");
    t_node node;
    node_init(&node);
    node_setdata(&node, "ty geju", 1);
    t_node node1;
    node_init(&node1);
    node_setdata(&node1, "ty geju", 1);
    t_node node2;
    node_init(&node2);
    node_setdata(&node2, "ty geju", 1);
    node_setdata(&node2, "aaaa", 1);

    node_addnext(&node, &node1);
    node_addnext(&node, &node2);


    node_delete(&node);
    return 0;
}*/