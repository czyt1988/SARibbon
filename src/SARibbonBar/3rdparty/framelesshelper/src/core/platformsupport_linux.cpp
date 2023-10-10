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

#ifdef __linux__

#include "framelesshelper_linux.h"
#include "sysapiloader_p.h"

//////////////////////////////////////////////
// Xlib

#if 0 // Causes segfault
#ifndef FRAMELESSHELPER_HAS_XLIB

FRAMELESSHELPER_STRING_CONSTANT(libX11)

FRAMELESSHELPER_STRING_CONSTANT(XInitThreads)

extern "C" int
XInitThreads(
    void
)
{
    if (!API_XLIB_AVAILABLE(XInitThreads)) {
        return 0;
    }
    return API_CALL_FUNCTION(libX11, XInitThreads);
}

#endif // FRAMELESSHELPER_HAS_XLIB
#endif

//////////////////////////////////////////////
// XCB

#ifndef FRAMELESSHELPER_HAS_XCB

FRAMELESSHELPER_STRING_CONSTANT(libxcb)

FRAMELESSHELPER_STRING_CONSTANT(xcb_send_event)
FRAMELESSHELPER_STRING_CONSTANT(xcb_flush)
FRAMELESSHELPER_STRING_CONSTANT(xcb_intern_atom)
FRAMELESSHELPER_STRING_CONSTANT(xcb_intern_atom_reply)
FRAMELESSHELPER_STRING_CONSTANT(xcb_ungrab_pointer)
FRAMELESSHELPER_STRING_CONSTANT(xcb_change_property)
FRAMELESSHELPER_STRING_CONSTANT(xcb_delete_property_checked)
FRAMELESSHELPER_STRING_CONSTANT(xcb_get_property)
FRAMELESSHELPER_STRING_CONSTANT(xcb_get_property_reply)
FRAMELESSHELPER_STRING_CONSTANT(xcb_get_property_value)
FRAMELESSHELPER_STRING_CONSTANT(xcb_get_property_value_length)
FRAMELESSHELPER_STRING_CONSTANT(xcb_list_properties)
FRAMELESSHELPER_STRING_CONSTANT(xcb_list_properties_reply)
FRAMELESSHELPER_STRING_CONSTANT(xcb_list_properties_atoms_length)
FRAMELESSHELPER_STRING_CONSTANT(xcb_list_properties_atoms)
FRAMELESSHELPER_STRING_CONSTANT(xcb_get_property_unchecked)

extern "C" xcb_void_cookie_t
xcb_send_event(
    xcb_connection_t *connection,
    uint8_t propagate,
    xcb_window_t destination,
    uint32_t event_mask,
    const char *event
)
{
    if (!API_XCB_AVAILABLE(xcb_send_event)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_send_event, connection, propagate, destination, event_mask, event);
}

extern "C" int
xcb_flush(
    xcb_connection_t *connection
)
{
    if (!API_XCB_AVAILABLE(xcb_flush)) {
        return 0;
    }
    return API_CALL_FUNCTION(libxcb, xcb_flush, connection);
}

extern "C" xcb_intern_atom_cookie_t
xcb_intern_atom(
    xcb_connection_t *connection,
    uint8_t only_if_exists,
    uint16_t name_len,
    const char *name
)
{
    if (!API_XCB_AVAILABLE(xcb_intern_atom)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_intern_atom, connection, only_if_exists, name_len, name);
}

extern "C" xcb_intern_atom_reply_t *
xcb_intern_atom_reply(
    xcb_connection_t *connection,
    xcb_intern_atom_cookie_t cookie,
    xcb_generic_error_t **error
)
{
    if (!API_XCB_AVAILABLE(xcb_intern_atom_reply)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libxcb, xcb_intern_atom_reply, connection, cookie, error);
}

extern "C" xcb_void_cookie_t
xcb_ungrab_pointer(
    xcb_connection_t *connection,
    xcb_timestamp_t time
)
{
    if (!API_XCB_AVAILABLE(xcb_ungrab_pointer)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_ungrab_pointer, connection, time);
}

extern "C" xcb_void_cookie_t
xcb_change_property(
    xcb_connection_t *connection,
    uint8_t mode,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint8_t format,
    uint32_t data_len,
    const void *data
)
{
    if (!API_XCB_AVAILABLE(xcb_change_property)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_change_property, connection,
        mode, window, property, type, format, data_len, data);
}

