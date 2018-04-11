/**
 * @file ctest_signals.c
 * @author Keefer Rourke <mail@krourke.org>
 * @date Wed Apr 11
 * @brief This file defines signal handlers for libctest.
 **/
#ifndef __CTEST_SIGNAL_HANDLERS_H__
#define __CTEST_SIGNAL_HANDLERS_H__

#include <signal.h>
#include <stdlib.h>

extern volatile sig_atomic_t __sigsegv_caught;

void __test_sigsegv_handler(int sig);

#endif
