/*
 * key.h - key functions header 
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

#ifndef CERES_KEY_H
#define CERES_KEY_H

#include <xcb/xcb.h>

#include "structs.h"

/* Define some key modifiers, use i.e: Alt + Ctrl = 0x8 + 0x4 = 0x12
 * Windows + Shift = 0x40 + 0x1 */
#define WindowsKey 0x40
#define ShiftKey   0x1
#define ControlKey 0x4
#define AltKey     0x8

bool keyboard_grab(void);
void keyboard_ungrab(void);
void key_grab_to_window(xcb_window_t, key_bind_t);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
