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

#include "utils.h"

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))

#include "framelessconfig_p.h"
#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include <cstring> // for std::memcpy
#include <QtCore/qloggingcategory.h>
#include <QtGui/qevent.h>
#include <QtGui/qwindow.h>
#include <QtGui/qscreen.h>
#include <QtGui/qpalette.h>
#include <QtGui/qguiapplication.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  include <QtGui/qpa/qplatformnativeinterface.h>
#  include <QtGui/qpa/qplatformwindow.h>
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#    include <QtGui/qpa/qplatformscreen_p.h>
#    include <QtGui/qpa/qplatformscreen.h>
#  else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#    include <QtPlatformHeaders/qxcbscreenfunctions.h>
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#    include <QtGui/private/qtx11extras_p.h>
#    define FRAMELESSHELPER_HAS_X11EXTRAS
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#else // !FRAMELESSHELPER_CONFIG(private_qt)
#  if __has_include(<QtX11Extras/qx11info_x11.h>)
#    include <QtX11Extras/qx11info_x11.h>
#    define FRAMELESSHELPER_HAS_X11EXTRAS
#  endif // __has_include(<QtX11Extras/qx11info_x11.h>)
#endif // FRAMELESSHELPER_CONFIG(private_qt)

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcUtilsLinux, "wangwenx190.framelesshelper.core.utils.linux")
#  define INFO qCInfo(lcUtilsLinux)
#  define DEBUG qCDebug(lcUtilsLinux)
#  define WARNING qCWarning(lcUtilsLinux)
#  define CRITICAL qCCritical(lcUtilsLinux)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

FRAMELESSHELPER_STRING_CONSTANT(dark)

FRAMELESSHELPER_BYTEARRAY_CONSTANT(rootwindow)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(x11screen)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(apptime)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(appusertime)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(gettimestamp)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(startupid)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(display)
FRAMELESSHELPER_BYTEARRAY_CONSTANT(connection)

static constexpr const auto _XCB_SEND_EVENT_MASK =
    (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY);

extern template bool gtkSettings<bool>(const gchar *);
extern QString gtkSettings(const gchar *);

[[maybe_unused]] [[nodiscard]] static inline int
    qtEdgesToWmMoveOrResizeOperation(const Qt::Edges edges)
{
    if (edges == Qt::Edges{}) {
        return _NET_WM_MOVERESIZE_CANCEL;
    }
    if (edges & Qt::TopEdge) {
        if (edges & Qt::LeftEdge) {
            return _NET_WM_MOVERESIZE_SIZE_TOPLEFT;
        }
        if (edges & Qt::RightEdge) {
            return _NET_WM_MOVERESIZE_SIZE_TOPRIGHT;
        }
        return _NET_WM_MOVERESIZE_SIZE_TOP;
    }
    if (edges & Qt::BottomEdge) {
        if (edges & Qt::LeftEdge) {
            return _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT;
        }
        if (edges & Qt::RightEdge) {
            return _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT;
        }
        return _NET_WM_MOVERESIZE_SIZE_BOTTOM;
    }
    if (edges & Qt::LeftEdge) {
        return _NET_WM_MOVERESIZE_SIZE_LEFT;
    }
    if (edges & Qt::RightEdge) {
        return _NET_WM_MOVERESIZE_SIZE_RIGHT;
    }
    return _NET_WM_MOVERESIZE_CANCEL;
}

[[maybe_unused]] static inline void generateMouseReleaseEvent(QWindow *window, const QPoint &globalPos)
{
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    // https://bugreports.qt.io/browse/QTBUG-102488
    const QPoint localPos = window->mapFromGlobal(globalPos);
    const QPoint scenePos = localPos; // windowPos in Qt5.
    const auto event = new QMouseEvent(
        QEvent::MouseButtonRelease,
        localPos,
        scenePos,
        globalPos,
        Qt::LeftButton,
        QGuiApplication::mouseButtons() ^ Qt::LeftButton,
        QGuiApplication::keyboardModifiers());
    QGuiApplication::postEvent(window, event);
}

