/*
 * xutil.c - some useful X functions
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

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>

#include "xutil.h"
#include "util.h"
#include "structs.h"
#include "atoms/atoms.h"

/* set_error_handler - Set the handler for followings errors {{{
 * 
 * @evenths The event handler
 * @handler Function to be called if an error is produced
 * @data Extra data
 */
void
set_error_handler(xcb_event_handlers_t *evenths,
                                  xcb_generic_error_handler_t handler,
                                  void *data)
{
    int err_num;
    for(err_num = 0; err_num < 256; err_num++)
	xcb_event_set_error_handler(evenths, err_num, handler, data);
} /* }}} */

/* check_other_wm - Check if another wm is running {{{
 *
 * @c The connection to the X
 */
void
check_other_wm(xcb_connection_t *c)
{
    int screen_counter;
    for(screen_counter = 0;
        screen_counter < xcb_setup_roots_length(xcb_get_setup(c));
        screen_counter++)
    {
         const uint32_t select_input_val = 
            XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
        /* Return an error if other wm is running */
        xcb_change_window_attributes(c,
                screen_get(c, screen_counter)->root,
                XCB_CW_EVENT_MASK, &select_input_val);
    }
} /* }}} */

/* check_invalid_connection - check if we have a invalid X conenction {{{
 *
 * @connection the connection to do the check
 */
void
check_invalid_connection(xcb_connection_t *connection)
{
    if(xcb_connection_has_error(connection))
        die("invalid X connection");
} /* }}} */

/* visual_get_from_screen - Get a visual from an screen {{{
 *
 * @screen screen struct
 * return a new visual
 */
xcb_visualtype_t *
visual_get_from_screen(xcb_screen_t *screen)
{
    if(!screen)
        return NULL;

    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen);

    if(depth_iter.data)
        for(; depth_iter.rem; xcb_depth_next (&depth_iter))
            for(xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator (depth_iter.data);
                 visual_iter.rem; xcb_visualtype_next (&visual_iter))
                if(screen->root_visual == visual_iter.data->visual_id)
                {
                    if(visual_iter.data)
                        return visual_iter.data;
                    else
                        die("cannot get visual (X related error)");
                }

    return NULL;
} /* }}} */

/* get_default_screen - get the default screen {{{
 */
xcb_screen_t *
get_default_screen(void)
{
    return screen_get(rootconf.connection, rootconf.screen_default);
} /* }}} */

/* screen_default_visual - get the default of a screen {{{
 */
xcb_visualtype_t *
screen_default_visual(xcb_screen_t *screen)
{
    if(!screen)
        return NULL;
    
    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen);

    if(depth_iter.data)
        for(; depth_iter.rem; xcb_depth_next (&depth_iter))
            for(xcb_visualtype_iterator_t visual_iter =
                    xcb_depth_visuals_iterator (depth_iter.data);
                 visual_iter.rem; xcb_visualtype_next (&visual_iter))
                if(screen->root_visual == visual_iter.data->visual_id)
                    return visual_iter.data;
    return NULL;
} /*  }}} */

/* get_string_from_atom - get a string from an atom  {{{
 */
bool
get_string_from_atom(xcb_window_t window, xcb_atom_t atom, char **text,
                     ssize_t *len)
{
    xcb_get_text_property_reply_t reply;

    if(!xcb_get_text_property_reply(rootconf.connection,
                                    xcb_get_text_property_unchecked(rootconf.connection,
                                                                    window,
                                                                    atom),
                                    &reply, NULL) ||
       !reply.name_len || reply.format != 8)
    {
        /* An error ocurred, cleanup */
        xcb_get_text_property_reply_wipe(&reply);
        return false;
    }

    if(text && len)
    {
        if(reply.encoding == STRING
           || reply.encoding == UTF8_STRING
           || reply.encoding == COMPOUND_TEXT)
        {
            *text = ((char *)calloc(1, sizeof(char) * (reply.name_len + 1)));
            /* The retrieved string is not NULL terminated */
            memcpy(*text, reply.name, reply.name_len);
            (*text)[reply.name_len] = '\0';
            *len = reply.name_len;
        }
    }
    else
    {
        *text = NULL;
        *len  = 0;
    }

    xcb_get_text_property_reply_wipe(&reply);
    return true;
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
