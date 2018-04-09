/**
 * @file ctest_testfn.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Apr 2018
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

test_t* test_t_init() {
    test_t* t   = malloc(sizeof(test_t));
    t->failed   = false;
    t->err      = 0;
    t->fail_msg = NULL;
    t->err_msg  = NULL;

    /* initialize all time values to 0 */
    t->start     = calloc(1, sizeof(struct timespec));
    t->end       = calloc(1, sizeof(struct timespec));
    t->failed_at = calloc(1, sizeof(struct timespec));
    t->error_at  = calloc(1, sizeof(struct timespec));

    /* set alternative interface for fail, error, done cases (more OO-like) */
    t->fail  = &test_fail;
    t->error = &test_error;
    t->done  = &test_done;

    return t;
}

int test_t_del(test_t* t) {
    if (t == NULL) return EXIT_FAILURE;

    if (t->fail_msg != NULL) {
        free(t->fail_msg);
    }
    if (t->err_msg != NULL) {
        for (int i = 0; i < t->err; i++) {
            if (t->err_msg[i] != NULL) {
                free(t->err_msg[i]);
            }
        }
        free(t->err_msg);
    }

    free(t->start);
    free(t->end);
    free(t->failed_at);
    free(t->error_at);

    free(t);

    return EXIT_SUCCESS;
}

void test_fail(test_t* t, char* msg) {
    t->failed   = true;
    t->fail_msg = calloc(strlen(msg) + 1, sizeof(char));
    strncpy(t->fail_msg, msg, strlen(msg));

    timespec_get(t->failed_at, TIME_UTC);

    return;
}

void test_error(test_t* t, char* msg) {
    t->err++;

    char** temp = realloc(t->err_msg, sizeof(char*) * (t->err));
    if (!temp) {
        errno = ENOMEM;
        return;
    }
    t->err_msg = temp;

    t->err_msg[t->err - 1] = calloc(strlen(msg) + 1, sizeof(char));
    strncpy(t->err_msg[t->err - 1], msg, strlen(msg));

    timespec_get(t->error_at, TIME_UTC);

    return;
}

void test_start(test_t* t) {
    timespec_get(t->start, TIME_UTC);

    return;
}

void test_done(test_t* t) {
    timespec_get(t->end, TIME_UTC);

    return;
}