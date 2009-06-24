/*
 * color.h - color functions header
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

#ifndef CERES_COLOR_H
#define CERES_COLOR_H

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <malloc.h>
#include <stdlib.h>

#include "structs.h"
#include "xutil.h"
#include "draw.h"

int init_RGB_color(uint16_t red, uint16_t green, uint16_t blue);
int init_named_color(ssize_t, const char *);
int color_init(const char *, ssize_t);


#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
