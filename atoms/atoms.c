/*
 * atoms.c - atoms functions
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

#include <string.h>

#include "structs.h"
#include "atoms/atoms.h"

typedef struct
{
    const char *name;
    xcb_atom_t *atom;
} atom_list_item_t;

#include "atoms/atoms_intern.h"


void
init_atoms(void)
{
    unsigned int i;
    xcb_intern_atom_cookie_t cookie_list[lenof(atom_list)];
    xcb_intern_atom_reply_t *reply;

    for(i = 0; i < lenof(atom_list); i++)
        cookie_list[i] = xcb_intern_atom_unchecked(rootconf.connection,
                false, strlen(atom_list[i].name) - 1,
                atom_list[i].name);
    for(i = 0; i < lenof(atom_list); i++)
    {
        reply = xcb_intern_atom_reply(rootconf.connection,
                    cookie_list[i], NULL);
        if(!reply)
            /* Oops error let's get the next atom */
            continue;

        *atom_list[i].atom = reply->atom;
    }
}
