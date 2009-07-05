/*
 * util.h - util macros and functions header
 *
 * Copyright © 2009 Ángel Alonso (feler) <feler@archlinux.us> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef CERES_UTIL_H
#define CERES_UTIL_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>

#define RGB_8TO16(i)   (0xffff * ((i) & 0xff) / 0xff)
#define RGB_16TO8(i)   (0xff * ((i) & 0xffff) / 0xffff)

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

#define debug(string, ...) _debug(__LINE__, \
                                __FUNCTION__, \
                                string, ## __VA_ARGS__)
void _debug(int, const char *, const char *, ...)
    __attribute__ ((format(printf, 3, 4)));

#define LENGTH(x)   (sizeof x / sizeof x[0])
#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
