/**
 * @private
 * @file strutil.h
 * @author Keefer Rourke <mail@krourke.org
 * @brief String format and manipulation functions and macros for libtdd.
 */
#ifndef __TDD_STRING_UTIL_H__
#define __TDD_STRING_UTIL_H__

#include <stdio.h>
#include <stdlib.h>

/**
 * __INDENT is a macro that prints n space characters to the FILE* f.
 *
 * @private
 * @param f - the f to print the spaces to
 * @param n - the number of spaces to print to the file
 **/
#define __INDENT(f, n) fprintf(f, "%.*s", n, " ")

/**
 * __hasprefix() detects if the character array str is prefixed by the
 * character array pre.
 *
 * @param str - the string for which to check the prefix
 * @param pre - the prefix to check for
 * @return 1 if the string has the prefix, 0 otherwise
 * @private
 */
int  __hasprefix(char* str, char* pre);

/**
 * __print_error() will print the specified string formatted as an error to
 * the file f.
 *
 * @param str - the error message
 * @param f   - the file to print the error to
 * @private
 */
void __print_error(FILE* f, char* str);
/**
 * __print_warning() will print the specified string formatted as a warning to
 * the file f.
 *
 * @param str - the warning message
 * @param f   - the file to print the warning to
 * @private
 */
void __print_warning(FILE* f, char* str);
/**
 * __print_success() will print the specified string formatted as a success to
 * the file f.
 *
 * @param str - the message
 * @param f   - the file to print the message to
 * @private
 */
void __print_success(FILE* f, char* str);
/**
 * __print_description() will print the specified string formatted as a
 * description to the file f.
 *
 * @param str - the description
 * @param f   - the file to print the message to
 * @private
 */
void __print_desc(FILE* f, char* str);
/**
 * __print_hilite() will print the specified string formatted with highliting
 * to the file f.
 *
 * @param str - the description
 * @param f   - the file to print the message to
 * @private
 */
void __print_hilite(FILE* f, char* str);

#endif
