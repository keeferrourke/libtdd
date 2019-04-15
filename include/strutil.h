/**
 * @private
 * @file strutil.h
 * @author Keefer Rourke <mail@krourke.org
 * @date 08 Apr 2018
 * @brief String format and manipulation functions and macros for libtdd.
 */
#ifndef __TDD_STRING_UTIL_H__
#define __TDD_STRING_UTIL_H__

#include <stdio.h>
#include <stdlib.h>

/**
 * INDENT is a macro that prints 6 space characters to the FILE* x
 **/
#define INDENT(x) fprintf(x, "      ")

int  __hasprefix(char* str, char* pre);
void __print_error(FILE* f, char* str);
void __print_warning(FILE* f, char* str);
void __print_success(FILE* f, char* str);
void __print_desc(FILE* f, char* str);
void __print_hilite(FILE* f, char* str);

#endif
