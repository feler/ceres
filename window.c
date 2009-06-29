/*
 * window.c - window handling functions
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
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>

#include "structs.h"
#include "atoms/atoms.h"

#include "window.h"

/* window_update_geometry  {{{
 * @window the window be changed
 * @new_geom the new geometry
 * @border border in pixels
 */
void
window_configure(xcb_window_t window, uint16_t x, uint16_t y,
        uint16_t width, uint16_t height, int border)
{
    xcb_configure_notify_event_t conf;

    /* Fill the struct */
    conf.event = window;
    conf.window = window;
    conf.response_type = XCB_CONFIGURE_NOTIFY;
    conf.above_sibling = XCB_NONE;
    conf.override_redirect = false;
    conf.x = x;
    conf.y = y;
    conf.width = width;
    conf.height = width;

    /* Update */
    xcb_send_event(rootconf.connection, false,
            window, XCB_EVENT_MASK_STRUCTURE_NOTIFY,
            (const char *) &conf);
} /*  }}} */

/* window_check_protocol - check if a given window has a given protocol {{{
 * @window a window
 * @proto a protocol
 */
bool
window_check_protocol(xcb_window_t window, xcb_atom_t protocol)
{
    uint16_t i;
    xcb_get_wm_protocols_reply_t protocols;
    
    if(xcb_get_wm_protocols_reply(rootconf.connection,
                xcb_get_wm_protocols_unchecked(rootconf.connection,
                    window, WM_PROTOCOLS),
                &protocols, NULL))
    {
        for(i = 0; i < protocols.atoms_len; i++)
            if(protocols.atoms[i] == protocol)
                return true;
        xcb_get_wm_protocols_reply_wipe(&protocols);
    }
    return false;
} /*  }}} */

/*  window_take_focus - say a window that have the focus {{{
 */
void
window_take_focus(xcb_window_t window)
{
    xcb_client_message_event_t event;

    event.response_type = XCB_CLIENT_MESSAGE;
    event.window = window;
    event.format = 32;
    event.data.data32[1] = XCB_CURRENT_TIME;
    event.type = WM_PROTOCOLS;
    event.data.data32[0] = WM_TAKE_FOCUS;

    xcb_send_event(rootconf.connection, false, window, XCB_EVENT_MASK_NO_EVENT,
                   (char *) &event);
} /*  }}} */

/* window_set_focus - set focus to a given window {{{
 * @window a window
 */
void
window_set_focus(xcb_window_t window)
{
    bool can_take_focus = window_check_protocol(window, WM_TAKE_FOCUS); 
    xcb_set_input_focus(rootconf.connection, XCB_INPUT_FOCUS_PARENT,
                        window, XCB_CURRENT_TIME);

    if(can_take_focus)
       window_take_focus(window);

} /*  }}} */

/* window_set_state - changes the state of a window {{{
 * @window a window
 * @state the state we want to change it to
 */
void
window_set_state(xcb_window_t window, long state)
{
    long data[] = { state, XCB_NONE };
    xcb_change_property(rootconf.connection, XCB_PROP_MODE_REPLACE, window,
            XCB_WM_HINT_STATE, XCB_WM_HINT_STATE, 32, 2, data);
} /*  }}} */

/*  ------window_get_state-----------------------------------------------{{{
 *  The following two functions (window_get_state_prepare and
 *  window_get_state) get the state of a window.
 *
 *  xcb_get_state_prepare gets the cookies to after get the states with
 *  with xcb_get_state.
 *
 *  The perfect use is like:
 *  for(...) { window_get_state_prepare(...) }
 *  for(...) { window_get_state(...) }
 *  First we get the cookies and after the states.
 *-----------------------------------------------------------------------
 */

/* window_get_state_prepare {{{
 */
xcb_get_property_cookie_t
window_get_state_prepare(xcb_window_t window)
{
    /* xcb_get_property(xcb_connection_t *       c,
     *                  uint8_t         _delete,
     *                  xcb_window_t    window,
     *                  xcb_atom_t      property,
     *                  xcb_atom_t      type,
     *                  uint32_t        long_offset,
     *                  uint32_t        long_length)
     */
    return xcb_get_property_unchecked(rootconf.connection, false, window,
            WM_STATE, WM_STATE, 0L, 2L);
} /*  }}} */

/* window_get_state {{{
 */
uint32_t
window_get_state(xcb_get_property_cookie_t cookie)
{
    xcb_get_property_reply_t *reply = 
        xcb_get_property_reply(rootconf.connection, cookie, NULL);

    if(reply)
        if(xcb_get_property_value_length(reply))
            return *(uint32_t *) xcb_get_property_value(reply);
    return 0;
} /*  }}} */

/* }}} */

/* window_set_border_color - set the border color of a window  {{{
 */
void
window_set_border_color(xcb_window_t window, uint32_t pixel)
{
    xcb_change_window_attributes(rootconf.connection, window,
                                 XCB_CW_BORDER_PIXEL, pixel);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
