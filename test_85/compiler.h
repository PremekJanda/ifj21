/**
 *  Soubor: semantic.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	25. 11. 2021 03:53:10
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
#include "code-generator.h"

// chybové návratové hodnoty
#define LEXICAL_ERROR 1
#define SYNTAX_ERROR 2
#define SEMANTIC_ERROR 3
#define GENERATION_ERROR 4
#define ALLOC_ERROR 99


#define RETURN_ERROR(__err_code) \
    if (error_code == 1) { \
        tree_delete(ast_root_node); \
        return LEXICAL_ERROR; \
    } else if (error_code == 2) { \
        tree_delete(ast_root_node); \
        return SYNTAX_ERROR; \
    } else if (error_code != 0) { \
        tree_delete(ast_root_node); \
        return error_code; \
    }
    