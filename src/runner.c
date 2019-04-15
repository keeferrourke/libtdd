/**
 * @file runner.c
 * @author Keefer Rourke <mail@krourke.org>
 * @brief This file contains implementation details of functions pertaining to
 *        using runner_t structures for creating simple test suites.
 **/
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tdd.h"

runner_t* runner_new(void* (*f)(void* t), char* name, char* desc) {
    if (name == NULL || f == NULL) {
        return NULL;
    }

    runner_t* tf = malloc(sizeof(runner_t));
    if (tf == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    tf->name = calloc(strlen(name) + 1, sizeof(char));
    if (desc != NULL) {
        tf->desc = calloc(strlen(desc) + 1, sizeof(char));
    } else {
        tf->desc = calloc(1, sizeof(char)); /* empty string */
    }
    tf->fn = f;

    if (tf->name == NULL || tf->desc == NULL) {
        errno = ENOMEM;
        if (tf->name) {
            free(tf->name);
        }
        if (tf->desc) {
            free(tf->desc);
        }
        free(tf);
        return NULL;
    }

    strncpy(tf->name, name, strlen(name));
    if (desc != NULL) {
        strncpy(tf->desc, desc, strlen(desc));
    }

    return tf;
}

int tdd_runner_del(runner_t* tf) {
    if (tf == NULL) return EXIT_FAILURE;

    if (tf->name != NULL) {
        free(tf->name);
    }
    if (tf->desc != NULL) {
        free(tf->desc);
    }
    free(tf);

    return EXIT_SUCCESS;
}
