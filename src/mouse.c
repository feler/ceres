/*
 * mouse.c - mouse functions
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
#include <string.h>
#include <unistd.h>

#include "structs.h"
#include "xutil.h"
#include "mouse.h"
#include "util.h"

/* mouse_set_cursor - get the cursor of a given cursor ID
 */
xcb_cursor_t
mouse_get_cursor(int cursor_id)
{
    /* Get font */
    xcb_font_t font = xcb_generate_id(rootconf.connection);
    xcb_void_cookie_t cookie_font;
    xcb_generic_error_t *error;
    cookie_font = xcb_open_font_checked(rootconf.connection, font,
                                        strlen("cursor"), "cursor");
    if((error = xcb_request_check(rootconf.connection, cookie_font)))
        warning("cannot open cursor font: %d", error->error_code);

    /* Set the cursor image */
    xcb_cursor_t cursor = xcb_generate_id(rootconf.connection);
    xcb_create_glyph_cursor(rootconf.connection, cursor, font, font,
                     cursor_id, cursor_id + 1, 0, 0, 0, 65535, 65535, 65535);

    return cursor;

} /*  }}} */

/* mouse_get_position_in_window - get the position of the mouse  {{{
 * in a given window 
 */
int
mouse_get_position_in_window(xcb_window_t window, int16_t *x, int16_t *y,
                             xcb_window_t *child, uint16_t *mask)
{
    xcb_query_pointer_cookie_t query_ptr_cookie;
    xcb_query_pointer_reply_t *query_ptr_reply;

    query_ptr_cookie = xcb_query_pointer_unchecked(rootconf.connection, window);
    query_ptr_reply = xcb_query_pointer_reply(rootconf.connection, 
                                              query_ptr_cookie,
                                              NULL);

    if(!query_ptr_reply)
        return 0;

    *x = query_ptr_reply->win_x;
    *y = query_ptr_reply->win_y;
    if(mask)
        *mask = query_ptr_reply->mask;
    if(child)
        *child = query_ptr_reply->child;

    return 1;
} /*  }}} */

/* mouse_grab - grab the mouse {{{
 */
int
mouse_grab(void)
{
    xcb_window_t root = XCB_NONE;

    xcb_cursor_t cursor = mouse_get_cursor(68);

    int screen, i;
    int16_t x, y;
    uint16_t mask;
    for(screen = 0;
        screen < xcb_setup_roots_length(xcb_get_setup(rootconf.connection));
        screen++)
    {
        root = screen_get(rootconf.connection, screen)->root;
        if(mouse_get_position_in_window(root, &x, &y, NULL, &mask))
            break;
    }

    xcb_grab_pointer_cookie_t grab_ptr_cookie;
    xcb_grab_pointer_reply_t *grab_ptr_reply;

    for(i = 0; i < 10; i++)
    {
        grab_ptr_cookie = 
            xcb_grab_pointer_unchecked(rootconf.connection, false, root,
                    XCB_EVENT_MASK_BUTTON_PRESS 
                    | XCB_EVENT_MASK_BUTTON_RELEASE
                    | XCB_EVENT_MASK_POINTER_MOTION,
                    XCB_GRAB_MODE_ASYNC,
                    XCB_GRAB_MODE_ASYNC,
                    root, cursor, XCB_CURRENT_TIME);

        grab_ptr_reply = xcb_grab_pointer_reply(rootconf.connection, 
                                                grab_ptr_cookie,
                                                NULL);
        if(grab_ptr_reply)
            return 1;

        usleep(1000);
    }
    
    warning("cannot grab the mouse");
    return 0;
} /*  }}} */

/* mouse_ungrab - stop grabbing the mouse {{{
 */
void
mouse_ungrab(void)
{
    xcb_ungrab_pointer(rootconf.connection, XCB_CURRENT_TIME);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
