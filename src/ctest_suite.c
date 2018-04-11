/**
 * @file ctest_suite.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Apr 2018
 * @brief This file contains implementation details of functions pertaining to
 *        using suite_t structures for managing simple test suites.
 **/
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ctest.h"
#include "ctest_signals.h"
#include "ctest_strutil.h"
#include "ctest_timeutil.h"

suite_t* suite_init() {
    suite_t* s = malloc(sizeof(suite_t));
    if (s == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    s->finished   = false;
    s->n_tests    = 0;
    s->n_segv     = 0;
    s->test_index = 0;
    s->tests      = NULL;
    s->results    = NULL;

    return s;
}

void suite_reset(suite_t* s) {
    if (s == NULL) return;

    /* reset all suite data */
    s->finished   = false;
    s->test_index = 0;
    for (int i = 0; i < s->n_tests; i++) {
        test_t_del(s->results[i]);
        s->results[i] = NULL;
    }

    return;
}

int suite_del(suite_t* s) {
    if (s == NULL) return EXIT_FAILURE;

    for (int i = 0; i < s->n_tests; i++) {
        testfn_del(s->tests[i]);
        test_t_del(s->results[i]);
    }
    free(s->tests);
    free(s->results);
    free(s);

    return EXIT_SUCCESS;
}

void suite_done(suite_t* s) {
    if (s == NULL) return;
    s->finished = true;
    return;
}

void suite_add(suite_t* s, int n, ...) {
    if (s == NULL) return;

    /* save old index to perform update */
    int old_index = s->n_tests;

    /* update the test count and realloc test and result arrays */
    s->n_tests += n;
    testfn** tmp_tests = realloc(s->tests, sizeof(testfn*) * s->n_tests);
    if (!tmp_tests) {
        errno = ENOMEM;
        return;
    }
    s->tests = tmp_tests;

    test_t** tmp_results = realloc(s->results, sizeof(test_t*) * s->n_tests);
    if (!tmp_results) {
        errno = ENOMEM;
        return;
    }
    s->results = tmp_results;

    /* add tests from the va_list */
    va_list ap;
    va_start(ap, n);
    for (int i = old_index; i < s->n_tests; i++) {
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
        if (s->results != NULL) {
            free(s->results);
        }
        errno = ENOMEM;
        return EXIT_FAILURE;
    }
    s->results = tmp_results;

    return EXIT_SUCCESS;
}

int suite_run(suite_t* s, bool fatal_failures) {
    if (s == NULL) return EXIT_FAILURE;

    for (int i = 0; i < s->n_tests; i++) {
        int res = suite_next(s, fatal_failures);
        if (res != EXIT_SUCCESS) {
            return res;
        }
    }
    suite_done(s);

    return EXIT_SUCCESS;
}

int suite_next(suite_t* s, bool fatal_failures) {
    if (s == NULL) return EXIT_FAILURE;

    /* set up test */
    test_t* t     = test_t_init();
    testfn* test  = s->tests[s->test_index];
    bool    bench = false;
    if (__hasprefix(test->name, "bench_")) {
        bench = true;
    }
    int crash_count = __sigsegv_caught;

    struct sigaction sa;
    sa.sa_flags   = SA_SIGINFO;
    sa.sa_handler = &__test_sigsegv_handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        test_t_del(t);
        return EXIT_FAILURE;
    }

    /* run test */
    if (bench) {
        test_start(t);
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, test->fn, t) != 0) {
        fprintf(stderr, "Could not create thread!\n");
        test_t_del(t);
        return EXIT_FAILURE;
    }
    void* retval = NULL;
    if (pthread_join(thread, &retval) != 0) {
        fprintf(stderr, "Could not join with thread!\n");
        test_t_del(t);
        return EXIT_FAILURE;
    }
    //    test->fn(t);
    if (bench && t->end->tv_sec == 0 && t->end->tv_nsec == 0) {
        test_done(t);
    }
    if (crash_count != __sigsegv_caught) {
        t->failed = true;
        s->n_segv++;
        char* segv_msg = "encountered segmentation fault";
        if (t->fail_msg != NULL) {
            free(t->fail_msg);
        }
        t->fail_msg = calloc(strlen(segv_msg) + 1, sizeof(char));
        strncpy(t->fail_msg, segv_msg, strlen(segv_msg));
    }

    /* keep record of test results */
    s->results[s->test_index++] = t;

    /* print test results */
    int   left = (s->n_tests) - (s->test_index + 1);
    char* res =
        calloc(256 + strlen(test->name) + strlen(test->desc), sizeof(char));
    int ret = EXIT_SUCCESS;
    if (t->failed) {
        sprintf(res, "fail: test %d/%d (%s)\n", s->test_index, s->n_tests,
                test->name);
        __print_error(stdout, res);
        __print_desc(stdout, t->fail_msg);
        fprintf(stdout, "\n");
        if (fatal_failures != 0) {
            printf("aborted with %d tests remaining.\n", left);
            ret = EXIT_FAILURE;
        }
    } else if (t->err != 0) {
        sprintf(res, "err:  test %d/%d (%s) encountered %d errors.\n",
                s->test_index, s->n_tests, test->name, t->err);
        __print_warning(stdout, res);
        for (int i = 0; i < t->err; i++) {
            char* why = calloc(16 + strlen(t->err_msg[i]), sizeof(char));
            sprintf(why, "      %d. %s\n", i + 1, t->err_msg[i]);
            __print_desc(stdout, why);
            free(why);
        }
    } else {
        sprintf(res, "okay: test %d/%d (%s): ", s->test_index, s->n_tests,
                test->name);
        __print_success(stdout, res);
        __print_desc(stdout, test->desc);
        fprintf(stdout, "\n");
    }
    free(res);

    /* print benchmarking info */
    if (bench) {
        struct timespec tdiff = __timespec_minus(t->end, t->start);

        char* bench_info = calloc(64 + strlen(test->name), sizeof(char));
        sprintf(bench_info, "      bench: test (%s) took ", test->name);
        __print_desc(stdout, bench_info);
        char* bench_res = calloc(256, sizeof(char));
        sprintf(bench_res, "%lds %ldns\n", tdiff.tv_sec, tdiff.tv_nsec);
        __print_hilite(stdout, bench_res);
        free(bench_info);
        free(bench_res);
    }

    return ret;
}

suite_stats_t* suite_stats(suite_t* s) {
    if (s == NULL) return NULL;

    suite_stats_t* stats = malloc(sizeof(suite_stats_t));
    stats->tests_run     = malloc(sizeof(char*) * (s->test_index));

    int nerr = 0, nfail = 0;
    for (int i = 0; i < s->test_index; i++) {
        testfn* t           = s->tests[i];
        stats->tests_run[i] = malloc(sizeof(char) * (strlen(t->name) + 1));
        strcpy(stats->tests_run[i], t->name);

        test_t* r = s->results[i];
        if (r->err != 0) nerr++;
        if (r->failed != 0) nfail++;
    }
    stats->n_tests = s->n_tests;
    stats->n_ran   = s->test_index;
    stats->n_error = nerr;
    stats->n_fail  = nfail;

    return stats;
}

int suite_delstats(suite_stats_t* stats) {
    if (stats == NULL) return EXIT_FAILURE;

    for (int i = 0; i < stats->n_ran; i++) {
        free(stats->tests_run[i]);
    }
    free(stats->tests_run);
    free(stats);

    return EXIT_SUCCESS;
}
