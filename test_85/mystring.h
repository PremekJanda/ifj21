/**
 * @file mystring.h
 * @author Radomír Bábek - xbabek02, VUT FIT
 * @brief Small library of functions, that work with string buffers.
 *        Documentation was writen in english, before we have made decision to 
 *        write all together in czech.
 * @version 0.1
 * @date 2021-11-13
 * Last Modified:	25. 11. 2021 02:22:51
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __MY_STRING_H
#define __MY_STRING_H

//used when reading from a file I've personally written, so I know how large each rows are
#define MAX_ROW_LENGTH 1000

#include <stddef.h>

typedef struct{
    int capacity;
    char*data;
}buffer_t;

/**
 * @brief Sets the initial capacity of a buffer.
 * 
 * @param buffer Buffer to be inicialized
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int buffer_init(buffer_t*buffer);

/**
 * @brief Frees all the allocated memory by the buffer.
 * 
 * @param buffer 
 */
void buffer_destroy(buffer_t*buffer);

/**
 * @brief Concatenation that also resizes the buffer if needed.
 * 
 * @param dst Buffer where the result will be stored
 * @param src String to be inserted to the end of the dst string
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int strcat_realloc(buffer_t*dst, const char*src);

/**
 * @brief Concatenation to the beginning of a string that also resizes the buffer if needed.
 * 
 * @param dst Buffer where the result will be stored
 * @param src String to be inserted to the beginning of the dst string
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int strcat_beginning_realloc(buffer_t*dst, const char*src);

/**
 * @brief Resizes the buffer, if needed capacity exceeds the capacity of the buffer.
 * 
 * @param dst Buffer
 * @param needed_capacity Capacity that buffes needs to have at least to be able to execute following operations
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int resize_if_needed(buffer_t*dst, int needed_capacity);

/**
 * @brief Inserts the string inside other string. Inserted string will start at the index position in the new string
 * 
 * @param dst Original string, the result will be stored here
 * @param src String to be inserted in between
 * @param position Starting position of src string
 * @return int Returns 1 if success, 0 when there is not enough memory
 */
int strinbetween_realloc(buffer_t*dst, const char*src, size_t position);

/**
 * @brief Copies the value of string src to the dst string buffer.
 * 
 * @param dst Destination, where the result string will be stored
 * @param src String to be copied
 * @return Returns 1 if success, 0 when there is not enough memory
 */
int strcpy_realloc(buffer_t*dst, const char*src);

/**
 * @brief Strcat using dynamic buffer and formated string
 * 
 * @param dst Buffer, where the result will be stored
 * @param fmt Formated string to be concatenated to a dst string
 * @return 0 if fails, 1 at success
 */
int strcat_format_realloc(buffer_t*dst, const char *fmt, ...);

/**
 * @brief Inserts the formated string inside other string. Inserted string will start at the index position in the new string
 * 
 * @param dst Original string, the result will be stored here
 * @param fmt Formated string
 * @param position Starting position of src string
 * @return Returns 1 if success, 0 when there is not enough memory
 */
int strinbetween_format_realloc(buffer_t*dst, size_t position, const char *fmt, ...);

/**
 * @brief Replaces all characters in string by given string
 * 
 * @param buffer Dynamic string buffer
 * @param c Char to be replaced
 * @param string String to replace char with
 * @return Returns 1 if success, 0 when there is not enough memory
 */
int replace_all_chars_by_string(buffer_t*buffer, char c, char*string);

/**
 * @brief Appends the content of text file into the string buffer
 * 
 * @param buffer Dynamic string buffer
 * @param filename Name of the file
 * @return 0 if success, 1 if file could not be opened
 */
int append_file(buffer_t*buffer, char*filename);

#endif