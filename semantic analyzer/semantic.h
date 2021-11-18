/**
 *  Soubor: semantic.h
 * 
 *  Předmět: IFJ - Implementace překladače imperativního jazyka IFJ21
 *  Poslední změna:18. 11. 2021 00:22:11
 *  Autoři: David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 *  Popis: Rozhraní pro semantickou analýzu
 */

// následující řádky zabrání násobnému vložení:
#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include <stdio.h>                  // size_t
#include <string.h>                 // string fce
#include <stdbool.h>                // bool
#include <stdlib.h>                 // malloc
#include <stdint.h>                 // uint32
#include <stdarg.h>                 // pro va_list

#include "symtable.h"

#include "test.h"


// 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp.
//      undefined variable
//      redefined variable
//      undefined function
//      redefined function
//      undefined reference to function
//      undefined reference to variable
//      non-initialized variable
// 4 - sémantická chyba v příkazu přiřazení (typová nekompatibilita).
//      type incompatibility
// 5 - sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot u volání funkce či návratu z funkce.
//      invalid parameter type
//      invalid return type
//      invalid number of returned values
//      no return ([type] expected)
// 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
//      incompatible types
// 7 - ostatní sémantické chyby.
//      index out of range

// ? Možné rozšíření warningů
//      unused function
//      unused variable

// TODO
// operáto # očekává string

#define SEM_OK 0
#define SEM_DEFINE 3
#define SEM_ASSIGN 4
#define SEM_FCE 5
#define SEM_TYPE 6
#define SEM_OTHER 7

// Vrací funkce co má v každé větvi
// Vrací funkce v každé větvi (if else musí vrátit v obou případech)
// Sedí typy
// Jasou proměnné definované
// Jsou redefinované

// pozor na popnutí globálního rámce
// přesunout deklaraci proměnné před while, aby nedocházelo k redefinování s každým cyklem 
// přepis proměnné na stejném rámci




// - - - - - - - - - - - - - - - - - - - - //
// - - - - Datové typy a struktury - - - - //
// - - - - - - - - - - - - - - - - - - - - //
// Typy



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
// - - - - Funkce  - - - - //
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





// - - - - - - - - - - - - - - - - //
// - - - -  Pomocné funkce - - - - //
// - - - - - - - - - - - - - - - - //

#endif // __SEMANTIC_H__