/*
 * ewmh.c - EWMH standar header
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
#include <unistd.h>

#include "structs.h"
#include "atoms/atoms.h"
#include "xutil.h"
#include "ewmh.h"

/* ewmh_update_dektop_geometry - updates the desktop geometry  {{{
 * @param screen_in The screen to update
 */
void
ewmh_update_desktop_geometry(int screen_in)
{
    
} /*  }}} */

/* init_ewmh - init ewmh {{{
 * @param screen_in The screen to initialize
 */
void
init_ewmh(int screen_in)
{
    xcb_screen_t *screen = screen_get(rootconf.connection, screen_in);
    xcb_window_t father;

    xcb_atom_t atom[] =
    {
        _NET_SUPPORTED,
        _NET_SUPPORTING_WM_CHECK,
        _NET_STARTUP_ID,
        _NET_CLIENT_LIST,
        _NET_CLIENT_LIST_STACKING,
        _NET_NUMBER_OF_DESKTOPS,
        _NET_CURRENT_DESKTOP,
        _NET_DESKTOP_NAMES,
        _NET_ACTIVE_WINDOW,
        _NET_WORKAREA,
        _NET_DESKTOP_GEOMETRY,
        _NET_CLOSE_WINDOW,
        _NET_WM_NAME,
        _NET_WM_STRUT_PARTIAL,
        _NET_WM_ICON_NAME,
        _NET_WM_VISIBLE_ICON_NAME,
        _NET_WM_DESKTOP,
        _NET_WM_WINDOW_TYPE,
        _NET_WM_WINDOW_TYPE_DESKTOP,
        _NET_WM_WINDOW_TYPE_DOCK,
        _NET_WM_WINDOW_TYPE_TOOLBAR,
        _NET_WM_WINDOW_TYPE_MENU,
        _NET_WM_WINDOW_TYPE_UTILITY,
        _NET_WM_WINDOW_TYPE_SPLASH,
        _NET_WM_WINDOW_TYPE_DIALOG,
        _NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        _NET_WM_WINDOW_TYPE_POPUP_MENU,
        _NET_WM_WINDOW_TYPE_TOOLTIP,
        _NET_WM_WINDOW_TYPE_NOTIFICATION,
        _NET_WM_WINDOW_TYPE_COMBO,
        _NET_WM_WINDOW_TYPE_DND,
        _NET_WM_WINDOW_TYPE_NORMAL,
        _NET_WM_ICON,
        _NET_WM_PID,
        _NET_WM_STATE,
        _NET_WM_STATE_STICKY,
        _NET_WM_STATE_SKIP_TASKBAR,
        _NET_WM_STATE_FULLSCREEN,
        _NET_WM_STATE_MAXIMIZED_HORZ,
        _NET_WM_STATE_MAXIMIZED_VERT,
        _NET_WM_STATE_ABOVE,
        _NET_WM_STATE_BELOW,
        _NET_WM_STATE_MODAL,
        _NET_WM_STATE_HIDDEN,
        _NET_WM_STATE_DEMANDS_ATTENTION
    };
    
    /* EWMH support per view */
    xcb_change_property(rootconf.connection, XCB_PROP_MODE_REPLACE,
                        screen->root, _NET_SUPPORTED, ATOM, 32,
                        lenof(atom), atom);

    /* Create our own window! */
    father = xcb_generate_id(rootconf.connection);
    xcb_create_window(rootconf.connection, screen->root_depth,
            father, screen->root, -1, -1, 1, 1, 0,
            XCB_COPY_FROM_PARENT, screen->root_visual, 0, NULL);

    xcb_change_property(rootconf.connection, XCB_PROP_MODE_REPLACE,
            screen->root, _NET_SUPPORTING_WM_CHECK, WINDOW, 32,
            1, &father);

    /* Set WM name */
    xcb_change_property(rootconf.connection, XCB_PROP_MODE_REPLACE,
            father, _NET_WM_NAME, UTF8_STRING, 8, 7, "ceres");

    /* set WM pid */
    int pid = getpid();
    xcb_change_property(rootconf.connection, XCB_PROP_MODE_REPLACE,
            father, _NET_WM_PID, CARDINAL, 32, 1, &pid);

} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
