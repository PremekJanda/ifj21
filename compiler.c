/**
 *  Soubor: compiler.c
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	26. 11. 2021 09:17:08
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
    ast_root_node = (t_node *)malloc(sizeof(t_node));
    if (node_init(ast_root_node))
        return ALLOC_ERROR;

    // zde proběhne lexikální a syntaktická analýza
    error_code = syntax_analyzer(ast_root_node);

    // lexikální chyba
    RETURN_ERROR(LEXICAL_ERROR)
    
    // syntaktická chyba
    RETURN_ERROR(SYNTAX_ERROR)
    
    // sémantická analýza
    error_code = semantic(ast_root_node);

    // pokud není úspěšná navrátí se její chybový kód
    RETURN_ERROR(SEMANTIC_ERROR)
    
    // generování kódu
    code_t code;
    init_code(&code);

    generate_code(ast_root_node, &code);
    RETURN_ERROR(CODE_GENERATION_ERROR)
    
    // uvolnění alokované paměti
    tree_delete(ast_root_node);

    return EXIT_SUCCESS;
}