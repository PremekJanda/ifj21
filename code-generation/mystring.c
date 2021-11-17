/**
 * @file mystring.c
 * @authors David Kocman  - xkocma08, VUT FIT
 *          Radomír Bábek - xbabek02, VUT FIT
 *          Martin Ohnút  - xohnut01, VUT FIT
 *          Přemek Janda  - xjanda28, VUT FIT
 * @brief Small library of functions, that work with string buffers
 * @version 0.1
 * @date 2021-11-13
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

int buffer_init(buffer_t*buffer){
    buffer->capacity = 2;
    if ((buffer->data = malloc(buffer->capacity)) != NULL){
        buffer->data[0] = '\0';
        return 1;
    }
    else {
        return 0;
    }
}

void buffer_destroy(buffer_t*buffer) {
    free(buffer->data);
}

int resize_if_needed(buffer_t*dst, int needed_capacity){
    if (dst->capacity < needed_capacity) { //resizes the capacity if needed
        if ((dst->data = realloc(dst->data, (dst->capacity = needed_capacity * 2))) == NULL)
            return 0;
    }
    return 1;
}

int strcat_realloc(buffer_t*dst, const char*src){
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1)) {
        return 0;
    }
    strcat(dst->data, src);
    return 1;
}

int strcat_beginning_realloc(buffer_t*dst, const char*src) {
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1)) {
        return 0;
    }

    int dst_len = strlen(dst->data) + 1;
    int src_len = strlen(src);
    memmove(dst->data + src_len, dst->data, dst_len);
    memcpy(dst->data, src, src_len);
    return 1;
}

int strcpy_realloc(buffer_t*dst, const char*src) {
    if (!resize_if_needed(dst, strlen(src))) {
        return 0;
    }
    strcpy(dst->data,src);
    return 1;
}

int strinbetween_realloc(buffer_t*dst, const char*src, size_t position) {
    if (!resize_if_needed(dst, strlen(dst->data) + strlen(src) + 1)) {
        return 0;
    }
    if (position > strlen(dst->data)) {
        strcat(dst->data, src);
    }
    else{
        int srclen = strlen(src);
        memmove(dst->data+srclen+position, dst->data+position, strlen(dst->data)+1-position);
        memcpy(dst->data+position,src,srclen);
    }
    return 1;
}

int strcat_format_realloc(buffer_t*dst, const char *fmt, ...){
    va_list args;
    buffer_t var;
    if (!buffer_init(&var))
        return 0;
    
    bool condition;
    do
    {
        condition = false;
        va_start(args, fmt);
        var.data[var.capacity-2] = '\0';
        if (vsnprintf(var.data, var.capacity, fmt, args) < 0){
            return 0;
        }
        va_end(args);
        if (var.data[var.capacity-2] != '\0') {
            if ((var.data = realloc(var.data, (var.capacity = var.capacity * 2))) == NULL){
                return 0;
            }  
            condition = true;
        }
    } while (condition);
    
    if (strcat_realloc(dst, var.data) == 0){
        buffer_destroy(&var);
        return 0;
    }
    else{
        buffer_destroy(&var);
        return 1;
    }
}

int strinbetween_format_realloc(buffer_t*dst, size_t index, const char *fmt, ...){
    va_list args;
    buffer_t var;
    if (!buffer_init(&var))
        return 0;
    
    bool condition;
    do
    {
        condition = false;
        va_start(args, fmt);
        var.data[var.capacity-2] = '\0';
        if (vsnprintf(var.data, var.capacity, fmt, args) < 0){
            return 0;
        }
        va_end(args);
        if (var.data[var.capacity-2] != '\0') {
            if ((var.data = realloc(var.data, (var.capacity = var.capacity * 2))) == NULL){
                return 0;
            }  
            condition = true;
        }
    } while (condition);
    
    if (strinbetween_realloc(dst, var.data, index) == 0){
        buffer_destroy(&var);
        return 0;
    }
    else{
        buffer_destroy(&var);
        return 1;
    }
}

int main(){
    buffer_t buffer;
    buffer_init(&buffer);
    strcpy_realloc(&buffer, "VAR \n");
    strcat_format_realloc(&buffer, "DEFVAR float@%a\n", 3.1415926);

    
    printf("%s", buffer.data);
    return 0;
}
