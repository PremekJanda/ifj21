/**
 *  Soubor: test.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:	24. 11. 2021 04:32:31
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Testovací rozhraní obsahující makra pro generování částí kódů
 */

// následující řádky zabrání násobnému vložení:
#include "tree.h"
#include "semantic.h"

#ifndef __TEST_H__
#define __TEST_H__



#define NODE_INIT(node, type, attribute) \
    t_node node; \
    node_init(&node); \
    node_setdata(&node, type, 0); \
    node_setdata(&node, attribute, 1);

#define MAKE_TREE() \
    NODE_INIT(node1,  "<prog>", "") \
     \
    NODE_INIT(node2,  "keyword", "require") \
    NODE_INIT(node3,  "string", "ifj21") \
    NODE_INIT(node4,  "<main-list>", "") \
 \
    NODE_INIT(node5,  "<def-decl-fcall>", "") \
     \
    NODE_INIT(node6,  "<main-list>", "") \
 \
    NODE_INIT(node7,  "keyword", "function") \
    NODE_INIT(node8,  "id", "main") /* definice funkce */\
    NODE_INIT(node9,  "<f-arg-list>", "") \
    NODE_INIT(node10, "<return-types>", "") \
    NODE_INIT(node11, "<stmt-list>", "") \
    NODE_INIT(node12, "keyword", "end") \
 \
    NODE_INIT(node13, "<def-decl-fcal>", "") \
    NODE_INIT(node14, "<main-list>", "") \
     \
    NODE_INIT(node15, "eps", "") \
 \
    NODE_INIT(node16, "eps", "") \
 \
    NODE_INIT(node17, "<stmt>", "") \
    NODE_INIT(node18, "<stmt-list>", "") \
 \
    NODE_INIT(node19, "id", "main") /* volání fce /*/\
    NODE_INIT(node20, "<item-list>", "") \
 \
    /*NODE_INIT(node21, "eps", "") */ /* na node21 přidána deklarace proměnné */\
 \
    NODE_INIT(node22, "<decl-local>", "") \
 \
    NODE_INIT(node23, "<stmt>", "") \
    NODE_INIT(node24, "<stmt-list>", "") \
 \
    NODE_INIT(node25, "eps", "") \
 \
    NODE_INIT(node26, "keyword", "local") \
    NODE_INIT(node27, "id", "s1") \
    NODE_INIT(node28, "DataAssign", ":") \
    NODE_INIT(node29, "<type>", "") \
    NODE_INIT(node30, "<decl-assign>", "") \
 \
    NODE_INIT(node31, "id", "write") \
    NODE_INIT(node32, "<assign-or-fcall>", "") \
 \
    NODE_INIT(node33, "<stmt>", "") \
    NODE_INIT(node34, "<stmt-list>", "") \
 \
    NODE_INIT(node35, "keyword", "string") \
 \
    NODE_INIT(node36, "assign", "=") \
    NODE_INIT(node37, "<item>", "") \
 \
    NODE_INIT(node38, "<item-list>", "") \
 \
    NODE_INIT(node39, "<if>", "") \
 \
    NODE_INIT(node40, "eps", "") \
 \
    NODE_INIT(node41, "expr", "") \
 \
    NODE_INIT(node42, "<item>", "") \
    NODE_INIT(node43, "<another-item>", "") \
 \
    NODE_INIT(node44, "keyword", "if") \
    NODE_INIT(node45, "<cond>", "") \
    NODE_INIT(node46, "keyword", "than") \
    NODE_INIT(node47, "<stmt-list>", "") \
    NODE_INIT(node48, "keyword", "else") \
    NODE_INIT(node49, "<stmt-list>", "") \
    NODE_INIT(node50, "keyword", "end") \
 \
 \
 \
    node_addnext(&node1, &node2); \
    node_addnext(&node1, &node3); \
    node_addnext(&node1, &node4); \
 \
    node_addnext(node1.next[2], &node5); \
    node_addnext(node1.next[2], &node6); \
 \
    node_addnext(node1.next[2]->next[0], &node7); \
    node_addnext(node1.next[2]->next[0], &node8); \
    node_addnext(node1.next[2]->next[0], &node9); \
    node_addnext(node1.next[2]->next[0], &node10); \
    node_addnext(node1.next[2]->next[0], &node11); \
    node_addnext(node1.next[2]->next[0], &node12); \
 \
    node_addnext(node1.next[2]->next[1], &node13); \
    node_addnext(node1.next[2]->next[1], &node14); \
     \
    node_addnext(node1.next[2]->next[0]->next[2], &node15); \
 \
    node_addnext(node1.next[2]->next[0]->next[3], &node16); \
     \
    node_addnext(node1.next[2]->next[0]->next[4], &node17); \
    node_addnext(node1.next[2]->next[0]->next[4], &node18); \
 \
    node_addnext(node1.next[2]->next[1]->next[0], &node19); \
    node_addnext(node1.next[2]->next[1]->next[0], &node20); \
 \
    /*node_addnext(node1.next[2]->next[1]->next[1], &node21); */\
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0], &node22); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1], &node23); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1], &node24); \
 \
    node_addnext(node1.next[2]->next[1]->next[0]->next[1], &node25); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0], &node26); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0], &node27); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0], &node28); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0], &node29); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0], &node30); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[0], &node31); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[0], &node32); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1], &node33); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1], &node34); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0]->next[3], &node35); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0]->next[4], &node36); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0]->next[4], &node37); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[0]->next[1], &node38); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0], &node39); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[1], &node40); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[0]->next[0]->next[4]->next[1], &node41); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[0]->next[1]->next[0], &node42); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[0]->next[1]->next[0], &node43); \
 \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node44); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node45); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node46); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node47); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node48); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node49); \
    node_addnext(node1.next[2]->next[0]->next[4]->next[1]->next[1]->next[0]->next[0], &node50); \
 \
    NODE_INIT(node105,  "<def-decl-fcall>", "") \
    NODE_INIT(node106,  "<main-list>", "") \
 \
    NODE_INIT(node107,  "keyword", "global") \
    NODE_INIT(node108,  "id", "jméno_proměnné") \
    NODE_INIT(node109,  "DataAssign", ":") \
    NODE_INIT(node1010,  "<f-or-type>", "") \
 \
    NODE_INIT(node1011,  "<type>", "") \
    NODE_INIT(node1012,  "<decl-assign>", "") \
    \
    NODE_INIT(node1013,  "keyword", "string") \
    \
    NODE_INIT(node1014,  " equals", "=") \
    NODE_INIT(node1015,  "<item>", "") \
    \
    NODE_INIT(node1016,  "expr", "") \
    \
    NODE_INIT(node1017,  "eps", "") \
