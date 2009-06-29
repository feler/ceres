/*
 * ceres.c - Ceres windows manager
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

const char *VERSION = "-0.0001 - not functional";

/* Includes {{{ */
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <locale.h>
#include <getopt.h>
#include <assert.h>

#include <ev.h>
#include <basedir.h>
#include <signal.h>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_event.h>

#include "structs.h"
#include "xutil.h"
#include "util.h"
#include "event.h"
#include "client.h"
#include "color.h"
#include "atoms/atoms.h"
#include "screen.h"
#include "mouse.h"
#include "root.h"
#include "key.h"
#include "window.h"
/* }}} */

ceres_t rootconf;

/* Ceres General Functions {{{ */

/* event_pre_handler - Procces the events and plays in consecuence {{{
 * Unused args
 */
static void
event_pre_handler(EV_P_ ev_check *w, int revents)
{
    xcb_generic_event_t *event;
    xcb_generic_event_t *mouse = NULL;

    /*--------------------------------------------------------------------
     *  ATTENTION: We can afford to handle all mouse motion events
     *  bacause it will consume much CPU. We will process it after
     *  a bunch of events.
     *--------------------------------------------------------------------
     */
    while((event = xcb_poll_for_event(rootconf.connection)))
    {
        fprintf(stderr, "Event, %d\n", XCB_EVENT_RESPONSE_TYPE(event));

        if(XCB_EVENT_RESPONSE_TYPE(event) == XCB_MOTION_NOTIFY)
            mouse = event;
        else
            xcb_event_handle(&rootconf.event_h, event);
    }
    if(mouse)
    {
        xcb_event_handle(&rootconf.event_h, mouse);
    }

    ceres_refresh();
} /* }}} */

/* init_colors_and_font - init colors and font {{{
 */
static int
init_colors_and_font(void)
{
    color_init("black", sizeof("black") - 1);
    color_init("white", sizeof("white") - 1);

    rootconf.font = font_new("sans 8");

    return 1;
} /* }}} */

/* at_exit - exit sequence {{{ 
 */
static void
at_exit(void)
{
    //Py_Finalize();

    xcb_flush(rootconf.connection);
    xcb_disconnect(rootconf.connection);
} /*  }}} */

/* other_wm_error - callback for the other other wm running error  {{{
 * @params unused
 */
static int
other_wm_error(void *data,
            xcb_connection_t * c,
            xcb_generic_error_t * error) 
{
    die("another windows manager is already running");
} /*  }}} */

/* x_error - default error function. {{{
 * There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's).  Other types of errors call Xlibs
 * default error handler, which may call exit.
 * \param data Currently unused.
 * \param c The connectiont to the X server.  
 * \param e The error event.
 * \return 0 if no error, or xerror's xlib return status.
 */
static int
x_error(void *data,
       xcb_connection_t *c,
       xcb_generic_error_t *e)
{
    /* error indentifiers */
    if(e->error_code == XCB_EVENT_ERROR_BAD_WINDOW
       || (e->error_code == XCB_EVENT_ERROR_BAD_MATCH
           && XCB_EVENT_REQUEST_TYPE(e) == XCB_SET_INPUT_FOCUS)
       || (e->error_code == XCB_EVENT_ERROR_BAD_VALUE
           && XCB_EVENT_REQUEST_TYPE(e) == XCB_KILL_CLIENT)
       || (XCB_EVENT_REQUEST_TYPE(e) == XCB_CONFIGURE_WINDOW
           && e->error_code == XCB_EVENT_ERROR_BAD_MATCH))
        return 0;

    warning("X error: request=%s, error=%s",
            xcb_event_get_request_label(XCB_EVENT_REQUEST_TYPE(e)),
            xcb_event_get_error_label(e->error_code));

    return 0;
} /*  }}} */

/* exit_signal - callback for the exit EV signal {{{
 */
static void
exit_signal(EV_P_ ev_signal *w, int revents)
{
    ev_unloop(EV_A_ 1);
} /* }}} */

/* io_event - empty function!! {{{
 */
static void
io_event(EV_P_ ev_io *w, int revents)
{
    /* Empty */
} /* }}} */

/* ceres_restart - restart ceres {{{
 * Execute the exit sequence and then execute ceres 
 */
static void
ceres_restart(void)
{ 
    at_exit();

    static const char *shell = NULL;

    if(!shell && !(shell = getenv("SHELL")))
        shell = "/bin/sh";

    execl(shell, shell, "-c", rootconf.argv, NULL);
} /* }}} */

