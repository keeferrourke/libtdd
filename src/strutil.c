/**
 * @file strutil.c
 * @author Keefer Rourke <mail@krourke.org>
 * @brief String format and manipulation functions and macros.
 *        Supports colour printing if USE_COLOUR or USE_COLOR are defined and
 *        the output file is `stdout`.
 * @private
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutil.h"

/* set formattijng for output */
#if defined(USE_COLOUR) || defined(USE_COLOR)
/* using colour, so define macros as ANSI escape sequences */
#define TEXT_RESET "\033[0m"    /* Default text */
#define TEXT_RED "\033[31m"     /* Red */
#define TEXT_GREEN "\033[32m"   /* Green */
#define TEXT_YELLOW "\033[33m"  /* Yellow */
#define TEXT_MAGENTA "\033[35m" /* Magenta */
#define TEXT_CYAN "\033[36m"    /* Cyan */
#define TEXT_WHITE "\033[37m"   /* White */
#define TEXT_BOLD "\033[1m"     /* Bold */
#define TEXT_ULINE "\033[4m"    /* Underlined */
#define TEXT_DIM "\033[2m"      /* Dimmed text */

#define SUCCESS TEXT_RESET TEXT_GREEN
#define ERROR TEXT_RESET TEXT_BOLD TEXT_RED
#define WARNING TEXT_RESET TEXT_YELLOW
#define DESCRIBE TEXT_RESET TEXT_DIM
#define ATTENTION TEXT_RESET TEXT_CYAN

#else
/* not using colour, so define macros as empty strings to nullify effect */
#define TEXT_RESET ""
#define TEXT_RED ""
#define TEXT_GREEN ""
#define TEXT_YELLOW ""
#define TEXT_MAGENTA ""
#define TEXT_CYAN ""
#define TEXT_WHITE ""
#define TEXT_BOLD ""
#define TEXT_ULINE ""
#define TEXT_DIM ""

#define SUCCESS ""
#define ERROR ""
#define WARNING ""
#define DESCRIBE ""
#define ATTENTION ""

#endif

int __hasprefix(char* str, char* pre) {
    if (str == NULL || strlen(str) == 0 || pre == NULL) {
        return 0;
    }
    if (strlen(pre) == 0) {
        return 1;
    }

    int has = 1;
    for (unsigned int i = 0; i < strlen(pre); i++) {
        if (str[i] != pre[i]) {
            has = 0;
            break;
        }
    }

    return has;
}

void __print_error(FILE* f, char* str) {
    if (f == NULL || str == NULL) return;
    if (f == stdout) {
        fprintf(f, ERROR "%s" TEXT_RESET, str);
    } else {
        fprintf(f, "%s", str);
    }
    return;
}

void __print_warning(FILE* f, char* str) {
    if (f == NULL || str == NULL) return;
    if (f == stdout) {
        fprintf(f, WARNING "%s" TEXT_RESET, str);
    } else {
        fprintf(f, "%s", str);
    }
    return;
}

void __print_success(FILE* f, char* str) {
    if (f == NULL || str == NULL) return;
    if (f == stdout) {
        fprintf(f, SUCCESS "%s" TEXT_RESET, str);
    } else {
        fprintf(f, "%s", str);
    }
    return;
}

void __print_desc(FILE* f, char* str) {
    if (f == NULL || str == NULL) return;
    if (f == stdout) {
        fprintf(f, DESCRIBE "%s" TEXT_RESET, str);
    } else {
        fprintf(f, "%s", str);
    }
    return;
}

void __print_hilite(FILE* f, char* str) {
    if (f == NULL || str == NULL) return;
    if (f == stdout) {
        fprintf(f, ATTENTION "%s" TEXT_RESET, str);
    } else {
        fprintf(f, "%s", str);
    }
    return;
}
