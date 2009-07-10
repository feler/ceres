/*
 * property.c - property management functions
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

#include <xcb/xcb_atom.h>

#include "structs.h"
#include "client.h"
#include "atoms/atoms.h"
#include "bar.h"
#include "property.h"

static int property_xrootpmap_id(void *data __attribute__ ((unused)),
                                xcb_connection_t *conn,
                                uint8_t state,
                                xcb_window_t window,
                                xcb_atom_t atom,
                                xcb_get_property_reply_t *reply)
{
    bar_need_update(rootconf.bar);

    return 0;
}

static int property_wm_name(void *data __attribute__ ((unused)),
                                xcb_connection_t *conn,
                                uint8_t state,
                                xcb_window_t window,
                                xcb_atom_t atom,
                                xcb_get_property_reply_t *reply)
{
    client_t *client = client_get_by_window(window);

    if(client)
    {
        client_update_name(client);
        bar_update_task_list(rootconf.bar);
    }

    return 0;
}

/* property_handlers_init - init the property handlers {{{
 */
void
property_handlers_init(void)
{
    xcb_property_handlers_init(&rootconf.prop_h, &rootconf.event_h);

    /* Background */
    xcb_property_set_handler(&rootconf.prop_h, _XROOTPMAP_ID, 1,
                             property_xrootpmap_id, NULL);

    xcb_property_set_handler(&rootconf.prop_h, _NET_WM_NAME, 1,
                             property_wm_name, NULL);
    xcb_property_set_handler(&rootconf.prop_h, WM_NAME, 1,
                             property_wm_name, NULL);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
