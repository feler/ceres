set(PROJECT_CER_NAME ceres)

# If ${SOURCE_DIR} is a git repository VERSION is set to
# `git describe` later.
set(VERSION devel)

set(CODENAME "Uniform")

project(${PROJECT_CER_NAME} C)

set(CMAKE_BUILD_TYPE RELEASE)

set(SOURCE_DIR ${SOURCE_DIR}/src)
set(CLUA_DIR ${SOURCE_DIR}/lua)

link_directories(/usr/local/lib)

# {{{ CFLAGS
add_definitions(-std=gnu99 -ggdb3 -fno-strict-aliasing -Wall -Wextra
    -Wchar-subscripts -Wundef -Wshadow -Wcast-align -Wwrite-strings
    -Wsign-compare -Wunused -Wno-unused-parameter -Wuninitialized -Winit-self
    -Wpointer-arith -Wredundant-decls -Wformat-nonliteral
    -Wno-format-zero-length -Wmissing-format-attribute -Wmissing-prototypes
    -Wstrict-prototypes)
# }}}

# {{{ Endianness
include(TestBigEndian)
TEST_BIG_ENDIAN(CERES_IS_BIG_ENDIAN)
# }}}

# {{{ Find external utilities
macro(a_find_program var prg req)
    set(required ${req})
    find_program(${var} ${prg})
    if(NOT ${var})
        message(STATUS "${prg} not found.")
        if(required)
            message(FATAL_ERROR "${prg} is required to build ceres")
        endif()
    else()
        message(STATUS "${prg} -> ${${var}}")
    endif()
endmacro()

a_find_program(CAT_EXECUTABLE cat TRUE)
a_find_program(LN_EXECUTABLE ln TRUE)
a_find_program(GREP_EXECUTABLE grep TRUE)
a_find_program(GIT_EXECUTABLE git FALSE)
a_find_program(HOSTNAME_EXECUTABLE hostname FALSE)
# pkg-config
find_package(PkgConfig)
# Lua
find_package(Lua51)
if(NOT LUA51_FOUND AND NOT LUA50_FOUND)
    message(FATAL_ERROR "lua library not found")
endif()
# }}}

# {{{ Version stamp
if(EXISTS ${SOURCE_DIR}/.git/HEAD AND GIT_EXECUTABLE)
    # get current version
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe
        WORKING_DIRECTORY ${SOURCE_DIR}
        OUTPUT_VARIABLE VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    # file the git-version-stamp.sh script will look into
    set(VERSION_STAMP_FILE ${BUILD_DIR}/.version_stamp)
    file(WRITE ${VERSION_STAMP_FILE} ${VERSION})
    # create a version_stamp target later
    set(BUILD_FROM_GIT TRUE)
elseif( EXISTS ${SOURCE_DIR}/.version_stamp )
    # get version from version stamp
    file(READ ${SOURCE_DIR}/.version_stamp VERSION)
endif()
# }}}

# {{{ Get hostname
execute_process(
    COMMAND ${HOSTNAME_EXECUTABLE}
    WORKING_DIRECTORY ${SOURCE_DIR}
    OUTPUT_VARIABLE BUILDHOSTNAME
    OUTPUT_STRIP_TRAILING_WHITESPACE)
# }}}

# {{{ Required libraries
#
# this sets up:
# CERES_REQUIRED_LIBRARIES
# CERES_REQUIRED_INCLUDE_DIRS

# Use pkgconfig to get most of the libraries
pkg_check_modules(CERES_COMMON_REQUIRED REQUIRED
    xcb>=1.1)

pkg_check_modules(CERES_REQUIRED REQUIRED
    glib-2.0
    cairo
    x11
    pango>=1.19.3
    pangocairo>=1.19.3
    xcb-randr
    xcb-xtest
    xcb-xinerama
    xcb-event>=0.3.4
    xcb-aux>=0.3.0
    xcb-atom>=0.3.0
    xcb-keysyms>=0.3.4
    xcb-icccm>=0.3.3
    xcb-image>=0.3.0
    xcb-property>=0.3.0
    cairo-xcb
    libstartup-notification-1.0>=0.10
    xproto>=7.0.15
    imlib2
    libxdg-basedir>=1.0.0)

