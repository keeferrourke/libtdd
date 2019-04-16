/**
 * @file tdd.h
 * @author Keefer Rourke <mail@krourke.org>
 *
 * @mainpage
 * This library provides a simple framework for defining, organizing, and
 * running unit tests in C. It is inspired by the golang `testing` pkg.
 *
 * Some minimal boilerplate is required to write a decent test suite for a
 * software package, but this is straight-forward and can be quickly written
 * in a main() function.
 *
 * In a typical testing binary, the main() function should initialize tests
 * and create any resource files, etc. that might be expected by tests in the
 * suite.
 *
 * @section example Example usage of this library
 *
 * Below is a basic example. See the source code `examples/` directory for
 * more.
 *
 *```
 *  #include <stdlib.h>
 *  #include "tdd.h"
 *
 *  static void* error_func(test_t* t) {
 *      test_error(t, "oops I made a mistake!");
 *      printf("I should keep going though!");
 *      test_error(t, "oops I made another mistake :(");
 *      return NULL;
 *  }
 *
 *  static void* fail_func(test_t* t) {
 *      ...
 *      test_fail(t, "I made a critical mistake!");
 *      // clean up code
 *      return NULL;
 *  }
 *
 *  static void* fatal_func(test_t* t) {
 *      test_fatal(t, "I made a critical mistake!");
 *      printf("this code will not be reached");
 *      return NULL;
 *  }
 *
 *  int main(int argc, char* argv[]) {
 *      // create test suite
 *      suite_t* s = suite_new();
 *
 *      // initalize tests
 *      // variadic func; add as many tests as needed
 *      suite_add(s, 3,
 *                runner_new(&error_func, "error", NULL),
 *                runner_new(&fail_func, "fail", NULL),
 *                runner_new(&fatal_func, "fatal ends early", NULL));
 *
 *      // run the test suite
 *      suite_run(s);
 *      stats = suite_get_stats(s);
 *      suite_del(s);
 *      ...
 *
 *      int retcode = stats.n_error;
 *      suite_stats_del(stats);
 *      return retcode;
 *  }
 *```
 *
 * @section api Test API
 * Test functions should be defined by one of the following signatures:
 *  - `void* test_func(test_t* t);` for regular tests
 *  - `void* bench_func(test_t* t);` for benchmarking tests
 *
 * These functions are added to a test suite using either the `suite_add(...)`
 * or `suite_add_test(runner_t*)` API calls.
 *
 * The name of the test when added to the suite matters.
 *
 * @section benchmarking Benchmarking
 * Test functions may be specially marked as time-sensitive, as in where
 * performance is paramount. Tests may be marked for benchmarking by adding
 * them to the suite with a name prefixed by `bench_`. For all benchmarked
 * functions A timer will be started when the test runs, and stopped when the
 * test finishes. A report of the runtime is printed after the test finishes.
 *
 * For example, the following function is added as a benchmarked function.
 * ```
 * suite_add_test(runner_new(&bench_func, "bench_func", "time sensitive test"));
 * ```
 *
 * The following code is added as a regular test. It will not be timed.
 * ```
 * suite_add_test(runner_new(&test_func, "test_func", "basic test"));
 * ```
 *
 * @section notes Notes
 * This library is multithreaded using POSIX `pthread`s. As such, any binaries
 * built using this library must be compiled with either `gcc` or `clang`'s
 * `-pthread` option.
 **/
#ifndef __TDD_H__
#define __TDD_H__

#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

/**
 * `tdd_sigsegv_caught` is a counter for the number of crashes that were
 * encountered in the test suite.
 */
extern volatile sig_atomic_t tdd_sigsegv_caught;

/**
 * Crash handler.
 **/
void tdd_sigsegv_handler(int sig);

/**
 * Testing structure. This structure is the only parameter in all testing
 * functions. If at any point during a testing function, unexpected bevahiour
 * occurs or the test downright fails, you should call `test_error(t)` and
 * `test_fail(t)` respectively.
 **/