QScreen *Utils::x11_findScreenForVirtualDesktop(const int virtualDesktopNumber)
{
#if FRAMELESSHELPER_CONFIG(private_qt)
    if (virtualDesktopNumber == -1) {
        return QGuiApplication::primaryScreen();
    }
    const QList<QScreen *> screens = QGuiApplication::screens();
    if (screens.isEmpty()) {
        return nullptr;
    }
    for (auto &&screen : std::as_const(screens)) {
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        const auto qxcbScreen = dynamic_cast<QNativeInterface::Private::QXcbScreen *>(screen->handle());
        if (qxcbScreen && (qxcbScreen->virtualDesktopNumber() == virtualDesktopNumber)) {
            return screen;
        }
#  else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        if (QXcbScreenFunctions::virtualDesktopNumber(screen) == virtualDesktopNumber) {
            return screen;
        }
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    }
    return nullptr;
#else // !FRAMELESSHELPER_CONFIG(private_qt)
    Q_UNUSED(virtualDesktopNumber);
    return QGuiApplication::primaryScreen();
#endif // FRAMELESSHELPER_CONFIG(private_qt)
}

x11_return_type Utils::x11_appRootWindow(const int screen)
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::appRootWindow(screen);
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return 0;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return 0;
    }
    QScreen *scr = ((screen == -1) ? QGuiApplication::primaryScreen() : x11_findScreenForVirtualDesktop(screen));
    if (!scr) {
        return 0;
    }
    return static_cast<xcb_window_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen(krootwindow, scr)));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    Q_UNUSED(screen);
    return 0;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

int Utils::x11_appScreen()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::appScreen();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return 0;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return 0;
    }
    return reinterpret_cast<qintptr>(native->nativeResourceForIntegration(kx11screen));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return 0;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

x11_return_type Utils::x11_appTime()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::appTime();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return 0;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return 0;
    }
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return 0;
    }
    return static_cast<xcb_timestamp_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen(kapptime, screen)));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return 0;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

x11_return_type Utils::x11_appUserTime()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::appUserTime();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return 0;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return 0;
    }
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return 0;
    }
    return static_cast<xcb_timestamp_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen(kappusertime, screen)));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return 0;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

x11_return_type Utils::x11_getTimestamp()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::getTimestamp();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return 0;
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return 0;
    }
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return 0;
    }
    return static_cast<xcb_timestamp_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen(kgettimestamp, screen)));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return 0;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

QByteArray Utils::x11_nextStartupId()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::nextStartupId();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return {};
    }
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    if (!native) {
        return {};
    }
    return static_cast<char *>(native->nativeResourceForIntegration(kstartupid));
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return {};
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

Display *Utils::x11_display()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::display();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return nullptr;
    }
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    using App = QNativeInterface::QX11Application;
    const auto native = qApp->nativeInterface<App>();
#    else // (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    const auto native = qApp->platformNativeInterface();
#    endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    if (!native) {
        return nullptr;
    }
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    return native->display();
#    else // (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    return reinterpret_cast<Display *>(native->nativeResourceForIntegration(kdisplay));
#    endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return nullptr;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

xcb_connection_t *Utils::x11_connection()
{
#ifdef FRAMELESSHELPER_HAS_X11EXTRAS
    return QX11Info::connection();
#else // !FRAMELESSHELPER_HAS_X11EXTRAS
#  if FRAMELESSHELPER_CONFIG(private_qt)
    if (!qApp) {
        return nullptr;
    }
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    using App = QNativeInterface::QX11Application;
    const auto native = qApp->nativeInterface<App>();
#    else // (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    const auto native = qApp->platformNativeInterface();
#    endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    if (!native) {
        return nullptr;
    }
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    return native->connection();
#    else // (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
    return reinterpret_cast<xcb_connection_t *>(native->nativeResourceForIntegration(kconnection));
#    endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#  else // !FRAMELESSHELPER_CONFIG(private_qt)
    return nullptr;
#  endif // FRAMELESSHELPER_CONFIG(private_qt)
#endif // FRAMELESSHELPER_HAS_X11EXTRAS
}

bool Utils::startSystemMove(QWindow *window, const QPoint &globalPos)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    window->startSystemMove();
    generateMouseReleaseEvent(window, globalPos);
    return true;
#else // (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    const QPoint nativeGlobalPos = Utils::toNativeGlobalPosition(window, globalPos);
    sendMoveResizeMessage(window->winId(), _NET_WM_MOVERESIZE_MOVE, nativeGlobalPos);
    return true;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
}

