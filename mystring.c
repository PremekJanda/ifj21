/**
 * @file mystring.c
 * @author Radomír Bábek - xbabek02, VUT FIT
 * @brief Small library of functions, that work with string buffers
 * @version 0.1
 * @date 2021-11-13
 * Last modified:	04. 12. 2021 15:34:17
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "mystring.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int buffer_init(buffer_t *buffer)
{
    buffer->capacity = 45;
    if ((buffer->data = malloc(buffer->capacity)) != NULL)
    {
        buffer->data[0] = '\0';
        return 1;
    }
    else
    {
        return 0;
    }
}

void buffer_destroy(buffer_t *buffer)
{
    free(buffer->data);
}

int resize_if_needed(buffer_t *dst, int needed_capacity)
{
    if (dst->capacity < needed_capacity)
    { // resizes the capacity if needed
        if ((dst->data = realloc(dst->data, (dst->capacity = needed_capacity * 2))) == NULL)
            return 0;
    }
    return 1;
}

int strcat_realloc(buffer_t *dst, const char *src)
{
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1))
    {
        return 0;
    }
    strcat(dst->data, src);
    return 1;
}

int strcat_beginning_realloc(buffer_t *dst, const char *src)
{
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1))
    {
        return 0;
    }

    int dst_len = strlen(dst->data) + 1;
    int src_len = strlen(src);
    memmove(dst->data + src_len, dst->data, dst_len);
    memcpy(dst->data, src, src_len);
    return 1;
}

int strcpy_realloc(buffer_t *dst, const char *src)
{
    if (!resize_if_needed(dst, strlen(src)))
    {
        return 0;
    }
    strcpy(dst->data, src);
    return 1;
}
int replace_all_chars_by_string(buffer_t *buffer, char c, char *string)
{
    char *ptr = NULL;
    buffer_t buffy;
    buffer_init(&buffy);
    ptr = buffer->data;
    int index;
    while ((ptr = strchr(ptr, c)) != NULL)
    {
        strcpy_realloc(&buffy, ptr + 1);
        if (*(buffy.data) != '\0')
        {
            *ptr = '\0';
            strcat_realloc(buffer, string);
            index = strlen(buffer->data);
            strcat_realloc(buffer, buffy.data);
            ptr = buffer->data + index;
        }
        else
        {
            if (!strcat_realloc(buffer, string))
            {
                buffer_destroy(&buffy);
                return 0;
            }
        }
        int i = strlen(string);
    }
    buffer_destroy(&buffy);
    return 1;
}

int strinbetween_realloc(buffer_t *dst, const char *src, size_t position)
{
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1))
    {
        return 0;
    }
    if (position > strlen(dst->data))
    {
        strcat(dst->data, src);
    }
    else
    {
        int srclen = strlen(src);
        memmove(dst->data + srclen + position, dst->data + position, strlen(dst->data) + 1 - position);
        memcpy(dst->data + position, src, srclen);
    }
    return 1;
}

int strcat_format_realloc(buffer_t *dst, const char *fmt, ...)
{
    va_list args;
    buffer_t var;
    if (!buffer_init(&var))
        return 0;

    bool condition;
    do
    {
        condition = false;
        va_start(args, fmt);
        var.data[var.capacity - 2] = '\0';
        if (vsnprintf(var.data, var.capacity, fmt, args) < 0)
        {
            return 0;
        }
        va_end(args);
        if (var.data[var.capacity - 2] != '\0')
        {
            if ((var.data = realloc(var.data, (var.capacity = var.capacity * 2))) == NULL)
            {
                return 0;
            }
            condition = true;
        }
    } while (condition);

    if (strcat_realloc(dst, var.data) == 0)
    {
        buffer_destroy(&var);
        return 0;
    }
    else
    {
        buffer_destroy(&var);
        return 1;
    }
}

int strinbetween_format_realloc(buffer_t *dst, size_t index, const char *fmt, ...)
{
    va_list args;
    buffer_t var;
    if (!buffer_init(&var))
        return 0;

    bool condition;
    do
    {
        condition = false;
        va_start(args, fmt);
        var.data[var.capacity - 2] = '\0';
        if (vsnprintf(var.data, var.capacity, fmt, args) < 0)
        {
            return 0;
        }
        va_end(args);
        if (var.data[var.capacity - 2] != '\0')
        {
            if ((var.data = realloc(var.data, (var.capacity = var.capacity * 2))) == NULL)
            {
                return 0;
            }
            condition = true;
        }
    } while (condition);

    if (strinbetween_realloc(dst, var.data, index) == 0)
    {
        buffer_destroy(&var);
        return 0;
    }
    else
    {
        buffer_destroy(&var);
        return 1;
    }
}

int replace_all_strings_by_string(buffer_t *b, char *orig, char *rep)
{
    char *ptr = b->data;
    int index = 0;
    char *rest_of_str = malloc(strlen(b->data) + 1);

    while ((ptr = strstr(ptr, orig)) != NULL)
    {
        ptr[0] = '\0';
        strcpy(rest_of_str, ptr + strlen(orig));
        index = ptr + strlen(rep) - b->data;

        strcat_realloc(b, rep);
        strcat_realloc(b, rest_of_str);

        ptr = b->data + index;
    }

    free(rest_of_str);
}

int append_file(buffer_t *buffer, char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        return 1;
    }
    char row[MAX_ROW_LENGTH];
    row[0] = '\0';
    row[MAX_ROW_LENGTH - 1] = '\0';

    while (fgets(row, 1000, f) != NULL)
    {
        if (row[MAX_ROW_LENGTH - 1] != '\0')
        {
            fprintf(stderr, "Error, field capacity exceeded in function append_file");
            return 1;
        }
        strcat_realloc(buffer, row);
    }

    fclose(f);
    return 0;
}