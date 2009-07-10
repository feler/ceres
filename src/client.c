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
#include "bar.h"
#include "window.h"
#include "layout.h"
#include "xutil.h"
#include "atoms/atoms.h"

#define max(A, B)               ((A) > (B) ? (A) : (B))
#define min(A, B)               ((A) < (B) ? (A) : (B))

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
    client_t *client;
    client = malloc(sizeof(client_t));

    const uint32_t vals[] = { (XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                               XCB_EVENT_MASK_PROPERTY_CHANGE  |
                               XCB_EVENT_MASK_ENTER_WINDOW     |
                               XCB_EVENT_MASK_LEAVE_WINDOW     |
                               XCB_EVENT_MASK_FOCUS_CHANGE) };

    client->window = window;
    client->geometry.x = window_geom->x;
    client->geometry.y = window_geom->y;
    client->geometry.width = window_geom->width;
    client->geometry.height = window_geom->height;
    client_set_border_width(client, rootconf.config.border_width);

    client_configure(client);

    /* Event configuration */
    xcb_change_window_attributes(rootconf.connection, client->window,
                                 XCB_CW_EVENT_MASK,
                                 vals);

    client_update_name(client);

    client_attach(client);
    client_attach_stack(client);

    client_set_focus(client);

    xcb_map_window(rootconf.connection, client->window);

    layout_update();
} /*  }}} */

/* client_get_by_window - get a client by his window {{{
 */
client_t *
client_get_by_window(xcb_window_t window)
{
    client_t *client;

    for(client = rootconf.clients; client && client->window != window;
        client = client->next);
    return client;
} /*  }}} */

/* client_set_focus - give focus to a client {{{
 *  if client is NULL the focus will be set to the next client
 *  or if there is no more client to root window.
 * @param client The client to give focus or NULL
 */
void
client_set_focus(client_t *client)
{
    if(!client)
        /* no client specified, get next client */
        for(client = rootconf.stack; client; client = client->snext);
    if(client)
    {
        client_detach_stack(client);
        client_attach_stack(client);
        window_set_focus(client->window);
    }
    else
        /* we don't have more clients, focus root */
        window_set_focus(get_default_screen()->root);
    rootconf.client_focused = client;
    client_update_border_color(client);

    bar_update_task_list(rootconf.bar);
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

/* client_detach - detach a client  {{{
 */
void
client_detach(client_t *client)
{
    client_t **c;

    for(c = &rootconf.clients; *c && *c != client; c = &(*c)->next);
    *c = client->next;
} /*  }}} */

/* client_unmanage - unmanage a client {{{
 */
void
client_unmanage(client_t *client)
{
    /* remove client from the chain */
    client_detach(client);
    client_detach_stack(client);

    if(!client || !client->window)
        return;
    xcb_grab_server(rootconf.connection);

    xcb_ungrab_button(rootconf.connection, XCB_BUTTON_INDEX_ANY, client->window,
                      XCB_BUTTON_MASK_ANY);
    window_set_state(client->window, XCB_WM_STATE_WITHDRAWN);
    free(client);
    xcb_flush(rootconf.connection);
    xcb_ungrab_server(rootconf.connection);
    client_set_focus(NULL);
    layout_update();
} /*  }}} */

/* client_update_border_color - set the client border color {{{
 * in client->border_color
 */
void
client_update_border_color(client_t *client)
{
    if(!client)
        return;
    if(rootconf.client_focused == client)
        xcb_change_window_attributes(rootconf.connection, client->window,
                                     XCB_CW_BORDER_PIXEL,
                                     rootconf.appearance.border_color_focus);
    else
        xcb_change_window_attributes(rootconf.connection, client->window,
                                     XCB_CW_BORDER_PIXEL,
                                     rootconf.appearance.border_color_normal);
    xcb_flush(rootconf.connection);
} /*  }}} */

/* client_attach_stack - attach a client the stack {{{
 */
void
client_attach_stack(client_t *client)
{
    if(rootconf.stack)
        client->snext = rootconf.stack;
    else
        client->snext = NULL;
    rootconf.stack = client;
} /*  }}} */

/* client_detach_stack - detach a client from the stack {{{
 */
void
client_detach_stack(client_t *client)
{
    client_t **tmp;

    for(tmp = &rootconf.stack; *tmp && *tmp != client; tmp = &(*tmp)->snext);
    *tmp = client->snext;
} /*  }}} */

/* client_set_border_width - set the border width of a client {{{
 */
void
client_set_border_width(client_t *client, uint32_t width)
{
    client->border_width = width;
    xcb_configure_window(rootconf.connection, client->window,
                         XCB_CONFIG_WINDOW_BORDER_WIDTH, &width);
} /*  }}} */

/* client_unfocus - unfocus a client {{{
 */
void
client_unfocus(client_t *client)
{
    if(!client)
        return;
    xcb_change_window_attributes(rootconf.connection, client->window,
                                 XCB_CW_BORDER_PIXEL,
                                 rootconf.appearance.border_color_normal);
} /*  }}} */

/* client_update_name - update the name of a client {{{
 */
void
client_update_name(client_t *client)
{
    char *name;
    ssize_t len;

    if(!get_string_from_atom(client->window, _NET_WM_NAME, &name, &len))
        if(!get_string_from_atom(client->window, WM_NAME, &name, &len))
            return;
    client->name = name;
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
