#ifndef _STR_H_

#define _STR_H_

#include <string.h>

/**
 * \file str.h Prototipos de operações com strings. 
 */

int str_compare(const void*, const void*);
char* str_clone(const char*);
int str_hash_code(char*);
void str_destroy(void*);
void str_trim(char*);
void str_to_lower(char*);
void str_to_upper(char*);
void str_remove_tags(char*);

#endif
