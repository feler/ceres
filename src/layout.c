/*
 * layout.c - layout functions
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
#include <stdio.h>
#include <unistd.h>

#include "structs.h"
#include "client.h"
#include "layout.h"
#include "bar.h"

/* layout_update - reorganize the windows by the actual layout {{{
 */
void
layout_update(void)
{
    
    /*-------------------------------------------------------------------------
     *  ATTENTION! actually we only have one layout (tiled), when we will have
     *  more this functions should be modified.
     *  TODO: do more layouts
     *-------------------------------------------------------------------------
     */
    layout_tile();
    /* Update the list of clients */
    bar_update_task_list(rootconf.bar);
} /*  }}} */

/* layout_tile - arrange function for do the tiling {{{
 */
void
layout_tile(void)
{
    int x, y, w, h, mw;
    unsigned int i, n;
    client_t *client;

    /* Get the number of clients */
    for(n = 0, client = client_next_tiled(rootconf.clients);
        client; client = client_next_tiled(client->next), n++);
    if(n == 0)
        return;

    client = client_next_tiled(rootconf.clients);
    mw = rootconf.config.mfact * rootconf.workarea.width;
    client_resize_and_move(client, rootconf.workarea.x, rootconf.workarea.y,
                           (n == 1 ? rootconf.workarea.width : mw) - 2 * client->border_width,
                           rootconf.workarea.height - 2 * client->border_width);    

    if(--n == 0)
        return;

    x = rootconf.workarea.x + mw;
    y = rootconf.workarea.y;
    w = rootconf.workarea.width - mw;
    h = rootconf.workarea.height / n;

    for(i = 0, client = client_next_tiled(client->next); client; client = client_next_tiled(client->next), i++)
    {
        client_resize_and_move(client, x, y, w - 2 * client->border_width, ((i + 1 == n)
                               ? rootconf.workarea.y + rootconf.workarea.height - y - 2 * client->border_width
                               : h - 2 * client->border_width));
        if(h != rootconf.workarea.height)
            y = client->geometry.y + ((client)->geometry.height + 2 * (client)->border_width);
    }
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
