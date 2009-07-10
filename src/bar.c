/*
 * bar.c - bar management functions
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

#include <cairo-xcb.h>
#include <xcb/xcb.h>
#include <pango/pangocairo.h>

#include "structs.h"
#include "xutil.h"
#include "bar.h"
#include "client.h"

/* bar_map - map a bar {{{
 */
void
bar_map(bar_t *bar)
{
    xcb_map_window(rootconf.connection, bar->window);
    xcb_flush(rootconf.connection);

    bar_need_update(bar);
} /*  }}} */

/* bar_new - create a bar {{{
 */
bar_t *
bar_new(void)
{
    xcb_screen_t *screen = get_default_screen();
    uint32_t vals[3];
    const uint32_t gc_mask    = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
    const uint32_t gc_vals[2] = { screen->black_pixel, screen->white_pixel };

    bar_t *bar;
    bar         = malloc(sizeof(bar_t));

    bar->geom.x      = 0;
    bar->geom.y      = 0;
    bar->geom.width  = screen->width_in_pixels;
    bar->geom.height = 15;
    bar->window      = xcb_generate_id(rootconf.connection);

    vals[0] = XCB_BACK_PIXMAP_PARENT_RELATIVE;
    vals[1] = 1;
    vals[2] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
        | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_ENTER_WINDOW
        | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY
        | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE
        | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE
        | XCB_EVENT_MASK_PROPERTY_CHANGE;

    xcb_create_window(rootconf.connection, screen->root_depth,
                      bar->window, screen->root,
                      bar->geom.x, bar->geom.y,
                      bar->geom.width, bar->geom.height, 0,
                      XCB_COPY_FROM_PARENT,
                      screen->root_visual,
                      XCB_CW_BACK_PIXMAP |
                      XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
                      vals);

    /* Set window to be at the top */
    const uint32_t value[] = { XCB_STACK_MODE_TOP_IF };
    xcb_configure_window(rootconf.connection, bar->window, XCB_CONFIG_WINDOW_STACK_MODE, value);

    bar->draw.pixmap = xcb_generate_id(rootconf.connection);
    xcb_create_pixmap(rootconf.connection, screen->root_depth, bar->draw.pixmap, screen->root,
                       bar->geom.width, bar->geom.height); 

    bar->gc = xcb_generate_id(rootconf.connection);
    xcb_create_gc(rootconf.connection, bar->gc, screen->root, gc_mask, gc_vals);

    /* Adjust workarea */
    rootconf.workarea.y += bar->geom.height;
    rootconf.workarea.height -= bar->geom.height;

    bar->draw.visual = screen_default_visual(get_default_screen());
    bar->draw.surface = cairo_xcb_surface_create(rootconf.connection, bar->draw.pixmap,
                                                 bar->draw.visual, bar->geom.width,
                                                 bar->geom.height);
    bar->draw.cr     = cairo_create(bar->draw.surface);
    bar->draw.layout = pango_cairo_create_layout(bar->draw.cr);

    /* First draw, empty the bar */
    bar_draw_rectangle(bar, bar->geom, rootconf.config.bg_normal);

    xcb_flush(rootconf.connection);

    return bar;
} /*  }}} */

/* bar_draw_rectangle - draw a rectangle in a bar  {{{
 */
void
bar_draw_rectangle(bar_t *bar, area_t geom, color_t color)
{
    cairo_set_antialias(bar->draw.cr, CAIRO_ANTIALIAS_NONE);
    cairo_set_line_width(bar->draw.cr, 1.0);
    cairo_set_miter_limit(bar->draw.cr, 10.0);
    cairo_set_line_join(bar->draw.cr, CAIRO_LINE_JOIN_MITER);
    cairo_set_source_rgb(bar->draw.cr,
                         color.red / 255.0,
                         color.green / 255.0,
                         color.blue / 255.0);
    cairo_rectangle(bar->draw.cr, geom.x, geom.y,
                    geom.width, geom.height);
    cairo_fill(bar->draw.cr);

    bar_need_update(bar);
} /*  }}} */

/* bar_draw - update the pixmap of a bar {{{
 */
void
bar_draw(bar_t *bar)
{
    xcb_copy_area(rootconf.connection, bar->draw.pixmap,
                  bar->window, bar->gc, 0, 0, 0, 0,
                  bar->geom.width,
                  bar->geom.height);
} /*  }}} */

/* bar_refresh - refresh the bar {{{
 */
void
bar_refresh(void)
{
    if(rootconf.bar->need_update)
    {
        bar_draw(rootconf.bar);
        rootconf.bar->need_update = false;
    }
} /*  }}} */

/* bar_need_update - notify that a bar need to be updated {{{
 */
void
bar_need_update(bar_t *bar)
{
    bar->need_update = true;
} /*  }}} */

/* bar_draw_text - draw text in a bar using pango and cairo {{{
 */
void
bar_draw_text(bar_t *bar, const char *text, area_t where, color_t color)
{
    pango_layout_set_text(bar->draw.layout, text, -1);
    pango_layout_set_font_description(bar->draw.layout, pango_font_description_from_string("Terminus 8"));
    pango_layout_set_width(bar->draw.layout, where.width);
    pango_layout_set_height(bar->draw.layout, where.height);

    cairo_move_to(bar->draw.cr, where.x, where.y);
    cairo_set_source_rgb(bar->draw.cr,
                         color.red / 255.0,
                         color.green / 255.0,
                         color.blue / 255.0);
    
    pango_cairo_update_layout(bar->draw.cr, bar->draw.layout);
    pango_cairo_show_layout(bar->draw.cr, bar->draw.layout);

    bar_need_update(bar);
} /*  }}} */

/* bar_update_task_list - update the task list of the bar {{{
 */
void
bar_update_task_list(bar_t *bar)
{
    unsigned int n, i;
    unsigned int single_width;
    client_t *client;
    area_t area;

    /* clean the bar */
    bar_draw_rectangle(bar, bar->geom, rootconf.config.bg_normal);

    /* Get number of clients */
    for(n = 0, client = client_next_tiled(rootconf.clients);
        client; client = client_next_tiled(client->next), n++);
    /* We have no clients, exit */
    if(n == 0)
        return;

    single_width = bar->geom.width / n;
    
    for(i = 0, client = client_next_tiled(rootconf.clients);
        client; client = client_next_tiled(client->next), i++)
    {
        area.x      = i == 0 ? 0 : single_width * i;
        area.y      = 0;
        area.width  = single_width;
        area.height = bar->geom.height;

        if(rootconf.client_focused == client)
            bar_draw_rectangle(bar, area, rootconf.config.bg_focus);
        else
            bar_draw_rectangle(bar, area, rootconf.config.bg_normal);
        area.y = 1;
        if(rootconf.client_focused == client)
            bar_draw_text(bar, client->name, area, rootconf.config.fg_focus);
        else
            bar_draw_text(bar, client->name, area, rootconf.config.fg_normal);
    }
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