/* restart_signal - Callback for the EV restart signal {{{
 * \params not used
 */
static void
restart_signal(EV_P_ ev_signal *w, int revents)
{ 
    ceres_restart(); 
} /* }}} */

/* exit_help - exit showing the help {{{
 * \param code The exit code
 */
static void
exit_help(int code)
{
    FILE *outfile = (code == 0) ? stdout : stderr;
    fprintf(outfile, "Usage: ceres [OPTION]\n"
                    "\n"
                    "Options:\n"
                    "    -h, --help         show help\n"
                    "    -v, --version      show version\n");
    exit(code);
} /* }}} */

/* ceres_signal_handler - handle some signals {{{ */
static void
ceres_signal_handler(int signal_num)
{
    switch(signal_num)
    {
        case SIGSEGV:
            /* We have a segmentation fault, we are trying to acces
             * some memory area that we are not permited to */
            die("Segmentation fault, please report");
    }
} /* }}} */

/* }}} */

/* scan - search for windows to manage {{{
 */
static void
scan(void)
{
    int i, phys_screen, tree_c_len;
    const int screen_max = xcb_setup_roots_length(xcb_get_setup(rootconf.connection));
    root_window_t root_wins[screen_max];
    xcb_query_tree_reply_t *tree_r;
    xcb_window_t *wins = NULL;
    xcb_get_window_attributes_reply_t *attr_r;
    xcb_get_geometry_reply_t *geom_r;
    long state;

    for(phys_screen = 0; phys_screen < screen_max; phys_screen++)
    {
        /* Get the root window ID associated to this screen */
        root_wins[phys_screen].window = screen_get(rootconf.connection, phys_screen)->root;

        /* Get the window tree associated to this screen */
        root_wins[phys_screen].tree_cookie = xcb_query_tree_unchecked(rootconf.connection,
                                                                      root_wins[phys_screen].window);
    }

    for(phys_screen = 0; phys_screen < screen_max; phys_screen++)
    {
        tree_r = xcb_query_tree_reply(rootconf.connection,
                                      root_wins[phys_screen].tree_cookie,
                                      NULL);

        if(!tree_r)
            continue;

        /* Get the tree of the children windows of the current root window */
        if(!(wins = xcb_query_tree_children(tree_r)))
            die("cannot get tree children");

        tree_c_len = xcb_query_tree_children_length(tree_r);
        xcb_get_window_attributes_cookie_t attr_wins[tree_c_len];
        xcb_get_property_cookie_t state_wins[tree_c_len];

        for(i = 0; i < tree_c_len; i++)
        {
            attr_wins[i] = xcb_get_window_attributes_unchecked(rootconf.connection,
                                                               wins[i]);

            state_wins[i] = window_get_state_prepare(wins[i]);
        }

        xcb_get_geometry_cookie_t *geom_wins[tree_c_len];

        for(i = 0; i < tree_c_len; i++)
        {
            attr_r = xcb_get_window_attributes_reply(rootconf.connection,
                                                     attr_wins[i],
                                                     NULL);

            state = window_get_state(state_wins[i]);

            if(!attr_r || attr_r->override_redirect
               || attr_r->map_state == XCB_MAP_STATE_UNVIEWABLE
               || state == XCB_WM_STATE_WITHDRAWN)
            {
                geom_wins[i] = NULL;
                continue;
            }

            /* Get the geometry of the current window */
            //geom_wins[i] = p_alloca(xcb_get_geometry_cookie_t, 1);
            *(geom_wins[i]) = xcb_get_geometry_unchecked(rootconf.connection, wins[i]);
        }

        for(i = 0; i < tree_c_len; i++)
        {
            if(!geom_wins[i] || !(geom_r = xcb_get_geometry_reply(rootconf.connection,
                                                                  *(geom_wins[i]), NULL)))
                continue;

            /* The window can be mapped, so force it to be undrawn for startup */
            xcb_unmap_window(rootconf.connection, wins[i]);

            client_manage(wins[i], geom_r);
        }
    }
} /* }}} */