bool Utils::startSystemResize(QWindow *window, const Qt::Edges edges, const QPoint &globalPos)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    if (edges == Qt::Edges{}) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    window->startSystemResize(edges);
    generateMouseReleaseEvent(window, globalPos);
    return true;
#else // (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    const QPoint nativeGlobalPos = Utils::toNativeGlobalPosition(window, globalPos);
    const int netWmOperation = qtEdgesToWmMoveOrResizeOperation(edges);
    sendMoveResizeMessage(window->winId(), netWmOperation, nativeGlobalPos);
    return true;
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
}

bool Utils::isTitleBarColorized()
{
    // ### TODO
    return false;
}

QColor Utils::getAccentColor_linux()
{
    // ### TODO
    return QGuiApplication::palette().color(QPalette::Highlight);
}

bool Utils::shouldAppsUseDarkMode_linux()
{
    /*
        https://docs.gtk.org/gtk3/running.html

        It's possible to set a theme variant after the theme name when using GTK_THEME:

            GTK_THEME=Adwaita:dark

        Some themes also have "-dark" as part of their name.

        We test this environment variable first because the documentation says
        it's mainly used for easy debugging, so it should be possible to use it
        to override any other settings.
    */
    const QString envThemeName = qEnvironmentVariable(GTK_THEME_NAME_ENV_VAR);
    if (!envThemeName.isEmpty()) {
        return envThemeName.contains(kdark, Qt::CaseInsensitive);
    }

    /*
        https://docs.gtk.org/gtk3/property.Settings.gtk-application-prefer-dark-theme.html

        This setting controls which theme is used when the theme specified by
        gtk-theme-name provides both light and dark variants. We can save a
        regex check by testing this property first.
    */
    const auto preferDark = gtkSettings<bool>(GTK_THEME_PREFER_DARK_PROP);
    if (preferDark) {
        return true;
    }

    /*
        https://docs.gtk.org/gtk3/property.Settings.gtk-theme-name.html
    */
    const auto curThemeName = gtkSettings(GTK_THEME_NAME_PROP);
    if (!curThemeName.isEmpty()) {
        return curThemeName.contains(kdark, Qt::CaseInsensitive);
    }

    return false;
}

bool Utils::setBlurBehindWindowEnabled(const WId windowId, const BlurMode mode, const QColor &color)
{
    Q_UNUSED(color);
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    static const xcb_atom_t atom = internAtom(ATOM_KDE_NET_WM_BLUR_BEHIND_REGION);
    if ((atom == XCB_NONE) || !isSupportedByRootWindow(atom)) {
        WARNING << "Current window manager doesn't support blur behind window.";
        return false;
    }
    static const xcb_atom_t deepinAtom = internAtom(ATOM_NET_WM_DEEPIN_BLUR_REGION_MASK);
    if ((deepinAtom != XCB_NONE) && isSupportedByWindowManager(deepinAtom)) {
        clearWindowProperty(windowId, deepinAtom);
    }
    const auto blurMode = [mode]() -> BlurMode {
        if ((mode == BlurMode::Disable) || (mode == BlurMode::Default)) {
            return mode;
        }
        WARNING << "The BlurMode::Windows_* enum values are not supported on Linux.";
        return BlurMode::Default;
    }();
    if (blurMode == BlurMode::Disable) {
        clearWindowProperty(windowId, atom);
    } else {
        const quint32 value = true;
        setWindowProperty(windowId, atom, XCB_ATOM_CARDINAL, &value, 1, sizeof(quint32) * 8);
    }
    return true;
}

QString Utils::getWallpaperFilePath()
{
#if 0
    GConfClient * const client = gconf_client_get_default();
    Q_ASSERT(client);
    if (!client) {
        return {};
    }
    const gchararray rawPath = gconf_client_get_string(client, "/desktop/gnome/background/picture_filename", nullptr);
    if (!rawPath || (*rawPath == '\0')) {
        WARNING << "Failed to retrieve the wallpaper file path.";
        return {};
    }
    return QUtf8String(rawPath);
#else
    // ### TODO
    return {};
#endif
}

