/**
 * @file signals.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date Wed Apr 11
 * @brief This file defines signal handlers for libtdd.
 **/
#include <signal.h>
#include <stdlib.h>

#include "signals.h"

volatile sig_atomic_t __sigsegv_caught = 0;

void __test_sigsegv_handler(int sig) {
    if (sig != SIGSEGV) return;

    __sigsegv_caught++;
}
