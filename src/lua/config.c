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

/* clua_GetNumber - get a number from a provided with provided name {{{
 */
lua_Number
clua_GetNumber(lua_State *L, const char *name)
{
    lua_getglobal(L, name);
    if(!lua_isnumber(L, -1))
        warning("config: '%s' must be a number, setting default value", name);
    return lua_tonumber(L, -1);
} /*  }}} */

/* clua_GetFloat - get a float {{{
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
} /*  }}} */

/* clua_GetString - get a string {{{
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
} /*  }}} */

/* clua_GetUint32_t - get a uint32_t {{{
 */
void
clua_GetUint32_t(const char *name, uint32_t default_val, uint32_t *to_set)
{
    lua_getglobal(rootconf.L, name);
    if(!lua_isnumber(rootconf.L, -1))
    {
        warning("config: '%s' must be a number (is it defined?), setting default value", name);
        *to_set = default_val;
    }

    *to_set = (uint32_t)lua_tonumber(rootconf.L, -1);
    lua_pop(rootconf.L, 1);
} /*  }}} */

/* clua_Init - init lua {{{
 */
void
clua_Init(void)
{
    /* open a new stack */
    rootconf.L = lua_open();

    /* load base libraries */
    luaL_openlibs(rootconf.L);
} /*  }}} */

/* clua_ParseConfig - parse the config file getting the options {{{
 */
bool
clua_ParseConfig(void)
{
    char *home_dir = getenv("HOME");
    char config_path[strlen(home_dir) + strlen("/.config/ceres/rc.lua")];
    sprintf(config_path, "%s%s", home_dir, "/.config/ceres/rc.lua");
    if(luaL_loadfile(rootconf.L, config_path) ||
       lua_pcall(rootconf.L, 0, 0, 0))
        die("cannot run cofiguration file: %s\n", lua_tostring(rootconf.L, -1));

    clua_GetFloat("mfact", 0.55, &rootconf.config.mfact);
    clua_GetString("border_normal", "#000000", &rootconf.config.border_normal);
    clua_GetString("border_focus",  "#ffffff", &rootconf.config.border_focus);
    clua_GetUint32_t("border_width", 1, &rootconf.config.border_width);

    return true;
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
