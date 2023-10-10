/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <FramelessHelper/Core/framelesshelpercore_global.h>

/*
 * Copyright (C) 2001-2006 Bart Massey, Jamey Sharp, and Josh Triplett.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or their
 * institutions shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the authors.
 */

// This one is not included in any public headers.
using Display = struct _XDisplay;

#if 0 // Causes segfault
#ifndef FRAMELESSHELPER_HAS_XLIB
extern "C"
{

FRAMELESSHELPER_CORE_API int
XInitThreads(
    void
);

} // extern "C"
#endif // FRAMELESSHELPER_HAS_XLIB
#endif

#if __has_include(<xcb/xcb.h>)
#  include <xcb/xcb.h>
#  define FRAMELESSHELPER_HAS_XCB
#else // !__has_include(<xcb/xcb.h>)

using xcb_connection_t = struct xcb_connection_t;
using xcb_button_t = uint8_t;
using xcb_window_t = uint32_t;
using xcb_timestamp_t = uint32_t;
using xcb_atom_t = uint32_t;

using xcb_generic_error_t = struct xcb_generic_error_t
{
    uint8_t response_type;
    uint8_t error_code;
    uint16_t sequence;
    uint32_t resource_id;
    uint16_t minor_code;
    uint8_t major_code;
    uint8_t pad0;
    uint32_t pad[5];
    uint32_t full_sequence;
};

using xcb_client_message_data_t = union xcb_client_message_data_t
{
    uint8_t data8[20];
    uint16_t data16[10];
    uint32_t data32[5];
};

using xcb_button_press_event_t = struct xcb_button_press_event_t
{
    uint8_t response_type;
    xcb_button_t detail;
    uint16_t sequence;
    xcb_timestamp_t time;
    xcb_window_t root;
    xcb_window_t event;
    xcb_window_t child;
    int16_t root_x;
    int16_t root_y;
    int16_t event_x;
    int16_t event_y;
    uint16_t state;
    uint8_t same_screen;
    uint8_t pad0;
};
using xcb_button_release_event_t = xcb_button_press_event_t;

using xcb_void_cookie_t = struct xcb_void_cookie_t
{
    unsigned int sequence;
};

using xcb_intern_atom_cookie_t = struct xcb_intern_atom_cookie_t
{
    unsigned int sequence;
};

using xcb_intern_atom_reply_t = struct xcb_intern_atom_reply_t
{
    uint8_t response_type;
    uint8_t pad0;
    uint16_t sequence;
    uint32_t length;
    xcb_atom_t atom;
};

using xcb_client_message_event_t = struct xcb_client_message_event_t
{
    uint8_t response_type;
    uint8_t format;
    uint16_t sequence;
    xcb_window_t window;
    xcb_atom_t type;
    xcb_client_message_data_t data;
};

using xcb_get_property_reply_t = struct xcb_get_property_reply_t
{
    uint8_t response_type;
    uint8_t format;
    uint16_t sequence;
    uint32_t length;
    xcb_atom_t type;
    uint32_t bytes_after;
    uint32_t value_len;
    uint8_t pad0[12];
};

using xcb_get_property_cookie_t = struct xcb_get_property_cookie_t
{
    unsigned int sequence;
};

using xcb_list_properties_cookie_t = struct xcb_list_properties_cookie_t
{
    unsigned int sequence;
};

using xcb_list_properties_reply_t = struct xcb_list_properties_reply_t
{
    uint8_t response_type;
    uint8_t pad0;
    uint16_t sequence;
    uint32_t length;
    uint16_t atoms_len;
    uint8_t pad1[22];
};

[[maybe_unused]] inline constexpr const auto XCB_NONE = 0;
[[maybe_unused]] inline constexpr const auto XCB_WINDOW_NONE = 0;
[[maybe_unused]] inline constexpr const auto XCB_CURRENT_TIME = 0;
[[maybe_unused]] inline constexpr const auto XCB_PROP_MODE_REPLACE = 0;
[[maybe_unused]] inline constexpr const auto XCB_ATOM_ATOM = 4;
[[maybe_unused]] inline constexpr const auto XCB_ATOM_CARDINAL = 6;
[[maybe_unused]] inline constexpr const auto XCB_ATOM_WINDOW = 33;
[[maybe_unused]] inline constexpr const auto XCB_BUTTON_INDEX_ANY = 0;
[[maybe_unused]] inline constexpr const auto XCB_BUTTON_INDEX_1 = 1;
[[maybe_unused]] inline constexpr const auto XCB_BUTTON_INDEX_2 = 2;
[[maybe_unused]] inline constexpr const auto XCB_BUTTON_INDEX_3 = 3;
[[maybe_unused]] inline constexpr const auto XCB_BUTTON_RELEASE = 5;
[[maybe_unused]] inline constexpr const auto XCB_CLIENT_MESSAGE = 33;
[[maybe_unused]] inline constexpr const auto XCB_EVENT_MASK_STRUCTURE_NOTIFY = 131072;
[[maybe_unused]] inline constexpr const auto XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT = 1048576;
[[maybe_unused]] inline constexpr const auto XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY = 524288;
#endif // __has_include(<xcb/xcb.h>)

