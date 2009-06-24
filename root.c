/*
 * root.c - root windows management functions
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

#include <xcb/xcb.h>

#include "structs.h"
#include "root.h"
#include "mouse.h"
#include "xutil.h"

/* root_window_set_cursor - set the root window cursor {{{
 */
void
root_window_set_cursor(int cursor_id)
{
    int screen;
    const uint32_t vals[] = { mouse_get_cursor(68) };
    
    for(screen = 0;
        screen < xcb_setup_roots_length(xcb_get_setup(rootconf.connection));
        screen++)
    {
        xcb_change_window_attributes(rootconf.connection,
                                     screen_get(rootconf.connection, screen)->root,
                                     XCB_CW_CURSOR,
                                     vals); 
    }
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
