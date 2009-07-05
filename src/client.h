/*
 * client.h - client header
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

#ifndef CERES_CLIENT_H
#define CERES_CLIENT_H

#include <xcb/xcb.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

void client_detach(client_t *);
void client_attach_stack(client_t *);
void client_detach_stack(client_t *);
void client_update_border_color(client_t *);
void client_unmanage(client_t *);
void client_manage(xcb_window_t, xcb_get_geometry_reply_t *);
void client_attach(client_t *);
client_t *client_next_tiled(client_t *);
client_t *client_get_by_window(xcb_window_t);
void client_set_focus(client_t *);
void client_resize_and_move(client_t *, uint32_t, uint32_t, uint32_t, uint32_t);
void client_set_border_width(client_t *, uint32_t);
void client_unfocus(client_t *);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=8