if(NOT CERES_REQUIRED_FOUND OR NOT CERES_COMMON_REQUIRED_FOUND)
    message(FATAL_ERROR)
endif()

macro(a_find_library variable library)
    find_library(${variable} ${library})
    if(NOT ${variable})
        message(FATAL_ERROR ${library} " library not found.")
    endif()
endmacro()

# Check for libev
a_find_library(LIB_EV ev)

# Error check
set(CERES_REQUIRED_LIBRARIES
    ${CERES_COMMON_REQUIRED_LDFLAGS}
    ${CERES_REQUIRED_LIBRARIES}
    ${LIB_EV}
    ${LUA_LIBRARIES})

set(CERES_REQUIRED_INCLUDE_DIRS
    ${CERES_COMMON_REQUIRED_INCLUDE_DIRS}
    ${CERES_REQUIRED_INCLUDE_DIRS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_INCLUDE_DIR})
# }}}

# {{{ Optional libraries
#
# this sets up:
# CERES_OPTIONAL_LIBRARIES
# CERES_OPTIONAL_INCLUDE_DIRS

# }}}

# {{{ Install path and configuration variables
if(DEFINED PREFIX)
    set(PREFIX ${PREFIX} CACHE PATH "install prefix")
    set(CMAKE_INSTALL_PREFIX ${PREFIX})
else()
    set(PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "install prefix")
endif()

#If a sysconfdir is specified, use it instead
#of the default configuration dir.
if(DEFINED SYSCONFDIR)
    set(SYSCONFDIR ${SYSCONFDIR} CACHE PATH "config directory")
else()
    set(SYSCONFDIR ${PREFIX}/etc CACHE PATH "config directory")
endif()

#If an XDG Config Dir is specificed, use it instead
#of the default XDG configuration dir.
if(DEFINED XDG_CONFIG_DIR)
    set(XDG_CONFIG_DIR ${XDG_CONFIG_DIR} CACHE PATH "xdg config directory")
else()
    set(XDG_CONFIG_DIR ${SYSCONFDIR}/xdg CACHE PATH "xdg config directory")
endif()

# setting CERES_DOC_PATH
if(DEFINED CERES_DOC_PATH)
    set(CERES_DOC_PATH ${CERES_DOC_PATH} CACHE PATH "ceres docs directory")
else()
    set(CERES_DOC_PATH ${PREFIX}/share/doc/${PROJECT_CER_NAME} CACHE PATH "ceres docs directory")
endif()

# setting CERES_XSESSION_PATH
if(DEFINED CERES_XSESSION_PATH)
    set(CERES_XSESSION_PATH ${CERES_XSESSION_PATH} CACHE PATH "ceres xsessions directory")
else()
    set(CERES_XSESSION_PATH ${PREFIX}/share/xsessions CACHE PATH "ceres xsessions directory")
endif()

# set man path
if(DEFINED CERES_MAN_PATH)
   set(CERES_MAN_PATH ${CERES_MAN_PATH} CACHE PATH "ceres manpage directory")
else()
   set(CERES_MAN_PATH ${PREFIX}/share/man CACHE PATH "ceres manpage directory")
endif()

# Hide to avoid confusion
mark_as_advanced(CMAKE_INSTALL_PREFIX)

set(CERES_VERSION          ${VERSION})
set(CERES_COMPILE_MACHINE  ${CMAKE_SYSTEM_PROCESSOR})
set(CERES_COMPILE_HOSTNAME ${BUILDHOSTNAME})
set(CERES_COMPILE_BY       $ENV{USER})
set(CERES_RELEASE          ${CODENAME})
set(CERES_SYSCONFDIR       ${XDG_CONFIG_DIR}/${PROJECT_CER_NAME})
# }}}

# vim: filetype=cmake:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=80