\
    node_addnext(node1.next[2]->next[1]->next[1], &node105); \
    node_addnext(node1.next[2]->next[1]->next[1], &node106); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0], &node107); \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0], &node108); \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0], &node109); \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0], &node1010); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[1], &node1017); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3], &node1011); \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3], &node1012); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3]->next[0], &node1013); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3]->next[1], &node1014); \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3]->next[1], &node1015); \
    \
    node_addnext(node1.next[2]->next[1]->next[1]->next[0]->next[3]->next[1]->next[0], &node1016); \
    \
    tree_print(node1, 0);



#define MAKE_SMALL_TREE() \
    NODE_INIT(node1,  "<prog>", "") \
     \
    NODE_INIT(node2,  "keyword", "require") \
    NODE_INIT(node3,  "string", "ifj21") \
    NODE_INIT(node4,  "<main-list>", "") \
 \
    NODE_INIT(node5,  "<def-decl-fcall>", "") \
    NODE_INIT(node6,  "<main-list>", "") \
    \
    NODE_INIT(node7,  "keyword", "global") \
    NODE_INIT(node8,  "id", "jméno_proměnné") \
    NODE_INIT(node9,  "DataAssign", ":") \
    NODE_INIT(node10,  "<f-or-type>", "") \
    \
    NODE_INIT(node11,  "<type>", "") \
    NODE_INIT(node12,  "<decl-assign>", "") \
    \
    NODE_INIT(node13,  "keyword", "string") \
    \
    NODE_INIT(node14,  " equals", "=") \
    NODE_INIT(node15,  "<item>", "") \
    \
    NODE_INIT(node16,  "expr", "") \
    \
    NODE_INIT(node17,  "eps", "") \
\
    node_addnext(&node1, &node2); \
    node_addnext(&node1, &node3); \
    node_addnext(&node1, &node4); \
\
    node_addnext(node1.next[2], &node5); \
    node_addnext(node1.next[2], &node6); \
    \
    node_addnext(node1.next[2]->next[0], &node7); \
    node_addnext(node1.next[2]->next[0], &node8); \
    node_addnext(node1.next[2]->next[0], &node9); \
    node_addnext(node1.next[2]->next[0], &node10); \
    \
    node_addnext(node1.next[2]->next[1], &node17); \
    \
    node_addnext(node1.next[2]->next[0]->next[3], &node11); \
    node_addnext(node1.next[2]->next[0]->next[3], &node12); \
    \
    node_addnext(node1.next[2]->next[0]->next[3]->next[0], &node13); \
    \
    node_addnext(node1.next[2]->next[0]->next[3]->next[1], &node14); \
    node_addnext(node1.next[2]->next[0]->next[3]->next[1], &node15); \
    \
    node_addnext(node1.next[2]->next[0]->next[3]->next[1]->next[0], &node16); \
    \
    tree_print(node1, 0);


