/**
 *  Soubor: test.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:18. 11. 2021 04:12:09
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Testovací rozhraní obsahující makra pro generování částí kódů
 */

// následující řádky zabrání násobnému vložení:
#ifndef __TEST_H__
#define __TEST_H__

#define NODE_INIT(node, attribute, type) \
    t_node node; \
    node_init(&node); \
    node_setdata(&node, attribute, 0); \
    node_setdata(&node, type, 1);

#define MAKE_TREE() \
    NODE_INIT(node1,  "", "<prog>") \
     \
    NODE_INIT(node2,  "keyword", "require") \
    NODE_INIT(node3,  "string", "ifj21") \
    NODE_INIT(node4,  "", "<main-list>") \
 \
    NODE_INIT(node5,  "", "<def-declare-fcall>") \
     \
    NODE_INIT(node6,  "", "<main-list>") \
 \
    NODE_INIT(node7,  "keyword", "function") \
    NODE_INIT(node8,  "string", "main") \
    NODE_INIT(node9,  "", "<func-argument-list>") \
    NODE_INIT(node10, "", "<return-types>") \
    NODE_INIT(node11, "", "<statement-list>") \
    NODE_INIT(node12, "keyword", "end") \
 \
    NODE_INIT(node13, "", "<def-declare-fcal>") \
    NODE_INIT(node14, "", "<main-list>") \
     \
    NODE_INIT(node15, "string", "eps") \
 \
    NODE_INIT(node16, "string", "eps") \
 \
    NODE_INIT(node17, "", "<statement>") \
    NODE_INIT(node18, "", "<statement-list>") \
 \
    NODE_INIT(node19, "string", "main") \
    NODE_INIT(node20, "", "<item-list>") \
 \
    NODE_INIT(node21, "", "eps") \
 \
 \
    node_addnext(&node1, &node2); \
    node_addnext(&node1, &node3); \
    node_addnext(&node1, &node4); \
 \
    node_addnext(&node1.next[2], &node5); \
    node_addnext(&node1.next[2], &node6); \
 \
    node_addnext(&node1.next[2].next[0], &node7); \
    node_addnext(&node1.next[2].next[0], &node8); \
    node_addnext(&node1.next[2].next[0], &node9); \
    node_addnext(&node1.next[2].next[0], &node10); \
    node_addnext(&node1.next[2].next[0], &node11); \
    node_addnext(&node1.next[2].next[0], &node12); \
 \
    node_addnext(&node1.next[2].next[1], &node13); \
    node_addnext(&node1.next[2].next[1], &node14); \
     \
    node_addnext(&node1.next[2].next[0].next[2], &node15); \
 \
    node_addnext(&node1.next[2].next[0].next[3], &node16); \
     \
    node_addnext(&node1.next[2].next[0].next[4], &node17); \
    node_addnext(&node1.next[2].next[0].next[4], &node18); \
 \
    node_addnext(&node1.next[2].next[1].next[0], &node19); \
    node_addnext(&node1.next[2].next[1].next[0], &node20); \
 \
    node_addnext(&node1.next[2].next[1].next[1], &node21); \
 \
    tree_print(node1, 0);



#define MAKE_SMALL_TREE() \
    NODE_INIT(node1,  "<prog>", "") \
     \
    NODE_INIT(node2,  "require", "keyword") \
    NODE_INIT(node3,  "ifj21", "string") \
    NODE_INIT(node4,  "<main-list>", "") \
    NODE_INIT(node5,  "bullshit", "string") \
\
    node_addnext(&node1, &node2); \
    node_addnext(&node1, &node3); \
    node_addnext(&node1, &node4); \
\
    node_addnext(&node1.next[2], &node5); \
    tree_print(node1, 0);



#define CREATE_HTAB(hash_table) \
    htab_t *hash_table = htab_init(HASH_TABLE_DIMENSION); \
    if (hash_table == NULL) \
        return error_exit("Nepovedlo se alokovat tabulku do paměti!\n"); \
    /* testování funkcionality tabulky na 200 číslech*/ \
    for (int i = 0; i < 200; i++) { \
        char *new_word; \
        new_word = malloc(MAX_WORD_LEN); \
        sprintf(new_word, "%d", rand() % 50); \
        bool free_word = false; \
        /* pokud bude identifikátor nalezen, bude později odstraněn*/ \
        if(htab_find(hash_table, new_word) != NULL) { \
            free_word = true; \
        } else { \
            /* vytvoří nový záznam v tabulce*/ \
            if (htab_lookup_add(hash_table, new_word) == NULL) \
                return error_exit("Chyba při allokaci paměti pro slovo '%s'!\n", new_word); \
        } \
        /* alokovaný identifikátor již existuje v tabulce a může být odstraněn*/ \
        if (free_word)  \
            free(new_word);   \
    }

#define MAKE_SYMTABLE() \
    stack_t *stack = stack_init(STACK_SIZE); \
    if (stack == NULL) \
        return error_exit("Nepovedlo se alokovat zásobník do paměti!\n"); \
 \
    CREATE_HTAB(hash_table0) \
 \
    stack_push(&stack, hash_table0); \
 \
    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_return"); \
    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val_type"); \
    fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val"); \
     \
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_return"); \
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_val_type"); \
    fce_item_push(&hash_table0->ptr_arr[1]->next_h_item->fce, "f_va"); \
 \
    stack_print(stack);

    

#define MAKE_DEF_TABLE() \
    def_table_t *deftable = def_table_init();\
    def_table_add(deftable, "fce1", 0);\
    def_table_add(deftable, "fce2", 0);\
    def_table_add(deftable, "fce3", 0);\
    def_table_add(deftable, "fce4", 0);\
    def_table_add(deftable, "fce5", 0);\
    def_table_add(deftable, "fce6", 0);\
    def_table_add(deftable, "fce7", 1);\
    def_table_add(deftable, "fce8", 1);\
    def_table_add(deftable, "fce9", 1);\
    def_table_add(deftable, "fce10", 0);\
    def_table_add(deftable, "fce11", 0);\
    def_table_add(deftable, "fce8", 0);\
\
    def_table_print(*deftable);


#define MAKE_LEX() \
    tToken *token = NULL;\
    \
    token = malloc(sizeof(struct Token));\
    if(token == NULL){\
        fprintf(stderr, "ERROR: Failed malloc");\
        return 1;\
    } \
    \
    initToken(token);\
\
    int length = TOKEN_LENGTH;\
    token->attribute = malloc(length * sizeof(char));\
    if(token->attribute == NULL){\
        fprintf(stderr, "ERROR: Failed malloc");\
        return 1;\
    }\
\
    token->type = malloc(15 * sizeof(char));\
    if(token->type == NULL){\
        fprintf(stderr, "ERROR: Failed malloc");\
        return 1;\
    }\
    token->line = 1;\
    \
    \
    while(c != EOF){\
        if(!scanner(token)){\
            printToken(token);\
        }\
    }\
    deleteToken(token);

    

#endif // __TEST_H__