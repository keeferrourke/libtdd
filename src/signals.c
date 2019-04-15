/**
 * @file signals.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date Wed Apr 11
 * @brief This file defines signal handlers for libtdd.
 **/
#include <signal.h>
#include <stdlib.h>

#include "tdd.h"

volatile sig_atomic_t tdd_sigsegv_caught = 0;

void tdd_sigsegv_handler(int sig) {
    if (sig != SIGSEGV) return;

    tdd_sigsegv_caught++;
}