WallpaperAspectStyle Utils::getWallpaperAspectStyle()
{
#if 0
    static constexpr const auto defaultAspectStyle = WallpaperAspectStyle::Fill;
    GConfClient * const client = gconf_client_get_default();
    Q_ASSERT(client);
    if (!client) {
        return defaultAspectStyle;
    }
    const gchararray rawOptions = gconf_client_get_string(client, "/desktop/gnome/background/picture_options", nullptr);
    if (!rawOptions || (*rawOptions == '\0')) {
        WARNING << "Failed to retrieve the wallpaper tile options.";
        return defaultAspectStyle;
    }
    const QString options = QUtf8String(rawOptions);
    if (options.isEmpty()) {
        return defaultAspectStyle;
    } else {
        if ((options == "wallpaper"_L1) || (options == "tiled"_L1)) {
            return WallpaperAspectStyle::Tile;
        } else if (options == "centered"_L1) {
            return WallpaperAspectStyle::Center;
        } else if (options == "stretched"_L1) {
            return WallpaperAspectStyle::Stretch;
        } else if (options == "scaled"_L1) {
            return WallpaperAspectStyle::Fit;
        } else {
            return defaultAspectStyle;
        }
    }
#else
    // ### TODO
    return WallpaperAspectStyle::Fill;
#endif
}

bool Utils::isBlurBehindWindowSupported()
{
    static const auto result = []() -> bool {
        if (FramelessConfig::instance()->isSet(Option::ForceNativeBackgroundBlur)) {
            return true;
        }
        if (FramelessConfig::instance()->isSet(Option::ForceNonNativeBackgroundBlur)) {
            return false;
        }
#if 0 // FIXME: The window will become totally black if we enable blur behind window on KWin.
        static const QString windowManager = getWindowManagerName();
        static const bool isDeepinV15 = (windowManager == FRAMELESSHELPER_STRING_LITERAL("Mutter(DeepinGala)"));
        if (isDeepinV15) {
            static const xcb_atom_t atom = internAtom(ATOM_NET_WM_DEEPIN_BLUR_REGION_ROUNDED);
            return ((atom != XCB_NONE) && isSupportedByWindowManager(atom));
        }
        static const bool isKWin = (windowManager == FRAMELESSHELPER_STRING_LITERAL("KWin"));
        if (isKWin) {
            static const xcb_atom_t atom = internAtom(ATOM_KDE_NET_WM_BLUR_BEHIND_REGION);
            return ((atom != XCB_NONE) && isSupportedByRootWindow(atom));
        }
#endif
        return false;
    }();
    return result;
}

static inline void themeChangeNotificationCallback()
{
    // Sometimes the FramelessManager instance may be destroyed already.
    if (FramelessManager * const manager = FramelessManager::instance()) {
        if (FramelessManagerPrivate * const managerPriv = FramelessManagerPrivate::get(manager)) {
            managerPriv->notifySystemThemeHasChangedOrNot();
        }
    }
}

bool Utils::registerThemeChangeNotification()
{
    GtkSettings * const settings = gtk_settings_get_default();
    Q_ASSERT(settings);
    if (!settings) {
        return false;
    }
    g_signal_connect(settings, "notify::gtk-application-prefer-dark-theme", themeChangeNotificationCallback, nullptr);
    g_signal_connect(settings, "notify::gtk-theme-name", themeChangeNotificationCallback, nullptr);
    return true;
}

QColor Utils::getFrameBorderColor(const bool active)
{
    return (active ? getAccentColor() : kDefaultDarkGrayColor);
}

xcb_atom_t Utils::internAtom(const char *name)
{
    Q_ASSERT(name);
    Q_ASSERT(*name != '\0');
    if (!name || (*name == '\0')) {
        return XCB_NONE;
    }
    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return XCB_NONE;
    }
    const xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, false, qstrlen(name), name);
    xcb_intern_atom_reply_t * const reply = xcb_intern_atom_reply(connection, cookie, nullptr);
    if (!reply) {
        return XCB_NONE;
    }
    const xcb_atom_t atom = reply->atom;
    std::free(reply);
    return atom;
}

