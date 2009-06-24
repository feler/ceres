#ifndef CERES_UTIL_H
#define CERES_UTIL_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>

#define die(string, ...) _die(__LINE__, \
                                  __FUNCTION__, \
                                  string, ## __VA_ARGS__)
void _die(int, const char *, const char *, ...)
    __attribute__ ((noreturn)) __attribute__ ((format(printf, 3, 4)));

#define warning(string, ...) _warning(__LINE__, \
                                __FUNCTION__, \
                                string, ## __VA_ARGS__)
void _warning(int, const char *, const char *, ...)
    __attribute__ ((format(printf, 3, 4)));
#endif
