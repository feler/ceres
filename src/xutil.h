/*
 * xutil.h - some useful X functions header
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

#ifndef CERES_XUTIL_H
#define CERES_XUTIL_H

#include <assert.h>

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_event.h>

#include "util.h"

void set_error_handler(xcb_event_handlers_t *, 
                        xcb_generic_error_handler_t, void *);
void check_other_wm(xcb_connection_t *);
void check_invalid_connection(xcb_connection_t *);
xcb_visualtype_t *visual_get_from_screen(xcb_screen_t *);

/* screen_get - get a screen {{{
 * \param connection The X connection
 * \param screen The screen number
 */
static inline xcb_screen_t *
screen_get(xcb_connection_t *connection, int screen)
{
    xcb_screen_t *s;

    check_invalid_connection(connection);

    s = xcb_aux_get_screen(connection, screen);

    assert(s);

    return s;
} /* }}} */

xcb_screen_t *get_default_screen(void);
xcb_visualtype_t *screen_default_visual(xcb_screen_t *);
bool get_string_from_atom(xcb_window_t, xcb_atom_t, char **, ssize_t *);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
