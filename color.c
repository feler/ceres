/*
 * color.c - color functions
 *
 * Copyright © 2009 Ángel Alonso <feler@archlinux.us>
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
#include "color.h"

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <malloc.h>
#include <stdlib.h>

#include "structs.h"
#include "xutil.h"
#include "draw.h"
#include "util.h"

/* color_parse - parse an hexadecimal color string to its component. {{{
 * \param colstr The color string.
 * \param len The color string length.
 * \param red A pointer to the red color to fill.
 * \param green A pointer to the green color to fill.
 * \param blue A pointer to the blue color to fill.
 * \param alpha A pointer to the alpha color to fill.
 */
int
color_parse(const char *colstr, ssize_t len,
            uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *alpha)
{
    unsigned long colnum;
    char *p;

    if(len == 7)
    {
        colnum = strtoul(colstr + 1, &p, 16);
        if(p - colstr != 7)
            goto invalid;
        *alpha = 0xff;
    }
    /* we have alpha */
    else if(len == 9)
    {
        colnum = strtoul(colstr + 1, &p, 16);
        if(p - colstr != 9)
            goto invalid;
        *alpha = colnum & 0xff;
        colnum >>= 8;
    }
    else
    {
      invalid:
        warning("ceres: invalid color: %s", colstr);
        return 0;
    }

    *red   = (colnum >> 16) & 0xff;
    *green = (colnum >> 8) & 0xff;
    *blue  = colnum & 0xff;

    return 1;
} /*  }}} */

/* init_RGB_color - init a RGB color (#ffffff, #000000,...) {{{
 */
int
init_RGB_color(uint16_t red, uint16_t green, uint16_t blue)
{
    xcb_screen_t *screen = get_default_screen();

    xcb_alloc_color_reply_t *reply = xcb_alloc_color_reply(rootconf.connection,
            xcb_alloc_color(rootconf.connection,
                screen->default_colormap,
                red,
                green,
                blue),
            NULL);

    if(!reply)
        return 0;
    free(reply);

    return 1;
} /*  }}} */

/* init_named_color - init a named color (white, black, ...)  {{{
 */
int
init_named_color(ssize_t len, const char *color_string)
{
    xcb_screen_t *screen = get_default_screen();

    xcb_alloc_named_color_reply_t *reply = xcb_alloc_named_color_reply(rootconf.connection,
            xcb_alloc_named_color(rootconf.connection,
                screen->default_colormap,
                len,
                color_string),
            NULL);
    if(!reply)
        return 0;

    free(reply);
    return 1;

} /*  }}} */

/* color_init - init a color it can be named or RGB {{{
 */
int
color_init(const char *colstr, ssize_t len)
{
    uint8_t red, green, blue, alpha;
    /* The color is given in RGB value */ 
    if(colstr[0] == '#') {
        if(!color_parse(colstr, len, &red, &green, &blue, &alpha))
        {
            warning("invalid color: %s", colstr);
            return 0;
        }

        if(!init_RGB_color(RGB_8TO16(red), RGB_8TO16(green), RGB_8TO16(blue)))
            warning("cannot alloc color: %s", colstr);
        else
            return 1;
        
        return 0;
    }
    else
    {
        if(!init_named_color(len, colstr))
            warning("cannot alloc color: %s", colstr);
        else
            return 1;

        return 0;
    }

} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
