/**
 * @private
 * @file timeutil.h
 * @author Keefer Rourke <mail@krourke.org>
 * @brief Private time format and manipulation functions and macros.
 */
#ifndef __TDD_TIME_UTIL_H__
#define __TDD_TIME_UTIL_H__

#include <stdlib.h>
#include <time.h>

/**
 * @private
 * @internal
 * __timespace_minus() subtracts two timespec structs to yeild a new
 * timespec.
 */
struct timespec __timespec_minus(struct timespec* a, struct timespec* b);

#endif
