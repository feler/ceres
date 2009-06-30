/*
 * config.c - Cres Python for parse the config file
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

#include <Python.h>

#include "structs.h"
#include "util.h"
#include "cerpy/config.h"

/* cerpy_GetString - get a string from the PyDict provided {{{
 */
const char *
cerpy_GetString(PyObject *dict, const char *name)
{
    const char *ret = "";

    if(PyDict_Check(dict))
    {
        PyObject *string = PyDict_GetItemString(dict, name);
        if(PyString_Check(string))
            ret = PyString_AsString(string);
        else
            warning("PyString: %s at PyDict provided, is not a string", name);
    }
    else
        die("PyObject provided is not a PyDict");

    return ret;
} /*  }}} */

/* cerpy_ParseConfig - parse the config file  {{{
 */
void
cerpy_ParseConfig(void)
{
    /* Append our config directory to PYTHONPATH for import our
     * config file */
    PyObject *sys_mod = PyImport_Import(PyString_FromString("sys"));
    PyRun_SimpleString("sys-path.append(\"/home/feler/.config/ceres/\")");
    Py_XDECREF(sys_mod);

    /* Import our config file */
    PyObject *config_mod = PyImport_Import(PyString_FromString("rc"));
    /* Get the dictionary */
    PyObject *config_dict = PyModule_GetDict(config_mod);
} /*  }}} */

// vim:et:sw=4:ts=8:softtabstop=4:cindent:fdm=marker:tw=80
