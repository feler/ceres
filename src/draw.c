/*
 * draw.c - draw functions
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

#include <stdio.h>

#include <cairo-xcb.h>

#include "structs.h"
#include "xutil.h"
#include "draw.h"

/* font_new - create a new font {{{
 *
 * \return a new font_t struct
 */
font_t *
font_new(const char *font_name)
{
    xcb_screen_t *s = screen_get(rootconf.connection,
                                rootconf.screen_default);
    cairo_surface_t *surface;
    cairo_t *cr;
    PangoLayout *layout;
    font_t *font;
    font = malloc(sizeof(font_t));

    /* Create a cairo surface */
    surface = cairo_xcb_surface_create(rootconf.connection,
                                       rootconf.screen_default,
                                       visual_get_from_screen(s),
                                       s->width_in_pixels,
                                       s->height_in_pixels);

    cr = cairo_create(surface);
    /* Pango layout */
    layout = pango_cairo_create_layout(cr);

    /* Get the font descriptionription used to set text on a PangoLayout */
    font->description = pango_font_description_from_string(font_name);
    pango_layout_set_font_description(layout, font->description);

    /* Get height */
    pango_layout_get_pixel_size(layout, NULL, &font->height);

    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return font;
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