QString Utils::getWindowManagerName()
{
    static const auto result = []() -> QString {
        xcb_connection_t * const connection = x11_connection();
        Q_ASSERT(connection);
        if (!connection) {
            return {};
        }
        const quint32 rootWindow = x11_appRootWindow(x11_appScreen());
        Q_ASSERT(rootWindow);
        if (!rootWindow) {
            return {};
        }
        static const xcb_atom_t wmCheckAtom = internAtom(ATOM_NET_SUPPORTING_WM_CHECK);
        if (wmCheckAtom == XCB_NONE) {
            WARNING << "Failed to retrieve the atom of _NET_SUPPORTING_WM_CHECK.";
            return {};
        }
        const xcb_get_property_cookie_t cookie = xcb_get_property_unchecked(connection, false, rootWindow, wmCheckAtom, XCB_ATOM_WINDOW, 0, 1024);
        xcb_get_property_reply_t * const reply = xcb_get_property_reply(connection, cookie, nullptr);
        if (!reply) {
            return {};
        }
        if (!((reply->format == 32) && (reply->type == XCB_ATOM_WINDOW))) {
            std::free(reply);
            return {};
        }
        const auto windowManager = *static_cast<xcb_window_t *>(xcb_get_property_value(reply));
        if (windowManager == XCB_WINDOW_NONE) {
            std::free(reply);
            return {};
        }
        static const xcb_atom_t wmNameAtom = internAtom(ATOM_NET_WM_NAME);
        if (wmNameAtom == XCB_NONE) {
            WARNING << "Failed to retrieve the atom of _NET_WM_NAME.";
            return {};
        }
        static const xcb_atom_t strAtom = internAtom(ATOM_UTF8_STRING);
        if (strAtom == XCB_NONE) {
            WARNING << "Failed to retrieve the atom of UTF8_STRING.";
            return {};
        }
        const xcb_get_property_cookie_t wmCookie = xcb_get_property_unchecked(connection, false, windowManager, wmNameAtom, strAtom, 0, 1024);
        xcb_get_property_reply_t * const wmReply = xcb_get_property_reply(connection, wmCookie, nullptr);
        if (!wmReply) {
            std::free(reply);
            return {};
        }
        if (!((wmReply->format == 8) && (wmReply->type == strAtom))) {
            std::free(wmReply);
            std::free(reply);
            return {};
        }
        const auto data = static_cast<const char *>(xcb_get_property_value(wmReply));
        const int len = xcb_get_property_value_length(wmReply);
        const QString wmName = QString::fromUtf8(data, len);
        std::free(wmReply);
        std::free(reply);
        return wmName;
    }();
    return result;
}

void Utils::openSystemMenu(const WId windowId, const QPoint &globalPos)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }

    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return;
    }

    const quint32 rootWindow = x11_appRootWindow(x11_appScreen());
    Q_ASSERT(rootWindow);
    if (!rootWindow) {
        return;
    }

    static const xcb_atom_t atom = internAtom(ATOM_GTK_SHOW_WINDOW_MENU);
    if ((atom == XCB_NONE) || !isSupportedByWindowManager(atom)) {
        WARNING << "Current window manager doesn't support showing window menu.";
        return;
    }

    xcb_client_message_event_t xev;
    memset(&xev, 0, sizeof(xev));
    xev.response_type = XCB_CLIENT_MESSAGE;
    xev.type = atom;
    xev.window = windowId;
    xev.format = 32;
    xev.data.data32[1] = globalPos.x();
    xev.data.data32[2] = globalPos.y();

    xcb_ungrab_pointer(connection, XCB_CURRENT_TIME);
    xcb_send_event(connection, false, rootWindow, _XCB_SEND_EVENT_MASK, reinterpret_cast<const char *>(&xev));
    xcb_flush(connection);
}

QByteArray Utils::getWindowProperty(const WId windowId, const xcb_atom_t prop, const xcb_atom_t type, const quint32 data_len)
{
    Q_ASSERT(windowId);
    Q_ASSERT(prop != XCB_NONE);
    Q_ASSERT(type != XCB_NONE);
    if (!windowId || (prop == XCB_NONE) || (type == XCB_NONE)) {
        return {};
    }
    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return {};
    }
    const xcb_get_property_cookie_t cookie = xcb_get_property(connection, false, windowId, prop, type, 0, data_len);
    xcb_get_property_reply_t * const reply = xcb_get_property_reply(connection, cookie, nullptr);
    if (!reply) {
        return {};
    }
    QByteArray data = {};
    const int len = xcb_get_property_value_length(reply);
    const auto buf = static_cast<const char *>(xcb_get_property_value(reply));
    data.append(buf, len);
    std::free(reply);
    return data;
}