/* Main {{{ */
int
main(int argc, char **argv)
{
    int opt, xfd;
    xdgHandle xdg;
    struct sigaction act;
    static struct option long_options[] =
    {
        { "help",    0, NULL, 'h' },
        { "version", 0, NULL, 'v' },
        { NULL,      0, NULL, '0' }
    };

    /* Set the locale, text won't be properly printed otherwise */
    if(!setlocale(LC_CTYPE, ""))
        warning("cannot set the locale(LC_CTYPE),"
                " probably the text won't be properly printed");

    /* Get XDG basedir info */
    xdgInitHandle(&xdg);

    /* Save args */
    rootconf.argv = argv[0];

    /* Process the args */
    while((opt = getopt_long(argc, argv, "vh:",
                    long_options, NULL)) != -1)
        switch(opt)
        {
            case 'v':
                printf("ceres %s\n", VERSION);
                return 0;
                break;
            case 'h':
                exit_help(0);
                break;
        }

    /* Init the ev loop */
    rootconf.loop = ev_default_loop(0);
    if(!rootconf.loop)
        die("cannot init libev, bad $LIB_EV in environment");

    /* Register signals */ 
    ev_timer_init(&rootconf.timer, &on_timer, 0., 0.);

    ev_signal sigint,sigterm,sighup;
    ev_signal_init(&sigint, exit_signal, SIGINT);
    ev_signal_init(&sigterm, exit_signal, SIGTERM);
    ev_signal_init(&sighup, restart_signal, SIGHUP);
    ev_signal_start(rootconf.loop, &sigint);
    ev_signal_start(rootconf.loop, &sigterm);
    ev_signal_start(rootconf.loop, &sighup);
    ev_unref(rootconf.loop);
    ev_unref(rootconf.loop);
    ev_unref(rootconf.loop);   

    act.sa_handler = ceres_signal_handler;
    act.sa_flags = 0;
    memset(&act.sa_mask, 0, sizeof(sigset_t));
    sigaction(SIGSEGV, &act, 0);

    /* Init Python, is planned to use python for config files */
    //Py_Initialize();

    /* Connect to the X server */
    rootconf.connection = xcb_connect(NULL, &rootconf.screen_default);
    if(xcb_connection_has_error(rootconf.connection))
        die("X connection invalid!");

    /* Cast a handler for errors and events */
    xcb_event_handlers_init(rootconf.connection, &rootconf.event_h);
    set_error_handler(&rootconf.event_h, other_wm_error, NULL);

    /* Start to grab the server */
    xcb_grab_server(rootconf.connection);
    xcb_flush(rootconf.connection);

    /* check events setup */
    ev_check xcheck;
    ev_io xio = { .fd = -1 };
    xfd = xcb_get_file_descriptor(rootconf.connection);
    ev_io_init(&xio, &io_event, xfd, EV_READ);
    ev_io_start(rootconf.loop, &xio);
    ev_check_init(&xcheck, &event_pre_handler);
    ev_check_start(rootconf.loop, &xcheck);
    ev_unref(rootconf.loop);

    /* Check if other window manager is running 
     * This will put an error in the xcb error stack if another
     * window manager is running */
    check_other_wm(rootconf.connection);

    /* Check if an error has been produced (if any)
     * Like the other wm error */
    xcb_aux_sync(rootconf.connection);
    xcb_event_poll_for_event_loop(&rootconf.event_h);

    /*  Set the default error handler (x_error) */
    set_error_handler(&rootconf.event_h, x_error, NULL);

    /* Init atoms */
    /*-------------------------------------------------------------------------
     *  HOW ATOMS WORK
     *  In ./atoms/ there is a python script that read the names of determinated
     *  atoms in a text, and generate his respectives variables, init_atoms()
     *  set the values of those variables which will used like: WM_PROTOCOLS,
     *  WM_NAME, ...
     *-------------------------------------------------------------------------
     */
    init_atoms();

    /* alloc key symbols  */
    rootconf.key_symbols = xcb_key_symbols_alloc(rootconf.connection);

    /* Init colors (black and white) and the font */
    init_colors_and_font();

    /* Init all the screens (select for events too) */
    init_screens();

    /* Set the cursor image in all root windows */
    root_window_set_cursor(68); // 68 = left arrow

    /* Scan windows */
    scan();
    
    /* Process for errors if any */
    xcb_event_poll_for_event_loop(&rootconf.event_h);

    event_set_handlers();

    /* Ungrab server, we will receive events */
    xcb_ungrab_server(rootconf.connection);
    xcb_flush(rootconf.connection);

    /* Refresh before events */
    ceres_refresh();

    /* ---- MAIN LOOP ---- */
    ev_loop(rootconf.loop, 0);

    /* Exit */
    at_exit();
    
    return 0;
} /* }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
