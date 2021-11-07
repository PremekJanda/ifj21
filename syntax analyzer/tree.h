#ifndef __TREE_H__
#define __TREE_H__

typedef struct tree_item
{
    int length;
    char *data;
}t_tree_item;

typedef struct node
{
    int next_capacity;
    int next_count;
    struct node *next;
    struct node *prev;
    t_tree_item *data;
}t_node;

typedef struct tree
{
    t_node *first;
    t_node *active;
}t_tree;

void node_init(t_node *node);

void node_addnext(t_node *node, t_node *next);

int node_setdata(t_node *node, char *data, int index);

void node_delete(t_node *node);

int node_copy(t_node *node1, t_node *node2);

void node_print(t_node node);

void tree_init(t_tree *tree);

void tree_print(t_node node);

void tree_setfirst(t_tree *tree, t_node *node);

void tree_delete(t_tree *tree);

#endif