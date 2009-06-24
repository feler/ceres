/*
 * mouse.h - mouse functions header
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


#ifndef  CERES_MOUSE_H
#define  CERES_MOUSE_H

#include <xcb/xcb.h>

xcb_cursor_t mouse_get_cursor(int);
int mouse_get_position_in_window(xcb_window_t, int16_t *, int16_t *,
                                 xcb_window_t *, uint16_t *);
int mouse_grab(void);
void mouse_ungrab(void);

#endif