#define MAKE_F_DECL_TREE() \
    NODE_INIT(node1,  "<prog>", "") \
     \
    NODE_INIT(node2,  "keyword", "require") \
    NODE_INIT(node3,  "string", "ifj21") \
    NODE_INIT(node4,  "<main-list>", "") \
 \
    NODE_INIT(node5,  "<def-decl-fcall>", "") \
    NODE_INIT(node6,  "<main-list>", "") \
    \
    NODE_INIT(node7,  "keyword", "global") \
    NODE_INIT(node8,  "id", "jméno_funkce") \
    NODE_INIT(node9,  "DataAssign", ":") \
    NODE_INIT(node10,  "<f-or-type>", "") \
    \
    NODE_INIT(node11,  "function", "") \
    NODE_INIT(node12,  "id", "") \
    NODE_INIT(node13,  "(", "(") \
    NODE_INIT(node14,  "<types>", "") \
    NODE_INIT(node15,  ")", ")") \
    NODE_INIT(node16,  "<return-types>", "") \
    \
    \
    NODE_INIT(node17,  "<type>", "") \
    NODE_INIT(node18,  "<type-list>", "") \
    \
    NODE_INIT(node19,  "keyword", "string") \
    \
    NODE_INIT(node20,  "<type>", "") \
    NODE_INIT(node21,  "<type-list>", "") \
    \
    NODE_INIT(node22,  "keyword", "string") \
    NODE_INIT(node23,  "eps", "") \
    \
    NODE_INIT(node24,  ":", ":") \
    NODE_INIT(node25,  "<type>", "") \
    NODE_INIT(node26,  "<type-list>", "") \
    \
    NODE_INIT(node27,  "keyword", "string") \
    \
    NODE_INIT(node28,  ",", ",") \
    NODE_INIT(node29,  "<type>", "") \
    NODE_INIT(node30,  "<type-list>", "") \
    \
    NODE_INIT(node31,  "keyword", "string") \
    NODE_INIT(node32,  "eps", "") \
    \
    NODE_INIT(node99,  "eps", "") \
\
    node_addnext(&node1, &node2); \
    node_addnext(&node1, &node3); \
    node_addnext(&node1, &node4); \
\
    node_addnext(node1.next[2], &node5); \
    node_addnext(node1.next[2], &node6); \
    \
    node_addnext(node1.next[2]->next[0], &node7); \
    node_addnext(node1.next[2]->next[0], &node8); \
    node_addnext(node1.next[2]->next[0], &node9); \
    node_addnext(node1.next[2]->next[0], &node10); \
    \
    node_addnext(node1.next[2]->next[1], &node99); \
    \
    node_addnext(node1.next[2]->next[0]->next[3], &node11); \
    node_addnext(node1.next[2]->next[0]->next[3], &node12); \
    node_addnext(node1.next[2]->next[0]->next[3], &node13); \
    node_addnext(node1.next[2]->next[0]->next[3], &node14); \
    node_addnext(node1.next[2]->next[0]->next[3], &node15); \
    node_addnext(node1.next[2]->next[0]->next[3], &node16); \
    \
    node_addnext(&node14, &node17); \
    node_addnext(&node14, &node18); \
    \
    node_addnext(&node17, &node19); \
    node_addnext(&node18, &node20); \
    node_addnext(&node18, &node21); \
    \
    node_addnext(&node20, &node22); \
    node_addnext(&node21, &node23); \
    \
    node_addnext(&node16, &node24); \
    node_addnext(&node16, &node25); \
    node_addnext(&node16, &node26); \
    \
    node_addnext(&node25, &node27); \
    \
    node_addnext(&node26, &node28); \
    node_addnext(&node26, &node29); \
    node_addnext(&node26, &node30); \
    \
    node_addnext(&node29, &node31); \
    \
    node_addnext(&node30, &node32); \
    \
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
            if (htab_lookup_add(hash_table, new_word, new_word, new_word, GLOBAL, 0) == NULL) \
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
    fce_item_t *item = malloc(strlen("f_return") + 1); \
    strcpy(item, "f_return"); \
    fce_item_push(&htab_find(hash_table0, "2")->fce, item); \
    /*fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val_type"); */\
    /*fce_item_push(&htab_find(hash_table0, "2")->fce, "f_val"); */\
     \
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