/**
 * @private
 * @file timeutil.h
 * @author Keefer Rourke <mail@krourke.org>
 * @brief Time format and manipulation functions and macros for libtdd.
 */
#ifndef __TDD_TIME_UTIL_H__
#define __TDD_TIME_UTIL_H__

#include <stdlib.h>
#include <time.h>

struct timespec __timespec_minus(struct timespec* a, struct timespec* b);

#endif
