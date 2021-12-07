/**
 *  Soubor: compiler.c
 *
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Last modified:	07. 12. 2021 13:40:41
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Zapouzdřuje veškerou logiku překladače od lexikální analýzy až po generování kódu
 */

#include "compiler.h"
#include "tree.h"

int main()
{
    // návratový kód chyby
    int error_code;

    // ukazatel na kořenový uzel abstraktního syntaktického stromu
    t_node *ast_root_node;
    ast_root_node = (t_node *)malloc(sizeof(t_node));
    if (node_init(ast_root_node))
        return ALLOC_ERROR;

    // inicializace tabulky symbolů
    stack_t *sym_table = symtable_init(STACK_SIZE);
    if (sym_table == NULL)
        return ALLOC_ERROR;

    // zde proběhne lexikální a syntaktická analýza
    error_code = syntax_analyzer(ast_root_node);

    // lexikální chyba
    RETURN_ERROR(LEXICAL_ERROR)

    // syntaktická chyba
    RETURN_ERROR(SYNTAX_ERROR)

    // sémantická analýza
    error_code = semantic(ast_root_node, sym_table);

    // pokud není úspěšná navrátí se její chybový kód
    RETURN_ERROR(SEMANTIC_ERROR)

    // generování kódu
    code_t code;
    init_code(&code);

    generate_code(ast_root_node, &code, sym_table);
    RETURN_ERROR(CODE_GENERATION_ERROR)

    // uvolnění alokované paměti
    FREE_MEMORY

    return EXIT_SUCCESS;
}