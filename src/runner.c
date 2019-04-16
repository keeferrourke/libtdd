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

    runner_t* runner = malloc(sizeof(runner_t));
    if (runner == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    runner->name = calloc(strlen(name) + 1, sizeof(char));
    if (desc != NULL) {
        runner->desc = calloc(strlen(desc) + 1, sizeof(char));
    } else {
        runner->desc = calloc(1, sizeof(char)); /* empty string */
    }
    runner->fn = f;

    if (runner->name == NULL || runner->desc == NULL) {
        errno = ENOMEM;
        if (runner->name) {
            free(runner->name);
        }
        if (runner->desc) {
            free(runner->desc);
        }
        free(runner);
        return NULL;
    }

    strncpy(runner->name, name, strlen(name));
    if (desc != NULL) {
        strncpy(runner->desc, desc, strlen(desc));
    }

    return runner;
}

int tdd_runner_del(runner_t* runner) {
    if (runner == NULL) return EXIT_FAILURE;

    if (runner->name != NULL) {
        free(runner->name);
    }
    if (runner->desc != NULL) {
        free(runner->desc);
    }
    free(runner);

    return EXIT_SUCCESS;
}
