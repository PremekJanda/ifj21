/**
 *  Soubor: compiler.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	25. 11. 2021 01:11:58
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Zapouzdřuje veškerou logiku překladače od lexikální analýzy až po generování kódu
 */

#include "compiler.h"

int main() {
    // návratový kód chyby
    int error_code;

    // ukazatel na kořenový prvek abstraktního syntaktického stromu
    t_node *ast_root_node;
    ast_root_node = malloc(sizeof(t_node));
    if (!node_init(ast_root_node))
        return ALLOC_ERROR;

    // zde proběhne lexikální a syntaktická analýza
    error_code = syntax_analyzer(ast_root_node);

    // lexikální chyba
    if (error_code == 1)
        return LEXICAL_ERROR; 
    
    // syntakrická chyba
    if (error_code == 2)
        return SYNTAX_ERROR; 
    
    // sémantická analýza
    error_code = semantic(ast_root_node);

    // pokud není úspěšná navrátí se její chybový kód
    if (!error_code)
        return error_code;

    // TODO
    // code_generation();
    
    // uvolnění alokované paměti
    tree_print(*ast_root_node, 0);
    node_delete(ast_root_node);

    return EXIT_SUCCESS;
}