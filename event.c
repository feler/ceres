/*
 * event.c - events related functions
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
#include <xcb/xcb_event.h>
#include <ev.h>
#include <stdio.h>
#include <unistd.h>

#include "event.h"
#include "structs.h"

/* EVENT HANDLERS [!!] {{{
 */

/* event_map_request - map request handler {{{
 */
static int
event_map_request(void *data __attribute__ ((unused)),
                  xcb_connection_t *connection, xcb_map_request_event_t *ev)
{
    fprintf(stderr, "Map Request\n");

    xcb_get_geometry_cookie_t geom_cookie;
    xcb_get_geometry_reply_t *geom_reply;

    geom_cookie = xcb_get_geometry_unchecked(connection, ev->window);
    geom_reply = xcb_get_geometry_reply(connection, geom_cookie, NULL);

    client_manage(ev->window, geom_reply);

    return 0;
} /*  }}} */

/* event_enter_notify - enter notify event handler {{{
 */
static int
event_enter_notify(void *data __attribute__ ((unused)),
                   xcb_connection_t *connection, xcb_enter_notify_event_t *ev)
{
    fprintf(stderr, "Enter notify\n");
    window_set_focus(ev->event);

    return 0;
} /*  }}} */

/* }}} */

/* ceres_refresh - refresh all the components {{{
 */
int
ceres_refresh(void)
{
    return xcb_flush(rootconf.connection);
} /*  }}} */

/* on_timer - function to execute on the timer {{{
 */
void
on_timer(EV_P_ ev_timer *w, int revents)
{
    ceres_refresh();
} /*  }}} */

/* event_set_handlers - set handlers for X events {{{
 */
void
event_set_handlers(void)
{
    xcb_event_set_map_request_handler(&rootconf.event_h, event_map_request, NULL);
    xcb_event_set_enter_notify_handler(&rootconf.event_h, event_enter_notify, NULL);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
