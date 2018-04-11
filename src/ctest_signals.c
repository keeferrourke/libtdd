/**
 * @file ctest_signals.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date Wed Apr 11
 * @brief This file defines signal handlers for libctest.
 **/
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t __sigsegv_caught = 0;

void __test_sigsegv_handler(int sig) {
    if (sig != SIGSEGV) return;

    __sigsegv_caught++;
}
