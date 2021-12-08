/**
 * @file mystring.h
 * @author Radomír Bábek - xbabek02, VUT FIT
 * @brief Small library of functions, that work with string buffers.
 *        Documentation was writen in english, before we have made decision to
 *        write all together in czech, hope it does not matter too much.
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	08. 12. 2021 01:08:05
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef __MY_STRING_H
#define __MY_STRING_H

// used when reading from a file I've personally written, so I know how large each rows are
#define MAX_ROW_LENGTH 1000

#include <stddef.h>

typedef struct
{
    int capacity;
    char *data;
} buffer_t;

/**
 * @brief Sets the initial capacity of a buffer.
 *
 * @param buffer Buffer to be inicialized
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int buffer_init(buffer_t *buffer);

/**
 * @brief Frees all the allocated memory by the buffer.
 *
 * @param buffer
 */
void buffer_destroy(buffer_t *buffer);

/**
 * @brief Concatenation that also resizes the buffer if needed.
 *
 * @param dst Buffer where the result will be stored
 * @param src String to be inserted to the end of the dst string
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int strcat_realloc(buffer_t *dst, const char *src);

/**
 * @brief Resizes the buffer, if needed capacity exceeds the capacity of the buffer.
 *
 * @param dst Buffer
 * @param needed_capacity Capacity that buffes needs to have at least to be able to execute following operations
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int resize_if_needed(buffer_t *dst, int needed_capacity);

/**
 * @brief Copies the value of string src to the dst string buffer.
 *
 * @param dst Destination, where the result string will be stored
 * @param src String to be copied
 * @return Returns 1 if success, 0 when there is not enough memory
 */
int strcpy_realloc(buffer_t *dst, const char *src);

/**
 * @brief Strcat using dynamic buffer and formated string
 *
 * @param dst Buffer, where the result will be stored
 * @param fmt Formated string to be concatenated to a dst string
 * @return 0 if fails, 1 at success
 */
int strcat_format_realloc(buffer_t *dst, const char *fmt, ...);

/**
 * @brief Appends the content of text file into the string buffer
 *
 * @param buffer Dynamic string buffer
 * @param filename Name of the file
 * @return 0 if success, 1 if file could not be opened
 */
int append_file(buffer_t *buffer, char *filename);

/**
 * @brief Replaces all strings in string by a given string
 *
 * @param b Dynamic buffer
 * @param orig original string
 * @param rep replacement
 *
 * @return Returns 1 if success, 0 when there is not enough memory
 */
int replace_all_strings_by_string(buffer_t *b, char *orig, char *rep);

#endif