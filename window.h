/*
 * window.h - window handling header
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

#ifndef CERES_WINDOW_H
#define CERES_WINDOW_H

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

void window_take_focus(xcb_window_t);
void window_configure(xcb_window_t, uint16_t x, uint16_t y,
        uint16_t width, uint16_t height, int);
bool window_check_protocol(xcb_window_t, xcb_atom_t);
void window_set_focus(xcb_window_t);
void window_set_state(xcb_window_t, long);
xcb_get_property_cookie_t window_get_state_prepare(xcb_window_t);
uint32_t window_get_state(xcb_get_property_cookie_t);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
