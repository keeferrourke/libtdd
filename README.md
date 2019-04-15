libtdd
======

`libtdd` is a minimalist testing framework inspired by the Golang
testing pkg. It is designed to provide a framework which may be used to
build scaffolding during Test Driven Development (TDD).

This small C library attempts to provide a consistent API for creating,
running, and checking the results of tests. There is very little
boilerplate required in the main function.


Features
--------

 * Easy API for test suite creation and execution using TDD semantics
 * Simple benchmarking
 * Pretty output with optional colour support
 * Summary statistics
 * Catch and count crashes (SIGSEGV handler)


Build and usage
---------------

The Makefile included with this project creates static and dynamic
libraries in the `lib/` directory. To build this, simply run:

    make lib    # libtdd.a
    make dylib  # libtdd.so (or libtdd.dylib on macOS/Darwin)

Colour output support can be enabled at compile time by including the
`DEFINE=-DUSE_COLOUR` directive on the `make` line. ex.

    make dylib DEFINE=-DUSE_COLOUR  # enables pretty colour printing

You can then include the outputted `libtdd.so` or `libtdd.a` in your
project as is required.

API documentation can be compiled with Doxygen and LaTeX via:

    make docs

See the Makefile for other relevant build targets.

**NOTE** macOS/Darwin support is untested at this time.
This library was built and tested on Fedora 27 Workstation with glibc
v2.26.

Example program
---------------

A simple example program that demonstrates all features provided by this
library can be found in the `examples/` directory of this project.

It can be compiled and run as follows:

    make lib
    cc -o tdd_example -std=c99 -Iinclude examples/main.c -pthread \
       -Llib -ltdd
    ./tdd_example


Conventions
-----------

### Testing

Every testing function is defined as a `void* test_myfunc(void* t)`
where the parameter `t` is a `test_t` that records information about
the test, including failure and error states, and start and end time
of each test. The parameter and return types are `void*` because tests
are run in `pthreads`. Despite this, there is no need to cast `t` when
calling the provided API functions.

The start of a test is signified by `test_start(t)` and the end
of a test is signified by `test_done(t)`, but these calls are completely
optional if only testing for correctness and not speed.

You can end the execution of a test early and mark it as a failure
through a call to `test_fail(t, "message")`. Non-critical errors that
should not end a test can be recorded by `test_error(t, "message")`.

### Benchmarking

If benchmarking, then for convenience, the `test_start(t)` and
`test_done(t)` function calls may be omitted within a test body if the
test name is prefixed by `bench_`. These times will be automatically
recorded by the suite.

Calls to `test_start(t)` and `test_done(t)` may still be made to
override the start and end times if the benchmarks require setup and
teardown code that should not be included in the recorded time.

i.e. a benchmarking function may be simply defined as

    void* bench_func(void* t) {
        ...
        return;
    }

and can be added to a test suite as a benchmarking function by

    suite_addtest(s, newtest("bench_one", &bench_func));

After execution of each benchmark function, a short summary of runtime
is printed.


Bugs and future development
---------------------------

No bugs to speak of at this moment :)

Future development:
 * It might be nice to optionally run tests in parallel; currently all
   tests run sequentially in the order they are added to the suite.


License information
-------------------

Copyright (c) 2018 Keefer Rourke <mail@krourke.org>

This software is released under the ISC License. See LICENSE for more
details.

Alternatives
------------

This project aims to realize a modern, easy to use solution for C
testing frameworks. If you don't want to use this, there are some
alternatives that I like:

 - [`rcunit`](https://github.com/jecklgamis/rcunit)
    + Seems very similar to the goals of my project;
      I wish I'd known about it before :wink:
 - [`minunit`](http://www.jera.com/techinfo/jtns/jtn002.html)
    + For the ultra minimal
 - [`check`](https://libcheck.github.io/check/)
    + Includes an extensive assertion library