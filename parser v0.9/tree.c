
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

void tree_item_init(t_tree_item *tree_item)
{
    tree_item->length = 0;
    tree_item->data = NULL;
}

int node_init(t_node *node)
{
    // inicializace potomků 
    node->next_capacity = 5;
    node->next_count = 0;
    node->prev = NULL;
    node->next = malloc(10 * sizeof(t_node *));
    ALLOC_CHECK(node->next)
    // inicializace atributu a typu
    node->data = malloc(sizeof(t_tree_item) * 2);
    ALLOC_CHECK(node->data)
    tree_item_init(&node->data[0]);
    tree_item_init(&node->data[1]);

    return 0;
}

int node_setdata(t_node *node, char *data, int index)
{
    if (index != 0 && index != 1)
        return 0;

    // uvolnění paměti, pokud je už prvek plný
    if (node->data[index].data != NULL)
        free(node->data[index].data);
        
    // nastavení nových hodnot
    node->data[index].data = malloc(strlen(data) + 1);
    node->data[index].length = strlen(data);
    ALLOC_CHECK(node->data[index].data)
    
    strcpy(node->data[index].data, data);
    
    return 0;
}

int node_addnext(t_node *node, t_node *next)
{
    node->next[node->next_count] = next;   // uloží ukazatel na další prvek
    (*next).prev = node;                    // nastavení ukazatele na předchozí prvek
    node->next_count++;
    // pro rozšíření počtu potomků
    return 0;
}

void node_free(t_node *node)
{
    free(node->next);                   // uvolnění potomků
    tree_item_delete(&node->data[0]);   //          atributu
    tree_item_delete(&node->data[1]);   //          typu
    free(node->data);                   //          struktury tree_item
}

void node_delete(t_node *node)
{
    // rekurze přes všechny potomky
    for(; node->next_count > 0; node->next_count--) 
    {
        node_delete(node->next[node->next_count - 1]);
        free(node->next[node->next_count - 1]);
    }
    node_free(node);
}

void node_print(t_node node, size_t tabs)
{
    for (size_t i = 0; i < tabs; i++)
        printf("      ");
    
    printf("%s|%s\n", node.data[0].data, node.data[1].data);
    // printf("%s %s\n", node.data[0].data, node.data[1].data);
}

void tree_print(t_node node, size_t tabs)
{
    // první se printne root, později s tabulátorem navíc potomci
    printf("%d: ", node.next_count);
    node_print(node, tabs);

    // rekurze přes všechny potomky
    for(int i = 0; i < node.next_count; i++)
        tree_print(*(node.next[node.next_count - i - 1]), tabs + 1);
}


void tree_item_delete(t_tree_item *tree_item)
{
    if (tree_item->data != NULL)
        free(tree_item->data);
}


// ! /////////////// ! //
// !     UNUSED      ! //
// ! /////////////// ! //
// void tree_init(t_tree *tree)
// {
//     tree->first = NULL;
//     tree->active = NULL;
// }

// void tree_setfirst(t_tree *tree, t_node *node)
// {
//     t_node new_node;
//     node_init(&new_node);
//     node_copy(&new_node, node);
//     tree->first = &new_node;
// }

// void tree_delete(t_tree *tree)
// {
//     node_delete(tree->first);
// }

// int node_copy(t_node *node1, t_node *node2)
// {
//     node1->next_count = node2->next_count;

//     if (node1->next_capacity != node2->next_capacity)
//     {
//         node1->next_capacity = node2->next_capacity;
//         t_node *newnext = realloc(node1->next, sizeof(t_node) * node1->next_capacity);
        
//         if (newnext == NULL)
//             return 1;
        
//         node1->next = newnext;
//     }

//     for(int i = 0; node2->next_count; i++)
//         node1->next[i] = node2->next[i];

//     tree_item_init(&node1->data[0]);
//     tree_item_init(&node1->data[1]);

//     if (node2->data[0].length != 0)
//         node_setdata(node1, node2->data[0].data, 0);
    
//     if (node2->data[1].length != 0)
//         node_setdata(node1, node2->data[1].data, 1);
    
//     return 0;
// }