[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_TOP = 1;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_TOPRIGHT = 2;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_RIGHT = 3;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT = 4;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_BOTTOM = 5;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT = 6;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_LEFT = 7;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_MOVE = 8;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_SIZE_KEYBOARD = 9;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_MOVE_KEYBOARD = 10;
[[maybe_unused]] inline constexpr const auto _NET_WM_MOVERESIZE_CANCEL = 11;

[[maybe_unused]] inline constexpr const char ATOM_NET_SUPPORTED[] = "_NET_SUPPORTED";
[[maybe_unused]] inline constexpr const char ATOM_NET_WM_NAME[] = "_NET_WM_NAME";
[[maybe_unused]] inline constexpr const char ATOM_NET_WM_MOVERESIZE[] = "_NET_WM_MOVERESIZE";
[[maybe_unused]] inline constexpr const char ATOM_NET_SUPPORTING_WM_CHECK[] = "_NET_SUPPORTING_WM_CHECK";
[[maybe_unused]] inline constexpr const char ATOM_NET_KDE_COMPOSITE_TOGGLING[] = "_NET_KDE_COMPOSITE_TOGGLING";
[[maybe_unused]] inline constexpr const char ATOM_KDE_NET_WM_BLUR_BEHIND_REGION[] = "_KDE_NET_WM_BLUR_BEHIND_REGION";
[[maybe_unused]] inline constexpr const char ATOM_GTK_SHOW_WINDOW_MENU[] = "_GTK_SHOW_WINDOW_MENU";
[[maybe_unused]] inline constexpr const char ATOM_DEEPIN_NO_TITLEBAR[] = "_DEEPIN_NO_TITLEBAR";
[[maybe_unused]] inline constexpr const char ATOM_DEEPIN_FORCE_DECORATE[] = "_DEEPIN_FORCE_DECORATE";
[[maybe_unused]] inline constexpr const char ATOM_NET_WM_DEEPIN_BLUR_REGION_MASK[] = "_NET_WM_DEEPIN_BLUR_REGION_MASK";
[[maybe_unused]] inline constexpr const char ATOM_NET_WM_DEEPIN_BLUR_REGION_ROUNDED[] = "_NET_WM_DEEPIN_BLUR_REGION_ROUNDED";
[[maybe_unused]] inline constexpr const char ATOM_UTF8_STRING[] = "UTF8_STRING";

#ifndef FRAMELESSHELPER_HAS_XCB
extern "C"
{

FRAMELESSHELPER_CORE_API xcb_void_cookie_t
xcb_send_event(
    xcb_connection_t *connection,
    uint8_t propagate,
    xcb_window_t destination,
    uint32_t event_mask,
    const char *event
);

FRAMELESSHELPER_CORE_API int
xcb_flush(
    xcb_connection_t *connection
);

FRAMELESSHELPER_CORE_API xcb_intern_atom_cookie_t
xcb_intern_atom(
    xcb_connection_t *connection,
    uint8_t only_if_exists,
    uint16_t name_len,
    const char *name
);

FRAMELESSHELPER_CORE_API xcb_intern_atom_reply_t *
xcb_intern_atom_reply(
    xcb_connection_t *connection,
    xcb_intern_atom_cookie_t cookie,
    xcb_generic_error_t **error
);

FRAMELESSHELPER_CORE_API xcb_void_cookie_t
xcb_ungrab_pointer(
    xcb_connection_t *connection,
    xcb_timestamp_t time
);

FRAMELESSHELPER_CORE_API xcb_void_cookie_t
xcb_change_property(
    xcb_connection_t *connection,
    uint8_t mode,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint8_t format,
    uint32_t data_len,
    const void *data
);

FRAMELESSHELPER_CORE_API xcb_void_cookie_t
xcb_delete_property_checked(
    xcb_connection_t *connection,
    xcb_window_t window,
    xcb_atom_t property
);

FRAMELESSHELPER_CORE_API xcb_get_property_cookie_t
xcb_get_property(
    xcb_connection_t *connection,
    uint8_t _delete,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint32_t long_offset,
    uint32_t long_length
);

FRAMELESSHELPER_CORE_API xcb_get_property_reply_t *
xcb_get_property_reply(
    xcb_connection_t *connection,
    xcb_get_property_cookie_t cookie,
    xcb_generic_error_t **error
);

FRAMELESSHELPER_CORE_API void *
xcb_get_property_value(
    const xcb_get_property_reply_t *reply
);

FRAMELESSHELPER_CORE_API int
xcb_get_property_value_length(
    const xcb_get_property_reply_t *reply
);

FRAMELESSHELPER_CORE_API xcb_list_properties_cookie_t
xcb_list_properties(
    xcb_connection_t *connection,
    xcb_window_t window
);

FRAMELESSHELPER_CORE_API xcb_list_properties_reply_t *
xcb_list_properties_reply(
    xcb_connection_t *connection,
    xcb_list_properties_cookie_t cookie,
    xcb_generic_error_t **error
);

FRAMELESSHELPER_CORE_API int
xcb_list_properties_atoms_length(
    const xcb_list_properties_reply_t *atom
);

FRAMELESSHELPER_CORE_API xcb_atom_t *
xcb_list_properties_atoms(
    const xcb_list_properties_reply_t *atom
);

FRAMELESSHELPER_CORE_API xcb_get_property_cookie_t
xcb_get_property_unchecked(
    xcb_connection_t *connection,
    uint8_t _delete,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint32_t long_offset,
    uint32_t long_length
);

} // extern "C"
#endif // FRAMELESSHELPER_HAS_XCB

/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#if __has_include(<gtk/gtk.h>)
#  undef signals // Workaround a compilation issue caused by GTK.
#  include <gtk/gtk.h>
#  define FRAMELESSHELPER_HAS_GTK
#else // !__has_include(<gtk/gtk.h>)

#define G_VALUE_INIT  { 0, { { 0 } } }
#define g_signal_connect(instance, detailed_signal, c_handler, data) \
    g_signal_connect_data((instance), (detailed_signal), (c_handler), (data), nullptr, G_CONNECT_DEFAULT)

using gint = int;
using glong = long;
using gshort = short;
using gboolean = int;
using gushort = unsigned short;
using guint = unsigned int;
using gulong = unsigned long;
using gfloat = float;
using gdouble = double;
using gchar = char;
using guchar = unsigned char;
using gchararray = char *;
using gpointer = void *;
using gint64 = signed long;
using guint64 = unsigned long;
using gsize = unsigned int;

using GType = unsigned long; // TODO: or unsigned int?
using GValue = struct _GValue;
using GObject = struct _GObject;
using GClosure = struct _GClosure;
using GtkSettings = struct _GtkSettings;

using GConnectFlags = enum GConnectFlags
{
    G_CONNECT_DEFAULT = 0,
    G_CONNECT_AFTER = 1 << 0,
    G_CONNECT_SWAPPED = 1 << 1
};

using GCallback = void(*)(void);
using GClosureNotify = void(*)(gpointer data, GClosure *closure);

struct _GValue
{
    GType g_type;

    union
    {
        gint v_int;
        guint v_uint;
        glong v_long;
        gulong v_ulong;
        gint64 v_int64;
        guint64 v_uint64;
        gfloat v_float;
        gdouble v_double;
        gpointer v_pointer;
    } data[2];
};
#endif // __has_include(<gtk/gtk.h>)

[[maybe_unused]] inline constexpr const char GTK_THEME_NAME_ENV_VAR[] = "GTK_THEME";
[[maybe_unused]] inline constexpr const char GTK_THEME_NAME_PROP[] = "gtk-theme-name";
[[maybe_unused]] inline constexpr const char GTK_THEME_PREFER_DARK_PROP[] = "gtk-application-prefer-dark-theme";

#ifndef FRAMELESSHELPER_HAS_GTK
extern "C"
{

FRAMELESSHELPER_CORE_API void
gtk_init(
    int *argc,
    char ***argv
);

FRAMELESSHELPER_CORE_API GValue *
g_value_init(
    GValue *value,
    GType g_type
);

FRAMELESSHELPER_CORE_API GValue *
g_value_reset(
    GValue *value
);

FRAMELESSHELPER_CORE_API void
g_value_unset(
    GValue *value
);

FRAMELESSHELPER_CORE_API gboolean
g_value_get_boolean(
    const GValue *value
);

FRAMELESSHELPER_CORE_API const gchar *
g_value_get_string(
    const GValue *value
);

FRAMELESSHELPER_CORE_API GtkSettings *
gtk_settings_get_default(
    void
);

FRAMELESSHELPER_CORE_API void
g_object_get_property(
    GObject *object,
    const gchar *property_name,
    GValue *value
);

FRAMELESSHELPER_CORE_API gulong
g_signal_connect_data(
    gpointer instance,
    const gchar *detailed_signal,
    GCallback c_handler,
    gpointer data,
    GClosureNotify destroy_data,
    GConnectFlags connect_flags
);

FRAMELESSHELPER_CORE_API void
g_free(
    gpointer mem
);

FRAMELESSHELPER_CORE_API void
g_object_unref(
    GObject *object
);

FRAMELESSHELPER_CORE_API void
g_clear_object(
    GObject **object_ptr
);

} // extern "C"
#endif // FRAMELESSHELPER_HAS_GTK

FRAMELESSHELPER_BEGIN_NAMESPACE
template<typename T>
[[nodiscard]] T gtkSettings(const gchar *property);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
using x11_return_type = quint32;
#else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
using x11_return_type = unsigned long;
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
FRAMELESSHELPER_END_NAMESPACE
