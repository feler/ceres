/*
 * config.h - lua config management header
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

#ifndef CLUA_CONFIG_H
#define CLUA_CONFIG_H

#include <lua.h>

lua_Number clua_GetNumber(lua_State *, const char *);
void clua_Init(void);
bool clua_ParseConfig(void);
void clua_GetFloat(const char *, float, float *);
void clua_GetString(const char *, const char *, const char **);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
