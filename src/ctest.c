/**
 * @file testing.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Jan 2018
 * @brief This file contains implementation details of functions pertaining to
 *        using test_t structures for managing simple test suites.
 **/
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ctest.h"

/* test_t */
test_t* test_t_init() {
    test_t* t = NULL;
    t         = malloc(sizeof(test_t));

    t->failed  = 0;
    t->err     = 0;
    t->message = NULL;
    t->start   = 0; /* epoch... close to an uninitialized value as it gets */
    t->end     = 0; /* epoch... close to an uninitialized value as it gets */

    return t;
}

void test_fail(test_t* t, char* msg) {
    t->failed    = 1;
    t->err       = 1;
    t->message   = msg;
    t->failed_at = time(0);
    return;
}

void test_error(test_t* t, char* msg) {
    t->err       = 1;
    t->message   = msg;
    t->failed_at = time(0);
    return;
}

void test_done(test_t* t) {
    t->end = time(0);
    return;
}

/* testfn */
testfn* newtest(char* name, void (*f)(test_t* t)) {
    testfn* tf = NULL;
    tf         = malloc(sizeof(testfn));
    tf->name   = name;
    tf->fn     = f;
    return tf;
}

/* suite_t */
suite_t* suite_init() {
    suite_t* s   = NULL;
    s            = malloc(sizeof(suite_t));
    s->finished  = 0;
    s->n_tests   = 0;
    s->cur_index = 0;
    s->cur_test  = NULL;
    s->tests     = NULL;

    return s;
}

int suite_del(suite_t* s) {
    if (s == NULL) return EXIT_FAILURE;

    int i;
    for (i = 0; i < s->n_tests; i++) {
        free(s->tests[i]);
        free(s->results[i]);
    }
    free(s->tests);
    free(s->results);
    free(s);

    return EXIT_SUCCESS;
}

void suite_done(suite_t* s) {
    if (s == NULL) return;
    s->finished = 1;
    return;
}

void suite_add(suite_t* s, int n, ...) {
    if (s == NULL) return;

    s->n_tests += n;
    s->tests   = malloc(sizeof(testfn*) * n);
    s->results = malloc(sizeof(test_t*) * n);

    va_list ap;
    va_start(ap, n);
    int i;
    for (i = 0; i < n; i++) {
        s->tests[i] = va_arg(ap, testfn*);
    }
    va_end(ap);

    return;
}

int suite_addtest(suite_t* s, testfn* f) {
    if (s == NULL) return EXIT_FAILURE;

    s->n_tests++;

    /* reallocate array and add new testfn */
    testfn** tmp_tests = realloc(s->tests, sizeof(testfn*) * s->n_tests);
    if (tmp_tests == NULL) {
        if (s->tests != NULL) {
            free(s->tests);
        }
        errno = ENOMEM;
        return EXIT_FAILURE;
    }
    s->tests                 = tmp_tests;
    s->tests[s->n_tests - 1] = f;

    /* grow results array with the tests */
    test_t** tmp_results = realloc(s->results, sizeof(test_t*) * s->n_tests);
    if (tmp_results == NULL) {
        if (s->tests != NULL) {
            free(s->tests);
        }
        errno = ENOMEM;
        return EXIT_FAILURE;
    }
    s->results = tmp_results;

    return EXIT_SUCCESS;
}

int suite_run(suite_t* s, short fatal_failures) {
    if (s == NULL) return EXIT_FAILURE;

    int i;
    for (i = 0; i < s->n_tests; i++) {
        int res = suite_next(s, fatal_failures);
        if (res != 0) {
            return res;
        }
    }
    suite_done(s);

    return EXIT_SUCCESS;
}

int suite_next(suite_t* s, short fatal_failures) {
    if (s == NULL) return EXIT_FAILURE;

    /* set up test */
    test_t* t    = test_t_init();
    testfn* test = s->tests[s->cur_index];
    s->cur_test  = test->name;
    /* run test */
    test->fn(t);
    int left = (s->n_tests) - (s->cur_index + 1);
    /* keep record of test results */
    s->results[s->cur_index++] = t;

    /* get test data */
    if (t->failed != 0) {
        printf("failed in %s: %s\n", s->cur_test, t->message);
        if (fatal_failures != 0) {
            printf("aborted with %d tests remaining.\n", left);
            return EXIT_FAILURE;
        }
    } else if (t->err != 0) {
        printf("error occured in %s: %s\n", s->cur_test, t->message);
    } else {
        printf("test (%d) %s: appears to have passed.\n", s->cur_index,
               s->cur_test);
        printf("%d/%d tests remaining.\n", left, s->n_tests);
    }

    return EXIT_SUCCESS;
}

stats_t* suite_stats(suite_t* s) {
    stats_t* stats   = malloc(sizeof(stats_t));
    stats->tests_run = malloc(sizeof(char*) * (s->cur_index));

    int nerr = 0, nfail = 0;
    int i;
    for (i = 0; i < s->cur_index; i++) {
        testfn* t           = s->tests[i];
        stats->tests_run[i] = malloc(sizeof(char) * (strlen(t->name) + 1));
        strcpy(stats->tests_run[i], t->name);

        test_t* r = s->results[i];
        if (r->err != 0) nerr++;
        if (r->failed != 0) nfail++;
    }
    stats->n_tests = s->n_tests;
    stats->n_ran   = s->cur_index;
    stats->n_error = nerr;
    stats->n_fail  = nfail;

    return stats;
}

int suite_delstats(stats_t* stats) {
    if (stats == NULL) return EXIT_FAILURE;

    int i;
    for (i = 0; i < stats->n_ran; i++) {
        free(stats->tests_run[i]);
    }
    free(stats->tests_run);
    free(stats);
    return EXIT_SUCCESS;
}