extern "C" xcb_void_cookie_t
xcb_delete_property_checked(
    xcb_connection_t *connection,
    xcb_window_t window,
    xcb_atom_t property
)
{
    if (!API_XCB_AVAILABLE(xcb_delete_property_checked)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_delete_property_checked, connection, window, property);
}

extern "C" xcb_get_property_cookie_t
xcb_get_property(
    xcb_connection_t *connection,
    uint8_t _delete,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint32_t long_offset,
    uint32_t long_length
)
{
    if (!API_XCB_AVAILABLE(xcb_get_property)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_get_property, connection,
        _delete, window, property, type, long_offset, long_length);
}

extern "C" xcb_get_property_reply_t *
xcb_get_property_reply(
    xcb_connection_t *connection,
    xcb_get_property_cookie_t cookie,
    xcb_generic_error_t **error
)
{
    if (!API_XCB_AVAILABLE(xcb_get_property_reply)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libxcb, xcb_get_property_reply, connection, cookie, error);
}

extern "C" void *
xcb_get_property_value(
    const xcb_get_property_reply_t *reply
)
{
    if (!API_XCB_AVAILABLE(xcb_get_property_value)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libxcb, xcb_get_property_value, reply);
}

extern "C" int
xcb_get_property_value_length(
    const xcb_get_property_reply_t *reply
)
{
    if (!API_XCB_AVAILABLE(xcb_get_property_value_length)) {
        return 0;
    }
    return API_CALL_FUNCTION(libxcb, xcb_get_property_value_length, reply);
}

extern "C" xcb_list_properties_cookie_t
xcb_list_properties(
    xcb_connection_t *connection,
    xcb_window_t window
)
{
    if (!API_XCB_AVAILABLE(xcb_list_properties)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_list_properties, connection, window);
}

extern "C" xcb_list_properties_reply_t *
xcb_list_properties_reply(
    xcb_connection_t *connection,
    xcb_list_properties_cookie_t cookie,
    xcb_generic_error_t **error
)
{
    if (!API_XCB_AVAILABLE(xcb_list_properties_reply)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libxcb, xcb_list_properties_reply, connection, cookie, error);
}

extern "C" int
xcb_list_properties_atoms_length(
    const xcb_list_properties_reply_t *atom
)
{
    if (!API_XCB_AVAILABLE(xcb_list_properties_atoms_length)) {
        return 0;
    }
    return API_CALL_FUNCTION(libxcb, xcb_list_properties_atoms_length, atom);
}

extern "C" xcb_atom_t *
xcb_list_properties_atoms(
    const xcb_list_properties_reply_t *atom
)
{
    if (!API_XCB_AVAILABLE(xcb_list_properties_atoms)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libxcb, xcb_list_properties_atoms, atom);
}

extern "C" xcb_get_property_cookie_t
xcb_get_property_unchecked(
    xcb_connection_t *connection,
    uint8_t _delete,
    xcb_window_t window,
    xcb_atom_t property,
    xcb_atom_t type,
    uint32_t long_offset,
    uint32_t long_length
)
{
    if (!API_XCB_AVAILABLE(xcb_get_property_unchecked)) {
        return {};
    }
    return API_CALL_FUNCTION(libxcb, xcb_get_property_unchecked, connection,
            _delete, window, property, type, long_offset, long_length);
}

#endif // FRAMELESSHELPER_HAS_XCB

///////////////////////////////////////////////////
// GTK

#ifndef FRAMELESSHELPER_HAS_GTK

#define GTKSETTINGS_IMPL(Type, ...) \
    Type gtkSettings(const gchar *property) \
    { \
        Q_ASSERT(property); \
        Q_ASSERT(*property != '\0'); \
        if (!property || (*property == '\0')) { \
            return Type{}; \
        } \
        static GtkSettings * const settings = gtk_settings_get_default(); \
        Q_ASSERT(settings); \
        if (!settings) { \
            return Type{}; \
        } \
        GValue value = G_VALUE_INIT; \
        g_object_get_property(reinterpret_cast<GObject *>(settings), property, &value); \
        __VA_ARGS__ \
        g_value_unset(&value); \
        return result; \
    }

FRAMELESSHELPER_STRING_CONSTANT2(libgtk, "libgtk-3")

