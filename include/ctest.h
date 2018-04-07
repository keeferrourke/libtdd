/**
 * @file ctest.h
 * @author Keefer Rourke <mail@krourke.org
 * @date 08 Jan 2018
 * @brief This header file contains function and structure definitions for a
 *        minimal testing library synchronous testing library.
 *
 * @mainpage
 * This library provides a simple framework for defining, organizing, and
 * running unit tests in C. It is inspired by the golang `testing` pkg.
 *
 * Some minimal boilerplate is required to write a decent test suite for a
 * software package, but this is straight-forward and can be quickly written
 * in a main() function. This boilerplate is an unfortunate requirement,
 * because for the moment, there is no good tooling that is akin to the
 * `go test <file>` idiom used in golang.
 *
 * In a typical testing binary, the main() function should initialize tests
 * and create any resource files, etc. that might be expected by tests.
 *
 * @section example Example usage of this library:
 *
 *```
 *  #include <stdlib.h>
 *  #include "ctest.h"
 *
 *  int main(int argc, char* argv[]) {
 *      // create test suite
 *      suite_t* s = suite_init();
 *
 *      // initalize tests
 *      // variadic func; add as many tests as needed
 *      suite_add(s, 2, newtest("error", &error_func),
 *                newtest("fail", &fail_func));
 *
 *      // run the test suite
 *      suite_run(s);
 *      stats = suite_stats(s);
 *      suite_del(s);
 *      ...
 *
 *      return stats.n_error
 *  }
 *  static void error_func(test_t* t) {
 *      test_error(t, "oops!");
 *  }
 *  static void fail_func(test_t* t) {
 *      test_fail(t, "badness!");
 *  }
 *```
 **/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Testing structure. This structure is the only parameter in all testing
 * functions. If at any point during a testing function, unexpected bevahiour
 * occurs or the test downright fails, you should call `test_error(t)` and
 * `test_fail(t)` respectively.
 **/
typedef struct test_s {
    /** failed is an integer flag specifying if the current test has failed
     * **/
    short failed;
    /** err is an integer flag specifying if the current test has
     * encountered an error **/
    short err;
    /** message is a message that is set by fail() or error() **/
    char* message;
    /** start is the timestamp at which the test was started **/
    time_t start;
    /** end is the timestamp at which the test was marked as finished **/
    time_t end;
    /** failed_at is the timestamp at which the test first experienced failure
     * or error **/
    time_t failed_at;

    /** fail() marks the test as failed with a message explaining the error
     * **/
    void (*fail)(char* msg);
    /** error() marks the test as having encountered an error **/
    void (*error)(char* msg);
} test_t;

/**
 * test_t_init() makes a new test function.
 **/
test_t* test_t_init();

/**
 * fail() marks the test as failed with a message.
 **/
void test_fail(test_t* t, char* msg);

/**
 * error() marks the test as having encountered an error.
 **/
void test_error(test_t* t, char* msg);

/**
 * test_done() marks the time at which the test finished; this may be useful
 *for benchmarking.
 **/
void test_done(test_t* t);

/**
 * Testing function.
 **/
typedef struct _testfn {
    char* name;
    void (*fn)(test_t* t);
} testfn;

/**
 * newtest() creates and returns a pointer to an initialized testfn.
 **/
testfn* newtest(char* name, void (*f)(test_t* t));

/**
 * Testing suite.
 **/
typedef struct suite_s {
    /** finished is an integer flag specifying if all tests have run **/
    short finished;
    /** n_tests is the number of tests in the suite **/
    int n_tests;
    /** cur_index is the index of the current test **/
    int cur_index;
    /** cur_test is the name of the current test **/
    char* cur_test;
    /** tests is an array of testfn* that make up the suite **/
    testfn** tests;
    /** results is an array of test_t* that detail testing results for each
     * element in tests **/
    test_t** results;
} suite_t;

/**
 * suite_init() creates and returns a new test suite.
 **/
suite_t* suite_init();

/**
 * suite_del() frees memory allocated to a test suite.
 **/
int suite_del(suite_t* s);

/**
 * suite_done() marks all the suite as having finished all tests.
 **/
void suite_done(suite_t* s);

/**
 * suite_add() adds n testfn to the suite.
 **/
void suite_add(suite_t* s, int n, ...);

/**
 * suite_addtest() adds a single testfn to the suite.
 **/
int suite_addtest(suite_t* s, testfn* f);

/**
 * suite_run() runs all tests in the test array.
 * @param s: the test suite to run
 * @param fatal_failures: non-zero values indicate that the suite should abort
 *                        on any test failure
 **/
int suite_run(suite_t* s, short fatal_failures);

/**
 * suite_next() runs the next test in the suite.
 * @param s: the test suite to run
 * @param fatal_failures: non-zero values indicate that the suite should abort
 *                        on any test failure
 **/
int suite_next(suite_t* s, short fatal_failures);

/**
 * Stats structure detailing results of test suite.
 **/
typedef struct stats_s {
    char** tests_run;
    int    n_tests;
    int    n_error;
    int    n_fail;
    int    n_ran;
} stats_t;

/**
 * suite_stats() returns a stats_t* detailing the results of the testing.
 **/
stats_t* suite_stats(suite_t* s);

/**
 * suite_delstats() frees memory allocates to a stats_t* returned by
 * suite_stats()
 **/
int suite_delstats(stats_t* stats);
