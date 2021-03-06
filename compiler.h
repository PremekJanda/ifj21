/**
 *  Soubor: compiler.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	07. 12. 2021 13:03:59
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: ORozhraní propojující jednotlivé komponenty
 */

// lexikální analýza
#include "lexikon.h"

// syntaktická analýza
#include "parser.h"

// sémantická analýza
#include "semantic.h"

// generování kódu
#include "code_generator.h"

// chybové návratové hodnoty
#define LEXICAL_ERROR 1
#define SYNTAX_ERROR 2
#define SEMANTIC_ERROR 3
#define CODE_GENERATION_ERROR 4
#define ALLOC_ERROR 99


#define RETURN_ERROR(__err_code) \
    if (error_code == 1) { \
        tree_delete(ast_root_node); \
        return LEXICAL_ERROR; \
    } else if (error_code == 2) { \
        tree_delete(ast_root_node); \
        return SYNTAX_ERROR; \
    } else if (error_code != 0) { \
        symtable_free(sym_table); \
        tree_delete(ast_root_node); \
        return error_code; \
    }

#define FREE_MEMORY \
    buffer_destroy(&code.text); \
    symtable_free(sym_table); \
    tree_delete(ast_root_node);
    