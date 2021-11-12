#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "table.h"


//creates empty cell
void cell_init(t_cell *cell)
{
    cell->count = 0;
    cell->capacity = 0;
    cell->value = NULL;
}

//deletes cell
void cell_delete(t_cell *cell)
{
    free(cell->value);
}

//adds char into cell value, returns 1 when failed
int cell_addchar(t_cell *cell, char character)
{
    cell->count++;
    if(cell->capacity == 0)
    {
        cell->capacity = 10;
        cell->value = malloc(sizeof(char) * cell->capacity);
    }
    else if(cell->capacity < cell->count)
    {
        cell->capacity = cell->capacity * 2;
        char *new_value = realloc(cell->value, sizeof(char) * cell->capacity);
        if(new_value == NULL)
            return 1;
        cell->value = new_value;
    }
    if(cell->value == NULL)
        return 1;
    cell->value[cell->count-1] = character;
    return 0;
}

//copies cell2 into cell1, returns 1 when failed
int cell_copy(t_cell *cell1, t_cell *cell2)
{
    cell1->count = 0;
    for(int i = 0; i < cell2->count; i++)
    {
        cell_addchar(cell1, cell2->value[i]);
    }
    if(cell1->value==NULL)
        return 1;
    return 0;
}

//prints cell value
void cell_print(t_cell cell, FILE *file)
{
    for(int i = 0; i < cell.count - 1; i++)
    {
        fprintf(file,"%c", cell.value[i]);
    }
}


//creates empty row
void row_init(t_row *row)
{
    row->count = 0;
    row->capacity = 0;
    row->cells = NULL;
}

//adds new cell into the row, returns 1 when failed
int row_addcell(t_row *row, t_cell cell)
{
    row->count++;
    if(row->capacity == 0)
    {
        row->capacity = 10;
        row->cells = malloc(sizeof(t_cell) * row->capacity);
    }
    else if(row->capacity < row->count)
    {
        row->capacity = row->capacity * 2;
        t_cell *new_cells = realloc(row->cells, sizeof(t_cell) * row->capacity);
        if(new_cells == NULL)
            return 1;
        row->cells = new_cells;
    }
    if(row->cells == NULL)
        return 0;
    cell_init(&row->cells[row->count-1]);
    if(cell_copy(&row->cells[row->count-1], &cell))
        return 1;
    return 0;
}

//deletes row
void row_delete(t_row *row)
{
    for(int i = 0; i < row->count; i++)
    {
        cell_delete(&row->cells[i]);
    }
    row->capacity = 0;
    row->count = 0;
    free(row->cells);
}

//copies row2 into row1, returns 1 when failed
int row_copy(t_row *row1, t_row row2)
{
    row_delete(row1);
    row_init(row1);
    row1->count = 0;
    for(int i = 0; i < row2.count; i++)
    {
        if(row_addcell(row1, row2.cells[i]))
            return 1;
    }
    return 0;
}

//prints all cells in row
void row_print(t_row row, char delim, FILE *file, bool last)
{
    for(int i = 0; i < row.count; i++)
    {
        cell_print(row.cells[i], file);
        if(i != row.count-1)
            fprintf(file,"%c",delim);
    }
    if(last != true)
        fprintf(file,"\n");
}

//creates empty table
void table_init(t_table *table)
{
    table->count = 0;
    table->capacity = 0;
    table->rows = NULL;
}

//adds row into table, returns 1 when failed
int table_addrow(t_table *table, t_row row)
{
    table->count++;
    if(table->capacity == 0)
    {
        table->capacity = 10;
        table->rows = malloc(sizeof(t_row) * table->capacity);
    }
    else if(table->capacity < table->count)
    {
        table->capacity = table->capacity * 2;
        t_row *new_rows = realloc(table->rows, sizeof(t_row) * table->capacity);
        if(new_rows == NULL)
            return 1;
        table->rows = new_rows;
    }
    if(table->rows == NULL)
        return 1;
    row_init(&table->rows[table->count-1]);
    if(row_copy(&table->rows[table->count-1],row))
        return 1;
    return 0;
}

//deletes whole table including all rows
void table_delete(t_table *table)
{
    for(int i = 0; i < table->count; i++)
    {
        row_delete(&table->rows[i]);
    }
    free(table->rows);
}

//prints all rows from the table
void table_print(t_table table, char delim)
{
    bool last = false;
    for(int i = 0; i < table.count; i++)
    {
        if(i+1 == table.count)
            last = true;
        row_print(table.rows[i], delim, stdout, last);
    }
}

char *table_find(t_table table,char *nonterminal, char *terminal)
{
    int line = 0;
    int column = 0;
    for(int i = 0; i < table.count; i++)
    {
        if(!strcmp(nonterminal, table.rows[i].cells[0].value))
        {
            line = i;
            break;
        }
    }
    if(line == 0)
        return 0;
    for(int i = 0; i < table.rows[0].count; i++)
    {
        if(!strcmp(terminal, table.rows[0].cells[i].value))
        {
            column = i;
            break;
        }
    }
    if(column == 0)
        return 0;
    return table.rows[line].cells[column].value;
}

int table_linelength(t_table table, int line)
{
    return table.rows[line].count;
}

char *table_getdata(t_table table, int line, int column)
{
    return table.rows[line].cells[column].value;
}

//read whole file and saves all data into the table, returns 1 when failed
int table_readfile(t_table *table, char filename[], char delim)
{
    FILE *file;
    file = fopen(filename, "r");
    char character;
    t_cell cell;
    t_row row;
    cell_init(&cell);
    row_init(&row);
    //reading every character from the file
    while((character = fgetc(file)) != EOF)
    {
        //when delim is found, adds cell to the row
        if(character == delim)
        {
            if(cell_addchar(&cell, '\0'))
                return 1;
            if(row_addcell(&row, cell))
                return 1;
            cell_delete(&cell);
            cell_init(&cell);
            continue;
        }
        //when end of the line is found, adds row to the table
        if(character == '\n')
        {
            if(cell_addchar(&cell, '\0'))
                return 1;
            if(row_addcell(&row, cell))
                return 1;
            if(table_addrow(table, row))
                return 1;

            cell_delete(&cell);
            row_delete(&row);
            cell_init(&cell);
            row_init(&row);
            continue;
        }
        if(character == '\r')
            continue;
        //adds character to the cell
        if(cell_addchar(&cell, character))
            return 1;
    }
    fclose(file);
    //adds last line to the table
    cell_addchar(&cell, '\0');
    row_addcell(&row, cell);
    table_addrow(table, row);
    cell_delete(&cell);
    row_delete(&row);
    return 0;
}
/*
int main(int argc, char **argv)
{
    //check whether arguments were entered
    if(argc == 1)
        return 1;

    char delim = ':';

    //creates new table
    t_table table;
    table_init(&table);

    //loads selected file to the table
    if(table_readfile(&table, argv[1], delim))
    {
        table_delete(&table);
        return 1;
    }
    //prints table
    table_print(table, delim);
    printf("\n");

    printf("%d\n", table_find(&table, "<prog>", "id"));

    //deletes table
    table_delete(&table);
    return 0;
}*/