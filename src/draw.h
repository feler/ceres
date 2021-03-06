/*
 * draw.h - draw functions header
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

#ifndef CERES_DRAW_H
#define CERES_DRAW_H

#include <cairo.h>
#include <pango/pangocairo.h>
#include <pango/pango.h>
#include <xcb/xcb.h>

#include "structs.h"

/* draw_text_context_t - store info to display test {{{
 */
typedef struct draw_text_context_t
{
    /* Attribute list, for more info see pango reference */
    PangoAttrList *attr_list;
    char *text;
    ssize_t len;
} draw_text_context_t; /*  }}} */

font_t *font_new(const char *);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
