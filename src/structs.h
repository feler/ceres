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

/*  Includes {{{
 */
/* XCB */
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_icccm.h>

/* Lua */
#include <lua.h>

/* cairo and pango */
#include <cairo.h>
#include <pango/pangocairo.h>

#include <stdbool.h>

#include <ev.h>
/* }}} */

#define lenof(foo) (ssize_t)sizeof(foo) / (ssize_t)sizeof(foo[0])

/* area_t - represent an area {{{
 */
typedef struct area_t 
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
} area_t; /*  }}} */

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

/* bar struct  {{{
 */
typedef struct bar_t
{
    xcb_gcontext_t gc;
    struct draw
    {
        xcb_visualtype_t *visual;
        xcb_pixmap_t pixmap;
        cairo_surface_t *surface;
        PangoLayout *layout;
        cairo_t *cr;
    } draw;
    xcb_window_t window;
    area_t geom;
    bool need_update;
} bar_t; /*  }}} */

/* client_t - client struct (window, geometry, ...) {{{
 */
typedef struct client_t client_t;
struct client_t
{
    /* Name */
    char *name;
    /* window */
    xcb_window_t window;
    /* Geometry x, y, weight, height */
    area_t geometry;
    /* border */
    uint32_t border_width;
    /* Next client */
    client_t *next;
    bool is_floating;
    uint32_t border_color;
    /* the next client in the stack */
    client_t *snext;
}; /*  }}} */

/* color_t - define red, green and blue to be used with cairo {{{
 */
typedef struct color_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t; /*  }}} */

/* ** ceres_t - Main struct a.k.a rootconf  {{{
 */
struct ceres_t
{
    /* Connection */
    xcb_connection_t *connection;
    /* Screen number */
    int screen_default;
    /* Events (or errors :P) handlers */
    xcb_event_handlers_t event_h;
    /* Property handlers */
    xcb_property_handlers_t prop_h;
    char *argv;
    /* Event loop */
    struct ev_loop *loop;
    /* Key symbols */
    xcb_key_symbols_t *key_symbols;
    /* font */
    font_t *font;
    /* timer */
    struct ev_timer timer;
    /* Clients */
    client_t *clients;
    /* screen geometry */
    area_t screen;
    /* workarea */
    area_t workarea;
    /* Appearance */
    struct appearance
    {
        const uint32_t *border_color_focus;
        const uint32_t *border_color_normal;
    } appearance;
    struct config
    {
        /* boder width to apply to new clients */
        uint32_t border_width;
        float mfact;
        const char *border_normal;
        const char *border_focus;
        color_t bg_normal;
        color_t fg_normal;
        color_t bg_focus;
        color_t fg_focus;
    } config;
    /* Actual focused client */
    client_t *client_focused;
    /* client stack */
    client_t *stack;
    /* Lua state */
    lua_State *L;
    /* bar */
    bar_t *bar;
}; /*  }}} */

typedef struct ceres_t ceres_t;

extern ceres_t rootconf;

/* key_bind_t - key bind struct {{{
 */
typedef struct key_bind_t
{
    uint16_t modifier;
    xcb_keysym_t keysym;
    void (*func)(void);
} key_bind_t; /*  }}} */

/* layout_t - layout struct {{{
 */
typedef struct layout_t
{
    /* Function which will organize the desktop */
    void (*arrange)(void);
} layout_t; /*  }}} */

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