void Utils::setWindowProperty(const WId windowId, const xcb_atom_t prop, const xcb_atom_t type, const void *data, const quint32 data_len, const uint8_t format)
{
    Q_ASSERT(windowId);
    Q_ASSERT(prop != XCB_NONE);
    Q_ASSERT(type != XCB_NONE);
    if (!windowId || (prop == XCB_NONE) || (type == XCB_NONE)) {
        return;
    }
    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return;
    }
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, windowId, prop, type, format, data_len, data);
    xcb_flush(connection);
}

void Utils::clearWindowProperty(const WId windowId, const xcb_atom_t prop)
{
    Q_ASSERT(windowId);
    Q_ASSERT(prop != XCB_NONE);
    if (!windowId || (prop == XCB_NONE)) {
        return;
    }
    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return;
    }
    xcb_delete_property_checked(connection, windowId, prop);
}

bool Utils::isSupportedByWindowManager(const xcb_atom_t atom)
{
    Q_ASSERT(atom != XCB_NONE);
    if (atom == XCB_NONE) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    using result_type = QList<xcb_atom_t>;
#else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    using result_type = QVector<xcb_atom_t>;
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    static const auto netWmAtoms = []() -> result_type {
        xcb_connection_t * const connection = x11_connection();
        Q_ASSERT(connection);
        if (!connection) {
            return {};
        }
        const quint32 rootWindow = x11_appRootWindow(x11_appScreen());
        Q_ASSERT(rootWindow);
        if (!rootWindow) {
            return {};
        }
        static const xcb_atom_t netSupportedAtom = internAtom(ATOM_NET_SUPPORTED);
        if (netSupportedAtom == XCB_NONE) {
            WARNING << "Failed to retrieve the atom of _NET_SUPPORTED.";
            return {};
        }
        result_type result = {};
        int offset = 0;
        int remaining = 0;
        do {
            const xcb_get_property_cookie_t cookie = xcb_get_property(connection, false, rootWindow, netSupportedAtom, XCB_ATOM_ATOM, offset, 1024);
            xcb_get_property_reply_t * const reply = xcb_get_property_reply(connection, cookie, nullptr);
            if (!reply) {
                break;
            }
            remaining = 0;
            if ((reply->type == XCB_ATOM_ATOM) && (reply->format == 32)) {
                const int len = (xcb_get_property_value_length(reply) / sizeof(xcb_atom_t));
                const auto atoms = static_cast<xcb_atom_t *>(xcb_get_property_value(reply));
                const int size = result.size();
                result.resize(size + len);
                std::memcpy(result.data() + size, atoms, len * sizeof(xcb_atom_t));
                remaining = reply->bytes_after;
                offset += len;
            }
            std::free(reply);
        } while (remaining > 0);
        return result;
    }();
    return netWmAtoms.contains(atom);
}

bool Utils::isSupportedByRootWindow(const xcb_atom_t atom)
{
    Q_ASSERT(atom != XCB_NONE);
    if (atom == XCB_NONE) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    using result_type = QList<xcb_atom_t>;
#else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    using result_type = QVector<xcb_atom_t>;
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    static const auto rootWindowProperties = []() -> result_type {
        xcb_connection_t * const connection = x11_connection();
        Q_ASSERT(connection);
        if (!connection) {
            return {};
        }
        const quint32 rootWindow = x11_appRootWindow(x11_appScreen());
        Q_ASSERT(rootWindow);
        if (!rootWindow) {
            return {};
        }
        result_type result = {};
        const xcb_list_properties_cookie_t cookie = xcb_list_properties(connection, rootWindow);
        xcb_list_properties_reply_t * const reply = xcb_list_properties_reply(connection, cookie, nullptr);
        if (!reply) {
            return {};
        }
        const int len = xcb_list_properties_atoms_length(reply);
        const auto atoms = static_cast<xcb_atom_t *>(xcb_list_properties_atoms(reply));
        result.resize(len);
        std::memcpy(result.data(), atoms, len * sizeof(xcb_atom_t));
        std::free(reply);
        return result;
    }();
    return rootWindowProperties.contains(atom);
}

