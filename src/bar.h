/*
 * bar.h - bar management header
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

#ifndef CERES_BAR_H
#define CERES_BAR_H

bar_t *bar_new(void);
void bar_map(bar_t  *);
void bar_draw_rectangle(bar_t *, area_t);
void bar_draw(bar_t *);
void bar_refresh(void);
void bar_draw_text(bar_t *, const char *, area_t);
void bar_need_update(bar_t *);
void bar_update_task_list(bar_t *);

#endif

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
