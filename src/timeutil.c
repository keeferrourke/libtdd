/**
 * @file timeutil.h
 * @author Keefer Rourke <mail@krourke.org>
 * @brief Time format and manipulation functions and macros.
 * @private
 */
#include <stdlib.h>
#include <time.h>

#include "timeutil.h"

#define NSEC_S 1000000000L

struct timespec __timespec_minus(struct timespec* a, struct timespec* b) {
    struct timespec ret;
    ret.tv_sec  = a->tv_sec - b->tv_sec;
    ret.tv_nsec = a->tv_nsec - b->tv_nsec;
    if (ret.tv_nsec < 0) {
        --(ret.tv_sec);
        ret.tv_nsec += NSEC_S;
    }
    return ret;
}
