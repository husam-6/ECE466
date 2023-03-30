#include <execinfo.h> // for backtrace
#include <stdio.h>
#include <stdlib.h>

/*
 * Die with backtrace, for internal errors
 *
 * backtrace() depends on glibc
 * some of this might fail depending on how much damage we did,
 * but we'll die in this function one way or another
 */
#define BACKTRACE_DEPTH 128

_Noreturn __attribute__((noreturn)) void die(const char *msg) {
    fprintf(stderr, "\nInternal error: %s\n", msg);

    // I print a graphic here to make it look nice

    fprintf(stderr, "Trying to print backtrace:\n------------------------------\n");

// on macOS, you must link against a sanitizer (e.g. -fsanitize=undefined)
// because macOS doesn't provide backtrace(). This also means you'll get line
// numbers. If you want to get line numbers on Linux too, just remove the #ifdef
// block so that all platforms use the __sanitizer_print_stack_trace() interface
// and get rid of backtrace().
// the compile flags you need are -fsanitize=undefined (or =address or =memory) and -ggdb.
#ifdef __APPLE__
        void __sanitizer_print_stack_trace(void);
        __sanitizer_print_stack_trace();
#else
        void *callstack[BACKTRACE_DEPTH];
        int frames = backtrace(callstack, BACKTRACE_DEPTH);
        char **strings = backtrace_symbols(callstack, frames);
        for (int i = 0; i < frames; i++) {
            fprintf(stderr, "    %s\n", strings[i]);
        }
#endif

    abort();
}

// int main() {
//     die("oops");
// }