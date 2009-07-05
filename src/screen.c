/*
 * screen.c - screen management functions
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
#include "xutil.h"
#include "ewmh.h"
#include "screen.h"

/* init_screens - init the screens {{{
 */
void
init_screens(void)
{
    int screen_counter;
    int max_screens = 
        xcb_setup_roots_length(xcb_get_setup(rootconf.connection));

    const uint32_t change_window_vals[] =
    {
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
            | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW
            | XCB_EVENT_MASK_STRUCTURE_NOTIFY
            | XCB_EVENT_MASK_PROPERTY_CHANGE
            | XCB_EVENT_MASK_BUTTON_PRESS
            | XCB_EVENT_MASK_BUTTON_RELEASE
            | XCB_EVENT_MASK_FOCUS_CHANGE
    };
    
    for(screen_counter = 0;
        screen_counter < max_screens;
        screen_counter++)
    {
        xcb_change_window_attributes(rootconf.connection,
                screen_get(rootconf.connection, screen_counter)->root,
                XCB_CW_EVENT_MASK,
                change_window_vals);

        init_ewmh(screen_counter);
    }

    screen_update_geom();
} /*  }}} */

/* update_workarea - update the area which the windows can use
 */
void
update_workarea(void)
{
    rootconf.workarea.x = rootconf.screen.x;
    rootconf.workarea.y = rootconf.screen.y;
    rootconf.workarea.width = rootconf.screen.width;
    rootconf.workarea.height = rootconf.screen.height;
}

/* screen_update_geom - update the geometry of actual screen {{{
 */
void
screen_update_geom(void)
{
    xcb_screen_t *screen = get_default_screen();

    rootconf.screen.y = 0;
    rootconf.screen.x = 0;
    rootconf.screen.width = screen->width_in_pixels;
    rootconf.screen.height = screen->height_in_pixels;

    update_workarea();
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
