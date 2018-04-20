/* main.c
 *
 * Example program demonstrating libtdd usage and features.
 * This program uses all the public features of the library.
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tdd.h"

static void* test_errfunc(void* t);
static void* test_failfunc(void* t);
static void* test_timer(void* t);
static void* bench_func(void* t);
static void* test_segvfunc(void* t);

int main(int argc, char* argv[]) {
    suite_t* s = suite_init();

    // suite_add is a variadic function that can be used to add an arbitrary
    // number of tests at once
    // new tests are added with a function pointer, a name, and an optional
    // description; if the description is to be omitted, you may pass NULL
    suite_add(s, 2,
              newtest(&test_timer, "test_timer",
                      "Manual benchmark. Requires timespan to be printed "
                      "manually."),
              newtest(&bench_func, "bench_func",
                      "Builtin benchmark. Execution timespan is printed "
                      "automatically below."));

    // suite_addtest is a function that simply appends a test to the list of
    // tests in the suite; could be used to programmatically add tests
    suite_addtest(s, newtest(&test_errfunc, "test_errfunc", "Raises error."));

    // suite_add can be called multiple times to add groups of tests to the
    // test suite
    suite_add(s, 2,
              newtest(&test_failfunc, "test_failfunc", "Fails immediately."),
              newtest(&test_segvfunc, "test_segvfunc", "Raises SIGSEGV."));

    printf("Running tests ignoring failures.\n");

    suite_run(s, false);
    if (s->finished) {
        printf("Suite ran all tests.\n");
    } else {
        printf("Suite only ran %d tests.\n", s->test_index + 1);
    }
    printf("Suite encountered: %d segmentation faults.\n", s->n_segv);
    printf("\n");

    // reset the test suite to demonstrate that it can be rerun with fatal
    // failures
    suite_reset(s);

    printf("Running tests with fatal failures.\n");
    suite_run(s, true);
    if (s->finished) {
        printf("Suite ran all tests.\n");
    } else {
        printf("Suite only ran %d tests.\n", s->test_index + 1);
    }

    // print summary statistics
    suite_stats_t* stats = suite_stats(s);
    printf("Suite encountered: %d segmentation faults.\n", s->n_segv);

    // delete the suite and all testfns associated with it
    suite_del(s);

    int ret = stats->n_fail;
    suite_delstats(stats);

    return ret;
}

void* test_errfunc(void* t) {
    test_error(t, "a non-critical error ocurred.");
    return NULL;
}

void* test_failfunc(void* t) {
    test_fail(t, "a critical error occurred!");
    printf("this code should not run unless fatal failures are disabled!\n");
    return NULL;
}

void* test_timer(void* t) {
    test_start(t);
    char* s = calloc(128, sizeof(char));
    strcpy(s, "This function is being timed!");
    free(s);
    test_done(t);
    return NULL;
}

void* bench_func(void* t) {
    char* s = calloc(128, sizeof(char));
    strcpy(s, "This function is being timed!");
    free(s);
    return NULL;
}

void* test_segvfunc(void* t) {
    raise(SIGSEGV);
    return NULL;
}
