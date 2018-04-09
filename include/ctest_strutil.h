/**
 * @file ctest_strutil.h
 * @author Keefer Rourke <mail@krourke.org
 * @date 08 Apr 2018
 * @brief String format and manipulation functions and macros for libctest.
 * @private
 */
#ifndef __CTEST_UTIL_H__
#define __CTEST_UTIL_H__

#include <stdlib.h>

int  __hasprefix(char* str, char* pre);
void __print_error(FILE* f, char* str);
void __print_warning(FILE* f, char* str);
void __print_success(FILE* f, char* str);
void __print_desc(FILE* f, char* str);
void __print_hilite(FILE* f, char* str);

#endif
