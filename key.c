/*
 * key.c - key function
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
#include <unistd.h>

#include "structs.h"
#include "xutil.h"

/* keyboard_grab - grab the keyboard {{{
 */
bool
keyboard_grab(void)
{
    xcb_grab_keyboard_cookie_t cookie;
    xcb_grab_keyboard_reply_t *reply;

    int i;
    /* Try 10 time to grab the keyboard */
    for(i = 0; i < 10; i++)
    {
        cookie = xcb_grab_keyboard(rootconf.connection, true,
                                   get_default_screen()->root,
                                   XCB_CURRENT_TIME,
                                   XCB_GRAB_MODE_ASYNC,
                                   XCB_GRAB_MODE_ASYNC);
        reply = xcb_grab_keyboard_reply(rootconf.connection, cookie,
                                        NULL);
        if(reply)
            return true;

        usleep(1000);
    }

    return false;
} /*  }}} */

/* keyboard_ungrab - ungrab the keyboard {{{
 */
void
keyboard_ungrab(void)
{
    xcb_ungrab_keyboard(rootconf.connection, XCB_CURRENT_TIME);
} /*  }}} */

/* key_grab_to_window - grab a key to a window {{{
 */
void
key_grab_to_window(xcb_window_t window, cer_key_t *key)
{
    /* We have a keycode */
    if(key->keycode)
        xcb_grab_key(rootconf.connection, true, window,
                     key->modifier, key->keycode, XCB_GRAB_MODE_ASYNC,
                     XCB_GRAB_MODE_ASYNC);
    /* We have a keysym */
    else if(key->keysym)
    {
        /* Get the keycode, LOOK: rootconf.keysymbols must be initialized */
        xcb_keycode_t *keycodes = xcb_key_symbols_get_keycode(rootconf.key_symbols, key->keysym);
        xcb_keycode_t *keycode_counter = keycodes;
        if(keycodes)
            for (keycode_counter = keycodes; *keycode_counter; keycode_counter++)
                xcb_grab_key(rootconf.connection, true, window,
                             key->modifier, *keycode_counter, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    }
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
