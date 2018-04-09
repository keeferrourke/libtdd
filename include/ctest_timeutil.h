/**
 * @file ctest_timeutil.h
 * @author Keefer Rourke <mail@krourke.org>
 * @date 08 Apr 2018
 * @brief Time format and manipulation functions and macros for libctest.
 * @private
 */
#ifndef __CTEST_TIME_UTIL_H__
#define __CTEST_TIME_UTIL_H__

#include <stdlib.h>
#include <time.h>

struct timespec __timespec_minus(struct timespec* a, struct timespec* b);

#endif
