/**
 * @file ctest.h
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Apr 2018
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
#ifndef __CTEST_H__
#define __CTEST_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

struct _testfn;
struct test_s;
struct suite_s;

/**
 * Testing structure. This structure is the only parameter in all testing
 * functions. If at any point during a testing function, unexpected bevahiour
 * occurs or the test downright fails, you should call `test_error(t)` and
 * `test_fail(t)` respectively.
 **/
typedef struct test_s {
    /**
     * failed is a boolean flag specifying if the current test has failed.
     **/
    bool failed;
    /**
     * err is a integer flag specifying the number of errors the current test
     * has encountered.
     **/
    int err;
    /**
     * fail_msg is a a message that is by test_fail() indicating the reason
     * for test failure. Heap allocated.
     **/
    char* fail_msg;
    /**
     * err_msg is an array of character strings that is appended to on each
     * call to test_error(). Each string in this array corresponds to the
     * reason for errors in order of occurance. Each string is Heap allocated.
     **/
    char** err_msg;
    /**
     * start is the timestamp at which the test was started. Heap allocated.
     **/
    struct timespec* start;
    /**
     * end is the timestamp at which the test was marked as done. Heap
     * allocated.
     **/
    struct timespec* end;
    /**
     * failed_at is the timestamp at which the test last experienced failure.
     * Heap allocated.
     **/
    struct timespec* failed_at;
    /**
     * error_at is the timestamp at which the test last experienced an error.
     * Heap allocated.
     **/
    struct timespec* error_at;
    /**
     * fail() marks the test as failed with a message explaining the reason
     * for failure.
     **/
    void (*fail)(struct test_s* t, char* msg);
    /**
     * error() marks the test as having encountered an error with a message
     * explaining the reason for the error
     **/
    void (*error)(struct test_s* t, char* msg);
    /** done() marks the test as finished **/
    void (*done)(struct test_s* t);
} test_t;

/**
 * test_t_init() makes a new test function. Not to be called explicitly.
 * @private
 * @return A pointer to a fully initialized test_t structure.
 **/
test_t* test_t_init();

/**
 * test_t_del() frees all memory associated with a test_t structure. Not to be
 * called explicitly.
 * @private
 * @param t - pointer to the test_t structure to be freed
 * @return int
 */
int test_t_del(test_t* t);

/**
 * test_fail() marks the test as failed with a message. Failures are
 * identified as critical errors that will not allow testing to continue.
 * Use test_fail() to catch fundamental errors in program function execution.
 * To be called within a testfn::fn. Alternatively may be called through the
 * test_t::fail interface.
 * @param t   - pointer to a test_t structure to capture the context of a
 *	        test failure
 * @param msg - character string indicating the reason for failure
 **/
void test_fail(test_t* t, char* msg);

/**
 * test_error() marks the test as having encountered an error. Errors are
 * identified as non-critical flaws in program function execution which do not
 * prevent continuation of testing. Use test_error() to record unexpected but
 * valid return values and similar flaws.
 * To be called within a testfn::fn. Alternatively may be called through the
 * test_t::error interface.
 * @param t   - pointer to a test_t structure to capture the context of a
 *	        test error
 * @param msg - character string indicating the reason for error
 **/
void test_error(test_t* t, char* msg);

/**
 * test_start() marks the time at which the test started. This may be useful
 * for benchmarking and should be called after any test setup code.
 * @param t - pointer to a test_t structure to capture the test finish time.
 **/
void test_start(test_t* t);

/**
 * test_done() marks the time at which the test finished. This may be useful
 * for benchmarking and should be called before any test teardown code.
 * @param t - pointer to a test_t structure to capture the test finish time.
 **/
void test_done(test_t* t);

/**
 * Testing function.
 **/
typedef struct _testfn {
    /**
     * name is a character string identifier for a test. It is usually just
     * the name of the function itself, such as "test_func".
     */
    char* name;
    /**
     * desc is a character string description for a test. It should be a
     * humanly readable explanation of what the test is performing.
     */
    char* desc;
    /**
     * fn is a pointer to a test function.
     * @param t - pointer to a test_t structure to capture context of test
     *		  results
     */
    void* (*fn)(void* t);
} testfn;

/**
 * newtest() creates and returns a pointer to an initialized testfn.
 * @param f    - a pointer to a function that records information about a test
 *		 within its single `test_t*` argument
 * @param name - a character string identifier for the test; usually the name
 *		 of the test function itself
 * @param desc - a human readable description of the test
 * @return A pointer to a fully initialized testfn structure.
 **/
testfn* newtest(void* (*f)(void* t), char* name, char* desc);

/**
 * int testfn_del() frees memory allocated to a test function.
 * @param tf - a pointer to a testfn that is to be destroyed
 * @return EXIT_SUCCESS, otherwise EXIT_FAILURE if tf is NULL.
 **/
int testfn_del(testfn* tf);

/**
 * Testing suite. Contains all tests, current runtime state, and the results
 * of each test. May be used to contruct a suite_stats_t after running.
 **/
typedef struct suite_s {
    /**
     * finished is a boolean flag specifying if all tests have run. If this
     * flag is not set, then the suite aborted testing with a fatal failure
     * from one of the tests in the suite.
     **/
    bool finished;
    /** n_tests is the number of tests in the suite. **/
    int n_tests;
    /** n_segv is the number of segmentation faults that were caught. **/
    int n_segv;
    /** test_index is the index of the current test. **/
    int test_index;
    /** tests is an array of testfn* that make up the suite. **/
    testfn** tests;
    /**
     * results is an array of test_t* that details testing results for each
     * element in tests.
     **/
    test_t** results;
} suite_t;

/**
 * Stats structure detailing results of test suite.
 **/
typedef struct suite_stats_s {
    char** tests_run;
    int    n_tests;
    int    n_error;
    int    n_fail;
    int    n_ran;
} suite_stats_t;

/**
 * suite_init() creates and returns a new test suite.
 * @return A pointer to a fully intialized suite_t structure.
 **/
suite_t* suite_init();

/**
 * suite_reset() resets a suite_t to its initial state, as if it were never
 * run.
 * @param s - a pointer to a suite_t test suite to be reset
 **/
void suite_reset(suite_t* s);

/**
 * suite_del() frees memory allocated to a test suite.
 * @param s - a pointer to a suite_t test suite that is to be destroyed
 * @return
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
 * @param fatal_failures: true indicates that the suite should abort testing
 *			  if any test was marked as a failure
 * @return `EXIT_SUCCESS`, or, if `fatal_failures` is true, `EXIT_FAILURE`
 *	   after the first failed test.
 **/
int suite_run(suite_t* s, bool fatal_failures);

/**
 * suite_next() runs the next test in the suite.
 * @param s: the test suite to run
 * @param fatal_failures: true indicates that the suite should abort testing
 *			  if any test was marked as a failure
 * @return `EXIT_SUCCESS`, or, if `fatal_failures` is true, `EXIT_FAILURE`
 *	   after the first failed test.
 **/
int suite_next(suite_t* s, bool fatal_failures);

/**
 * suite_stats() returns a stats_t* detailing the results of the testing.
 **/
suite_stats_t* suite_stats(suite_t* s);

/**
 * suite_delstats() frees memory allocated to a stats_t* returned by
 * suite_stats()
 **/
int suite_delstats(suite_stats_t* stats);

#endif