FRAMELESSHELPER_STRING_CONSTANT(gtk_init)
FRAMELESSHELPER_STRING_CONSTANT(g_value_init)
FRAMELESSHELPER_STRING_CONSTANT(g_value_reset)
FRAMELESSHELPER_STRING_CONSTANT(g_value_unset)
FRAMELESSHELPER_STRING_CONSTANT(g_value_get_boolean)
FRAMELESSHELPER_STRING_CONSTANT(g_value_get_string)
FRAMELESSHELPER_STRING_CONSTANT(gtk_settings_get_default)
FRAMELESSHELPER_STRING_CONSTANT(g_object_get_property)
FRAMELESSHELPER_STRING_CONSTANT(g_signal_connect_data)
FRAMELESSHELPER_STRING_CONSTANT(g_free)
FRAMELESSHELPER_STRING_CONSTANT(g_object_unref)
FRAMELESSHELPER_STRING_CONSTANT(g_clear_object)

extern "C" void
gtk_init(
    int *argc,
    char ***argv
)
{
    if (!API_GTK_AVAILABLE(gtk_init)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, gtk_init, argc, argv);
}

extern "C" GValue *
g_value_init(
    GValue *value,
    GType g_type
)
{
    if (!API_GTK_AVAILABLE(g_value_init)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libgtk, g_value_init, value, g_type);
}

extern "C" GValue *
g_value_reset(
    GValue *value
)
{
    if (!API_GTK_AVAILABLE(g_value_reset)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libgtk, g_value_reset, value);
}

extern "C" void
g_value_unset(
    GValue *value
)
{
    if (!API_GTK_AVAILABLE(g_value_unset)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, g_value_unset, value);
}

extern "C" gboolean
g_value_get_boolean(
    const GValue *value
)
{
    if (!API_GTK_AVAILABLE(g_value_get_boolean)) {
        return false;
    }
    return API_CALL_FUNCTION(libgtk, g_value_get_boolean, value);
}

extern "C" const gchar *
g_value_get_string(
    const GValue *value
)
{
    if (!API_GTK_AVAILABLE(g_value_get_string)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libgtk, g_value_get_string, value);
}

extern "C" GtkSettings *
gtk_settings_get_default(
    void
)
{
    if (!API_GTK_AVAILABLE(gtk_settings_get_default)) {
        return nullptr;
    }
    return API_CALL_FUNCTION(libgtk, gtk_settings_get_default);
}

extern "C" void
g_object_get_property(
    GObject *object,
    const gchar *property_name,
    GValue *value
)
{
    if (!API_GTK_AVAILABLE(g_object_get_property)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, g_object_get_property, object, property_name, value);
}

extern "C" gulong
g_signal_connect_data(
    gpointer instance,
    const gchar *detailed_signal,
    GCallback c_handler,
    gpointer data,
    GClosureNotify destroy_data,
    GConnectFlags connect_flags
)
{
    if (!API_GTK_AVAILABLE(g_signal_connect_data)) {
        return 0;
    }
    return API_CALL_FUNCTION(libgtk, g_signal_connect_data, instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}

extern "C" void
g_free(
    gpointer mem
)
{
    if (!API_GTK_AVAILABLE(g_free)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, g_free, mem);
}

extern "C" void
g_object_unref(
    GObject *object
)
{
    if (!API_GTK_AVAILABLE(g_object_unref)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, g_object_unref, object);
}

extern "C" void
g_clear_object(
    GObject **object_ptr
)
{
    if (!API_GTK_AVAILABLE(g_clear_object)) {
        return;
    }
    API_CALL_FUNCTION(libgtk, g_clear_object, object_ptr);
}

GTKSETTINGS_IMPL(bool, const bool result = g_value_get_boolean(&value);)
GTKSETTINGS_IMPL(QString, const QString result = QUtf8String(g_value_get_string(&value));)

#endif // FRAMELESSHELPER_HAS_GTK

FRAMELESSHELPER_BEGIN_NAMESPACE
template<typename T>
T gtkSettings(const gchar *property)
{
    Q_ASSERT(property);
    Q_ASSERT(*property != '\0');
    if (!property || (*property == '\0')) {
        return T{};
    }
    static GtkSettings * const settings = gtk_settings_get_default();
    if (!settings) {
        return T{};
    }
    T result = {};
    g_object_get(settings, property, &result, nullptr);
    return result;
}

template bool gtkSettings<bool>(const gchar *);

QString gtkSettings(const gchar *property)
{
    Q_ASSERT(property);
    Q_ASSERT(*property != '\0');
    if (!property || (*property == '\0')) {
        return {};
    }
    const auto raw = gtkSettings<gchararray>(property);
    const QString result = QUtf8String(raw);
    g_free(raw);
    return result;
}
FRAMELESSHELPER_END_NAMESPACE

#endif // __linux__
