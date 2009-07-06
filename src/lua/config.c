/*
 * config.c - lua functions for configuretion file management
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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>

#include "structs.h"
#include "util.h"
#include "lua/config.h"

/* clua_GetNumber - get a number from a provided with provided name
 */
lua_Number
clua_GetNumber(lua_State *L, const char *name)
{
    lua_getglobal(L, name);
    if(!lua_isnumber(L, -1))
        warning("config: '%s' must be a number", name);
    return lua_tonumber(L, -1);
}

/* clua_GetFloat - get a float
 */
void
clua_GetFloat(const char *name, float default_val, float *to_set)
{
    lua_getglobal(rootconf.L, name);
    if(!lua_isnumber(rootconf.L, -1))
    {
        warning("config: '%s' must be a number (is it defined?), setting default value", name);
        *to_set = default_val;
    }

    *to_set = (float)lua_tonumber(rootconf.L, -1);
    lua_pop(rootconf.L, 1);
}

/* clua_GetString - get a string
 */
void
clua_GetString(const char *name, const char *default_val, const char **to_set)
{
    lua_getglobal(rootconf.L, name);
    if(!lua_isstring(rootconf.L, -1))
    {
        warning("config: '%s' must be a string (is it defined?), setting default value", name);
        *to_set = default_val;
    }

    *to_set = lua_tostring(rootconf.L, -1);
    lua_pop(rootconf.L, 1);
}

/* clua_Init - init lua
 */
void
clua_Init(void)
{
    /* open a new stack */
    rootconf.L = lua_open();

    /* load base libraries */
    luaL_openlibs(rootconf.L);
}

/* clua_ParseConfig - parse the config file getting the options
 */
bool
clua_ParseConfig(void)
{
    if(luaL_loadfile(rootconf.L, "/home/feler/.ceresrc") ||
       lua_pcall(rootconf.L, 0, 0, 0))
        die("cannot run cofiguration file: %s\n", lua_tostring(rootconf.L, -1));

    clua_GetFloat("mfact", 0.55, &rootconf.config.mfact);
    clua_GetString("border_normal", "#000000", &rootconf.config.border_normal);
    clua_GetString("border_focus",  "#ffffff", &rootconf.config.border_focus);
    printf("%s\n", rootconf.config.border_normal);

    return true;
}

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
