/*
 * structs.h - ALL the structs (typedef, struct, ...)
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

#ifndef CERES_STRUCTS_H
#define CERES_STRUCTS_H

/* XCB */
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_icccm.h>

/* cairo and pango */
#include <cairo.h>
#include <pango/pangocairo.h>

#include <stdbool.h>

#include <ev.h>

#include "window.h"
#include "client.h"

#define lenof(foo) (ssize_t)sizeof(foo) / (ssize_t)sizeof(foo[0])

typedef struct area_t area_t;

/* font_t - font struct {{{
 */
typedef struct
{
    PangoFontDescription *description;
    int height;
} font_t; /*  }}} */

/* root_window_t - Root window structs, useful for scan() {{{
 */
typedef struct root_window_t
{
    xcb_window_t window;
    /* The query tree cookie for be used with xcb_query_tree_reply(...)  */
    xcb_query_tree_cookie_t tree_cookie;
} root_window_t; /*  }}} */

/* client_t - client struct (window, geometry, ...) {{{
 */
typedef struct client_t
{
    /* Name */
    char name[256];
    /* window */
    xcb_window_t window;
    /* Geometry x, y, weight, height */
    area_t geometry;
    /* border */
    uint32_t border_width;
} client_t; /*  }}} */

/* ceres_t - Main struct a.k.a rootconf  {{{
 */
struct ceres_t
{
    /* Connection */
    xcb_connection_t *connection;
    /* Screen number */
    int screen_default;
    /* Events (or errors :P) handlers */
    xcb_event_handlers_t event_h;
    char *argv;
    /* Event loop */
    struct ev_loop *loop;
    /* Key symbols */
    struct xcb_key_symbols_t *key_symbols;
    /* font */
    font_t *font;
    /* timer */
    struct ev_timer timer;
    /* Clients */
    struct
    {
        /* The focused client */
        client_t *focus;
    } clients;
    xcb_event_handlers_t event_handlers;
}; /*  }}} */

typedef struct ceres_t ceres_t;

extern ceres_t rootconf;

/*  Color related structs {{{
 */
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
    bool initialized;
} color_t;

typedef struct
{
    color_t *color;
    const char *colstr;
    xcb_alloc_named_color_cookie_t cookie;
    bool has_error;
} color_init_cookie_t;

typedef struct
{
    uint32_t pixel;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t alpha;
    bool initialized;
} xcolor_t;

typedef struct
{
    union
    {
        xcb_alloc_color_cookie_t cookie_hexa;
        xcb_alloc_named_color_cookie_t cookie_named;
    };

    xcolor_t *color;
    bool is_hexa;
    bool has_error;
    const char *colstr;
} xcolor_init_request_t;
/* }}} */
#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80