bool Utils::tryHideSystemTitleBar(const WId windowId, const bool hide)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    static const xcb_atom_t deepinNoTitleBarAtom = internAtom(ATOM_DEEPIN_NO_TITLEBAR);
    if ((deepinNoTitleBarAtom == XCB_NONE) || !isSupportedByWindowManager(deepinNoTitleBarAtom)) {
        WARNING << "Current window manager doesn't support hiding title bar natively.";
        return false;
    }
    const quint32 value = hide;
    setWindowProperty(windowId, deepinNoTitleBarAtom, XCB_ATOM_CARDINAL, &value, 1, sizeof(quint32) * 8);
    static const xcb_atom_t deepinForceDecorateAtom = internAtom(ATOM_DEEPIN_FORCE_DECORATE);
    if ((deepinForceDecorateAtom == XCB_NONE) || !isSupportedByWindowManager(deepinForceDecorateAtom)) {
        return true;
    }
    if (hide) {
        setWindowProperty(windowId, deepinForceDecorateAtom, XCB_ATOM_CARDINAL, &value, 1, sizeof(quint32) * 8);
    } else {
        clearWindowProperty(windowId, deepinForceDecorateAtom);
    }
    return true;
}

void Utils::sendMoveResizeMessage(const WId windowId, const uint32_t action, const QPoint &globalPos, const Qt::MouseButton button)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }

    xcb_connection_t * const connection = x11_connection();
    Q_ASSERT(connection);
    if (!connection) {
        return;
    }
    const quint32 rootWindow = x11_appRootWindow(x11_appScreen());
    Q_ASSERT(rootWindow);
    if (!rootWindow) {
        return;
    }

    static const xcb_atom_t atom = internAtom(ATOM_NET_WM_MOVERESIZE);
    if ((atom == XCB_NONE) || !isSupportedByWindowManager(atom)) {
        WARNING << "Current window manager doesn't support move resize operation.";
        return;
    }

    xcb_client_message_event_t xev;
    memset(&xev, 0, sizeof(xev));
    xev.response_type = XCB_CLIENT_MESSAGE;
    xev.type = atom;
    xev.window = windowId;
    xev.format = 32;
    xev.data.data32[0] = globalPos.x();
    xev.data.data32[1] = globalPos.y();
    xev.data.data32[2] = action;
    xev.data.data32[3] = [button]() -> int {
        if (button == Qt::LeftButton) {
            return XCB_BUTTON_INDEX_1;
        }
        if (button == Qt::RightButton) {
            return XCB_BUTTON_INDEX_3;
        }
        return XCB_BUTTON_INDEX_ANY;
    }();
    xev.data.data32[4] = 0;

    if (action != _NET_WM_MOVERESIZE_CANCEL) {
        xcb_ungrab_pointer(connection, XCB_CURRENT_TIME);
    }
    xcb_send_event(connection, false, rootWindow, _XCB_SEND_EVENT_MASK, reinterpret_cast<const char *>(&xev));
    xcb_flush(connection);
}

bool Utils::isCustomDecorationSupported()
{
    static const xcb_atom_t atom = internAtom(ATOM_DEEPIN_NO_TITLEBAR);
    return ((atom != XCB_NONE) && isSupportedByWindowManager(atom));
}

bool Utils::setPlatformPropertiesForWindow(QWindow *window, const QVariantHash &props)
{
    Q_ASSERT(window);
    Q_ASSERT(!props.isEmpty());
    if (!window || props.isEmpty()) {
        return false;
    }
    static const auto object = [window]() -> QObject * {
        if (!qGuiApp) {
            return nullptr;
        }
        using buildNativeSettingsPtr = bool(*)(QObject *, WId);
        static const auto pbuildNativeSettings
            = reinterpret_cast<buildNativeSettingsPtr>(
                QGuiApplication::platformFunction(
                    FRAMELESSHELPER_BYTEARRAY_LITERAL("_d_buildNativeSettings")));
        if (!pbuildNativeSettings) {
            return nullptr;
        }
        const auto obj = new QObject(window);
        if (!pbuildNativeSettings(obj, window->winId())) {
            delete obj;
            return nullptr;
        }
        return obj;
    }();
    if (!object) {
        return false;
    }
    auto it = props.constBegin();
    while (it != props.constEnd()) {
        object->setProperty(qUtf8Printable(it.key()), it.value());
        ++it;
    }
    return true;
}

FRAMELESSHELPER_END_NAMESPACE

#endif // Q_OS_LINUX