typedef struct test_t {
    /**
     * `name` is a character string that describes the test result.
     **/
    const char* name;
    /**
     * `failed` is a boolean flag specifying if the current test has failed.
     **/
    bool failed;
    /**
     * `err` is a integer flag specifying the number of errors the current
     * test has encountered.
     **/
    int err;
    /**
     * `fail_msg` is a message that is by *test_fail() indicating the reason
     * for test failure. Heap allocated.
     **/
    char* fail_msg;
    /**
     * `err_msg` is an array of character strings that is appended to on each
     * call to test_error(). Each string in this array corresponds to the
     * reason for errors in order of occurance. Each string is Heap allocated.
     **/
    char** err_msg;
    /**
     * `start` is the timestamp at which the test was started. Heap allocated.
     **/
    struct timespec* start;
    /**
     * `end` is the timestamp at which the test was marked as done. Heap
     * allocated.
     **/
    struct timespec* end;
    /**
     * `failed_at` is the timestamp at which the test last encountered a
     * failure. Heap allocated.
     **/
    struct timespec* failed_at;
    /**
     * `error_at` is the timestamp at which the test last encountered an
     * error. Heap allocated.
     **/
    struct timespec* error_at;
    /**
     * `fail()` marks the test as failed with a message explaining the reason
     * for failure.
     **/
    void* (*fail)(struct test_t* t, char* msg);
    /**
     * `error()` marks the test as having encountered an error with a message
     * explaining the reason for the error
     **/
    void* (*error)(struct test_t* t, char* msg);
    /** `begin()` starts a benchmark timer for the test **/
    void* (*begin)(struct test_t* t);
    /** `done()` marks the test as finished **/
    void* (*done)(struct test_t* t);
} test_t;

/**
 * `tdd_test_new()` makes a new test function. Not to be called explicitly.
 * @private
 * @return A pointer to a fully initialized `test_t` structure.
 **/
test_t* tdd_test_new();

/**
 * `tdd_test_del()` frees all memory associated with a `test_t` structure.
 * Not to be called explicitly.
 * @private
 * @param t - pointer to the `test_t` structure to be freed
 * @return int
 */
int tdd_test_del(test_t* t);

/**
 * `test_fail()` marks the test as failed with a message. Failures are
 * identified as critical errors that will not allow testing to continue.
 * Use `test_fail()` to catch fundamental errors in program function
 * execution. To be called within a `runner_t::fn`. Alternatively may be
 * called through the `test_t::fail` interface.
 * @param t   - pointer to a `test_t` structure to capture the context of a
 *	            test failure
 * @param msg - character string indicating the reason for failure
 **/
void* test_fail(test_t* t, char* msg);

/**
 * `test_error()` marks the test as having encountered an error. Errors are
 * identified as non-critical flaws in program function execution which do
 * not prevent continuation of testing. Use `test_error()` to record
 * unexpected but valid return values and similar flaws. To be called within a
 * `runner_t::fn`. Alternatively may be called through the `test_t::error`
 * interface.
 * @param t   - pointer to a `test_t` structure to capture the context of a
 *  	        test error
 * @param msg - character string indicating the reason for error
 **/
void* test_error(test_t* t, char* msg);

/**
 * `test_timer_start()` marks the time at which the test started. This may be
 * useful for benchmarking and should be called after any test setup code.
 * @param t - pointer to a `test_t` structure to capture the test finish time
 **/
void* test_timer_start(test_t* t);

/**
 * `test_timer_end()` marks the time at which the test finished. This may be
 * useful for benchmarking and should be called before any test teardown code.
 * @param t - pointer to a `test_t` structure to capture the test finish time
 **/
void* test_timer_end(test_t* t);

/**
 * `test_fatal()` is a convenience macro that will fail and end a test.
 * @param t   - pointer to a `test_t` structure to capture the context of a
 *	            test failure
 * @param msg - character string indicating the reason for failure
 */
#define test_fatal(t, msg) return test_fail(t, msg);

/**
 * Testing function.
 **/
typedef struct runner_t {
    /**
     * `name` is a character string identifier for a test. It is usually just
     * the name of the function itself, such as "test_func".
     */
    char* name;
    /**
     * `desc` is a character string description for a test. It should be a
     * humanly readable explanation of what the test is performing. Optional
     * field in constructor.
     */
    char* desc;
    /**
     * `fn` is a pointer to a test function.
     * @param t - pointer to a `test_t` structure to capture context of test
     *		      results
     */
    void* (*fn)(void* t);
} runner_t;

/**
 * `runner_new()` creates and returns a pointer to an initialized runner_t.
 * @param f    - a pointer to a function that records information about a test
 *		         within its single `test_t*` argument
 * @param name - a character string identifier for the test; usually the name
 *		         of the test function itself
 * @param desc - a human readable description of the test; can be `NULL`
 * @return A pointer to a fully initialized runner_t structure.
 **/
runner_t* runner_new(void* (*f)(void* t), char* name, char* desc);

/**
 * `runner_del()` frees memory allocated to a test function. Should not be
 * called manually.
 * @private
 * @param tr - a pointer to a `runner_t` that is to be destroyed
 * @return`EXIT_SUCCESS`, otherwise `EXIT_FAILURE` if tf is `NULL`.
 **/
int tdd_runner_del(runner_t* tr);

/**
 * Testing suite. Contains all tests, current runtime state, and the results
 * of each test. May be used to contruct a suite_stats_t after running.
 **/
