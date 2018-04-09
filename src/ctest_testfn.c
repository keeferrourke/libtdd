/**
 * @file ctest.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Jan 2018
 * @brief This file contains implementation details of functions pertaining to
 *        using test_t structures for managing simple test suites.
 **/
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ctest.h"

testfn* newtest(void (*f)(test_t* t), char* name, char* desc) {
    if (name == NULL || desc == NULL || f == NULL) {
        return NULL;
    }

    testfn* tf = malloc(sizeof(testfn));
    if (tf == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    tf->name = calloc(strlen(name) + 1, sizeof(char));
    tf->desc = calloc(strlen(desc) + 1, sizeof(char));
    tf->fn   = f;

    if (tf->name == NULL || tf->desc == NULL) {
        errno = ENOMEM;
        if (tf->name) {
            free(tf->name);
        }
        if (tf->desc) {
            free(tf->desc);
        }
        return NULL;
    }

    strncpy(tf->name, name, strlen(name));
    strncpy(tf->desc, desc, strlen(desc));

    return tf;
}

int testfn_del(testfn* tf) {
    if (tf == NULL) return EXIT_FAILURE;

    free(tf->name);
    free(tf->desc);
    free(tf);

    return EXIT_SUCCESS;
}
