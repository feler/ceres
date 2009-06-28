/*
 * client.c - client functions
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
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

#include "structs.h"
#include "client.h"
#include "window.h"
#include "layout.h"

/* client_attach - attach a client to the chain {{{
 * @param client The client to attach
 */
void
client_attach(client_t *client)
{
    client->next = rootconf.clients;
    rootconf.clients = client;
} /*  }}} */

/* client_next_tiled - return the next client which can be tiled {{{
 */
client_t *
client_next_tiled(client_t *client)
{
    for(; client && (client->is_floating); client = client->next);
    return client;
} /* }}} */

/* client_configure - configure a client with his new attributtes {{{
 * @param client The client to configure
 */
static void
client_configure(client_t *client)
{
    xcb_configure_notify_event_t event;

    event.window = client->window;
    event.response_type = XCB_CONFIGURE_NOTIFY;
    event.x = client->geometry.x;
    event.y = client->geometry.y;
    event.width = client->geometry.width;
    event.height = client->geometry.height;
    event.override_redirect = false;
    event.above_sibling = XCB_NONE;
    event.border_width = client->border_width;

    xcb_send_event(rootconf.connection, false, client->window, 
            XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char *) &event);
} /*  }}} */

/* client_manage - manage a client giving a window {{{
 * @param window A window
 * @param window_geom The window geometry
 */
void
client_manage(xcb_window_t window, xcb_get_geometry_reply_t *window_geom)
{
    fprintf(stderr, "Manage Window\n");
    client_t *client;
    client = malloc(sizeof(client_t));

    client->window = window;
    client->geometry.x = window_geom->x;
    client->geometry.y = window_geom->y;
    client->geometry.width = window_geom->width;
    client->geometry.height = window_geom->height;
    client->border_width = window_geom->border_width;

    client_attach(client);
    
    xcb_map_window(rootconf.connection, client->window);
    layout_tile();
    xcb_flush(rootconf.connection);
} /*  }}} */

/* client_get_by_window - get a client by his window {{{
 */
client_t *
client_get_by_window(xcb_window_t window)
{
    client_t *client;

    for(client = rootconf.clients; client && client->window != window; client = client->next);
    return client;
} /*  }}} */

/* client_set_focus - give focus to a client {{{
 */
void
client_set_focus(client_t *client)
{
    window_set_focus(client->window);
} /*  }}} */

/* client_resize_and_move - resize and move a client {{{
 */
void
client_resize_and_move(client_t *client, uint32_t x, uint32_t y, uint32_t width,
                       uint32_t height)
{
    client->geometry.x = x;
    client->geometry.y = y;
    client->geometry.width = width;
    client->geometry.height = height;

    const uint32_t values[] = { x, y, width, height };

    xcb_configure_window(rootconf.connection, client->window,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                         XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                         values);
    client_configure(client);
    xcb_flush(rootconf.connection);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