typedef struct suite_t {
    /**
     * `finished` is a boolean flag specifying if all tests have run. If this
     * flag is not set by the time the suite has completed all tests, then it
     * aborted testing with a fatal failure from one of the tests in the
     * suite.
     **/
    bool finished;
    /** `n_tests` is the number of tests in the suite. **/
    int n_tests;
    /** `n_segv` is the number of segmentation faults that were caught. **/
    int n_segv;
    /** `test_index` is the index of the current test. **/
    int test_index;
    /** `tests` is an array of runner_t* that make up the suite. **/
    runner_t** tests;
    /**
     * `results` is an array of `test_t*` that details testing results for
     * each element in tests.
     **/
    test_t** results;
    /**
     * `outfile` is a FILE pointer which is where the results of the test will
     * be printed. This is stdout by default, but may be changed manually
     * after the suite is initialized and before it is run.
     **/
    FILE* outfile;
    /**
     * `quiet` is a boolean flag indicating that results should not be printed
     * as the test suite runs; reporting can instead be done through creating
     * a stats structure after the suite finishes.
     **/
    bool quiet;
} suite_t;

/**
 * `suite_new()` creates and returns a new test suite.
 * @return A pointer to a fully intialized suite_t structure.
 **/
suite_t* suite_new();

/**
 * `suite_reset()` resets a `suite_t` to its initial state, as if it were
 * never run.
 * @param s - a pointer to a `suite_t` test suite to be reset
 **/
void suite_reset(suite_t* s);

/**
 * `suite_del()` frees memory allocated to a test suite.
 * @param s - a pointer to a `suite_t` test suite that is to be destroyed
 * @return
 **/
int suite_del(suite_t* s);

/**
 * `suite_done()` marks all the suite as having finished all tests.
 **/
void suite_done(suite_t* s);

/**
 * `suite_add()` adds n runner_t to the suite.
 **/
void suite_add(suite_t* s, int n, ...);

/**
 * `suite_add_test()` adds a single `runner_t` to the suite.
 **/
int suite_add_test(suite_t* s, runner_t* f);

/**
 * `suite_run()` runs all tests in the test array.
 * @param s              - the test suite to run
 * @param fatal_failures - true indicates that the suite should abort
 *                         testing if any test was marked as a failure
 * @return `EXIT_SUCCESS`, or, if `fatal_failures` is true, `EXIT_FAILURE`
 *	   after the first failed test.
 **/
int suite_run(suite_t* s, bool fatal_failures);

/**
 * `suite_next()` runs the next test in the suite.
 * @param s              - the test suite to run
 * @param fatal_failures - true indicates that the suite should abort
 *                         testing if any test was marked as a failure
 * @return `EXIT_SUCCESS`, or, if `fatal_failures` is true, `EXIT_FAILURE`
 *	   after the first failed test.
 **/
int suite_next(suite_t* s, bool fatal_failures);

/**
 * Test result. This structure holds the name of a test which ran, and
 * indicates if the test passed.
 **/
typedef struct tdd_result_t {
    /**
     * `name` is the name of the test that produced this result.
     **/
    char* name;
    /**
     * `ok` indicates if the test that produced this result was successful.
     **/
    bool ok;
} tdd_result_t;

tdd_result_t* tdd_result_new(char* name, bool ok);
int tdd_result_del(tdd_result_t* result);

/**
 * Stats structure detailing results of test suite.
 **/
typedef struct suite_stats_t {
    /**
     * `test_run` is an array of tdd_result_t containing the results of the
     * tests that ran in the suite.
     **/
    tdd_result_t** tests_run;
    /**
     * `n_tests` is the total number of tests in the suite.
     **/
    int n_tests;
    /**
     * `n_error` is the total number of errors in the suite.
     **/
    int n_error;
    /**
     * `n_fail` is the total number of failures in the suite.
     **/
    int n_fail;
    /**
     * `n_ran` is the total number of tests that ran in the suite. If this
     * count differs from `n_tests`, then some tests were skipped.
     **/
    int n_ran;
    /**
     * `success_rate` is the percent rate of successful tests in the suite.
     **/
    double success_rate;
    /**
     * `fatal_failures` is an indication that the suite ran with fatal
     * failures enabled.
     */
    bool fatal_failures;
} suite_stats_t;

/**
 * `suite_get_stats()` returns a `stats_t*` detailing the results of the
 * testing.
 **/
suite_stats_t* suite_get_stats(suite_t* s);

/**
 * `suite_stats_del()` frees memory allocated to a `stats_t*` returned by
 * `suite_get_stats()`
 **/
int suite_stats_del(suite_stats_t* stats);

#endif
