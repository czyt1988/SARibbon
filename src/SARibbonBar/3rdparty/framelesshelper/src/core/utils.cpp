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
#include "framelesshelpercore_global_p.h"
#include "framelessmanager_p.h"
#ifdef Q_OS_WINDOWS
#  include "winverhelper_p.h"
#endif // Q_OS_WINDOWS
#include <array>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qwindow.h>
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qpalette.h>
#include <QtGui/qsurface.h>
#include <QtGui/qsurfaceformat.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  include <QtGui/private/qhighdpiscaling_p.h>
#  include <QtGui/private/qwindow_p.h>
#endif // FRAMELESSHELPER_CONFIG(private_qt)
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
#  include <QtGui/qstylehints.h>
#elif ((QT_VERSION >= QT_VERSION_CHECK(6, 2, 1)) && FRAMELESSHELPER_CONFIG(private_qt))
#  include <QtGui/qpa/qplatformtheme.h>
#  include <QtGui/private/qguiapplication_p.h>
#endif // ((QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)) && FRAMELESSHELPER_CONFIG(private_qt))

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcUtilsCommon, "wangwenx190.framelesshelper.core.utils.common")
#  define INFO qCInfo(lcUtilsCommon)
#  define DEBUG qCDebug(lcUtilsCommon)
#  define WARNING qCWarning(lcUtilsCommon)
#  define CRITICAL qCCritical(lcUtilsCommon)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

#if FRAMELESSHELPER_CONFIG(bundle_resource)
struct FONT_ICON
{
    quint32 SegoeUI = 0;
    quint32 Fallback = 0;
};

static constexpr const std::array<FONT_ICON, static_cast<int>(SystemButtonType::Last) + 1> g_fontIconsTable =
{
    FONT_ICON{ 0x0000, 0x0000 },
    FONT_ICON{ 0xE756, 0x0000 },
    FONT_ICON{ 0xE897, 0x0000 },
    FONT_ICON{ 0xE921, 0xE93E },
    FONT_ICON{ 0xE922, 0xE93C },
    FONT_ICON{ 0xE923, 0xE93D },
    FONT_ICON{ 0xE8BB, 0xE93B }
};
#endif // FRAMELESSHELPER_CONFIG(bundle_resource)

#if !FRAMELESSHELPER_CONFIG(private_qt)
[[nodiscard]] static inline QPoint getScaleOrigin(const QWindow *window)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
    QScreen *screen = window->screen();
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    if (!screen) {
        return {};
    }
    return screen->geometry().topLeft();
}
#endif // !FRAMELESSHELPER_CONFIG(private_qt)

Qt::CursorShape Utils::calculateCursorShape(const QWindow *window, const QPoint &pos)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(window);
    Q_UNUSED(pos);
    return Qt::ArrowCursor;
#else
    Q_ASSERT(window);
    if (!window) {
        return Qt::ArrowCursor;
    }
    if (window->visibility() != QWindow::Windowed) {
        return Qt::ArrowCursor;
    }
    const int x = pos.x();
    const int y = pos.y();
    const int w = window->width();
    const int h = window->height();
    if (((x < kDefaultResizeBorderThickness) && (y < kDefaultResizeBorderThickness))
        || ((x >= (w - kDefaultResizeBorderThickness)) && (y >= (h - kDefaultResizeBorderThickness)))) {
        return Qt::SizeFDiagCursor;
    }
    if (((x >= (w - kDefaultResizeBorderThickness)) && (y < kDefaultResizeBorderThickness))
        || ((x < kDefaultResizeBorderThickness) && (y >= (h - kDefaultResizeBorderThickness)))) {
        return Qt::SizeBDiagCursor;
    }
    if ((x < kDefaultResizeBorderThickness) || (x >= (w - kDefaultResizeBorderThickness))) {
        return Qt::SizeHorCursor;
    }
    if ((y < kDefaultResizeBorderThickness) || (y >= (h - kDefaultResizeBorderThickness))) {
        return Qt::SizeVerCursor;
    }
    return Qt::ArrowCursor;
#endif
}

Qt::Edges Utils::calculateWindowEdges(const QWindow *window, const QPoint &pos)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(window);
    Q_UNUSED(pos);
    return {};
#else
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
    if (window->visibility() != QWindow::Windowed) {
        return {};
    }
    Qt::Edges edges = {};
    const int x = pos.x();
    const int y = pos.y();
    if (x < kDefaultResizeBorderThickness) {
        edges |= Qt::LeftEdge;
    }
    if (x >= (window->width() - kDefaultResizeBorderThickness)) {
        edges |= Qt::RightEdge;
    }
    if (y < kDefaultResizeBorderThickness) {
        edges |= Qt::TopEdge;
    }
    if (y >= (window->height() - kDefaultResizeBorderThickness)) {
        edges |= Qt::BottomEdge;
    }
    return edges;
#endif
}

QString Utils::getSystemButtonGlyph(const SystemButtonType button)
{
#if FRAMELESSHELPER_CONFIG(bundle_resource)
    const FONT_ICON &icon = g_fontIconsTable.at(static_cast<int>(button));
#  ifdef Q_OS_WINDOWS
    // Windows 11: Segoe Fluent Icons (https://docs.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font)
    // Windows 10: Segoe MDL2 Assets (https://docs.microsoft.com/en-us/windows/apps/design/style/segoe-ui-symbol-font)
    // Windows 7~8.1: Our own custom icon
    if (WindowsVersionHelper::isWin10OrGreater()) {
        return QChar(icon.SegoeUI);
    }
#  endif // Q_OS_WINDOWS
    // We always use our own icons on UNIX platforms because Microsoft doesn't allow distributing
    // the Segoe icon font to other platforms than Windows.
    return QChar(icon.Fallback);
#else // !FRAMELESSHELPER_CONFIG(bundle_resource)
    return {};
#endif // FRAMELESSHELPER_CONFIG(bundle_resource)
}

QWindow *Utils::findWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return nullptr;
    }
    const QWindowList windows = QGuiApplication::topLevelWindows();
    if (windows.isEmpty()) {
        return nullptr;
    }
    for (auto &&window : std::as_const(windows)) {
        if (window && window->handle()) {
            if (window->winId() == windowId) {
                return window;
            }
        }
    }
    return nullptr;
}

bool Utils::moveWindowToDesktopCenter(const WId windowId, const bool considerTaskBar)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data || !data->callbacks) {
        return false;
    }
    const QSize windowSize = data->callbacks->getWindowSize();
    if (windowSize.isEmpty() || (windowSize <= kDefaultWindowSize)) {
        return false;
    }
    const QScreen *screen = data->callbacks->getWindowScreen();
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    Q_ASSERT(screen);
    if (!screen) {
        return false;
    }
    const QSize screenSize = (considerTaskBar ? screen->availableSize() : screen->size());
    const QPoint offset = (considerTaskBar ? screen->availableGeometry().topLeft() : QPoint(0, 0));
    const int newX = std::round(qreal(screenSize.width() - windowSize.width()) / qreal(2));
    const int newY = std::round(qreal(screenSize.height() - windowSize.height()) / qreal(2));
    data->callbacks->setWindowPosition(QPoint(newX + offset.x(), newY + offset.y()));
    return true;
}

Qt::WindowState Utils::windowStatesToWindowState(const Qt::WindowStates states)
{
    if (states & Qt::WindowFullScreen) {
        return Qt::WindowFullScreen;
    }
    if (states & Qt::WindowMaximized) {
        return Qt::WindowMaximized;
    }
    if (states & Qt::WindowMinimized) {
        return Qt::WindowMinimized;
    }
    return Qt::WindowNoState;
}

bool Utils::isThemeChangeEvent(const QEvent * const event)
{
    Q_ASSERT(event);
    if (!event) {
        return false;
    }
    // QGuiApplication will only deliver theme change events to top level Q(Quick)Windows,
    // QWidgets won't get such notifications, no matter whether it's top level widget or not.
    // QEvent::ThemeChange: Send by the Windows QPA.
    // QEvent::ApplicationPaletteChange: All other platforms (Linux & macOS).
    const QEvent::Type type = event->type();
    return ((type == QEvent::ThemeChange) || (type == QEvent::ApplicationPaletteChange));
}

QColor Utils::calculateSystemButtonBackgroundColor(const SystemButtonType button, const ButtonState state)
{
    if (state == ButtonState::Normal) {
        return kDefaultTransparentColor;
    }
    const bool isClose = (button == SystemButtonType::Close);
    const bool isTitleColor = isTitleBarColorized();
    const bool isHovered = (state == ButtonState::Hovered);
    const auto result = [isClose, isTitleColor]() -> QColor {
        if (isClose) {
            return kDefaultSystemCloseButtonBackgroundColor;
        }
        if (isTitleColor) {
            return getAccentColor();
        }
        return kDefaultSystemButtonBackgroundColor;
    }();
    if (isClose) {
        return (isHovered ? result.lighter(110) : result.lighter(140));
    }
    if (!isTitleColor) {
        return (isHovered ? result.lighter(110) : result);
    }
    return (isHovered ? result.lighter(150) : result.lighter(120));
}

bool Utils::shouldAppsUseDarkMode()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    return (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
#elif ((QT_VERSION >= QT_VERSION_CHECK(6, 2, 1)) && FRAMELESSHELPER_CONFIG(private_qt))
    if (const QPlatformTheme * const theme = QGuiApplicationPrivate::platformTheme()) {
        return (theme->appearance() == QPlatformTheme::Appearance::Dark);
    }
    return false;
#else // ((QT_VERSION < QT_VERSION_CHECK(6, 2, 1)) || !FRAMELESSHELPER_CONFIG(private_qt))
#  ifdef Q_OS_WINDOWS
    return shouldAppsUseDarkMode_windows();
#  elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    return shouldAppsUseDarkMode_linux();
#  elif defined(Q_OS_MACOS)
    return shouldAppsUseDarkMode_macos();
#  else
    return false;
#  endif
#endif
}

qreal Utils::roundScaleFactor(const qreal factor)
{
    // Qt can't handle scale factors less than 1.0 (according to the comments in qhighdpiscaling.cpp).
    Q_ASSERT((factor > qreal(1)) || qFuzzyCompare(factor, qreal(1)));
    if (factor < qreal(1)) {
        return qreal(1);
    }
#if (!FRAMELESSHELPER_CONFIG(private_qt) || (QT_VERSION < QT_VERSION_CHECK(6, 2, 1)))
#  if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    static const auto policy = QGuiApplication::highDpiScaleFactorRoundingPolicy();
    switch (policy) {
    case Qt::HighDpiScaleFactorRoundingPolicy::Round:
        return std::round(factor);
    case Qt::HighDpiScaleFactorRoundingPolicy::Ceil:
        return std::ceil(factor);
    case Qt::HighDpiScaleFactorRoundingPolicy::Floor:
        return std::floor(factor);
    case Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor:
    {
        static constexpr const auto flag = qreal(0.75);
        const qreal gap = (factor - qreal(int(factor)));
        return (((gap > flag) || qFuzzyCompare(gap, flag)) ? std::round(factor) : std::floor(factor));
    }
    case Qt::HighDpiScaleFactorRoundingPolicy::PassThrough:
    case Qt::HighDpiScaleFactorRoundingPolicy::Unset: // According to Qt source code, this enum value is the same with PassThrough.
        return factor;
    }
    return 1;
#  else // (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    return std::round(factor);
#  endif // (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#else // (FRAMELESSHELPER_CONFIG(private_qt) && (QT_VERSION >= QT_VERSION_CHECK(6, 2, 1)))
    return QHighDpiScaling::roundScaleFactor(factor);
#endif
}

int Utils::toNativePixels(const QWindow *window, const int value)
{
    Q_ASSERT(window);
    if (!window) {
        return 0;
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::toNativePixels(value, window);
#else
    return std::round(qreal(value) * window->devicePixelRatio());
#endif
}

QPoint Utils::toNativePixels(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::toNativePixels(point, window);
#else
    const QPoint origin = getScaleOrigin(window);
    return QPointF(QPointF(point - origin) * window->devicePixelRatio()).toPoint() + origin;
#endif
}

QSize Utils::toNativePixels(const QWindow *window, const QSize &size)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::toNativePixels(size, window);
#else
    return QSizeF(QSizeF(size) * window->devicePixelRatio()).toSize();
#endif
}

QRect Utils::toNativePixels(const QWindow *window, const QRect &rect)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::toNativePixels(rect, window);
#else
    return QRect(toNativePixels(window, rect.topLeft()), toNativePixels(window, rect.size()));
#endif
}

int Utils::fromNativePixels(const QWindow *window, const int value)
{
    Q_ASSERT(window);
    if (!window) {
        return 0;
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::fromNativePixels(value, window);
#else
    return std::round(qreal(value) / window->devicePixelRatio());
#endif
}

QPoint Utils::fromNativePixels(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::fromNativePixels(point, window);
#else
    const QPoint origin = getScaleOrigin(window);
    return QPointF(QPointF(point - origin) / window->devicePixelRatio()).toPoint() + origin;
#endif
}

QSize Utils::fromNativePixels(const QWindow *window, const QSize &size)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::fromNativePixels(size, window);
#else
    return QSizeF(QSizeF(size) / window->devicePixelRatio()).toSize();
#endif
}

QRect Utils::fromNativePixels(const QWindow *window, const QRect &rect)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::fromNativePixels(rect, window);
#else
    return QRect(fromNativePixels(window, rect.topLeft()), fromNativePixels(window, rect.size()));
#endif
}

QPoint Utils::toNativeLocalPosition(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::toNativeLocalPosition(point, window);
#else
    return QPointF(QPointF(point) * window->devicePixelRatio()).toPoint();
#endif
}

QPoint Utils::toNativeGlobalPosition(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if (!FRAMELESSHELPER_CONFIG(private_qt) || (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    return toNativePixels(window, point);
#else
    return QHighDpi::toNativeGlobalPosition(point, window);
#endif
}

QPoint Utils::fromNativeLocalPosition(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    return QHighDpi::fromNativeLocalPosition(point, window);
#else
    return QPointF(QPointF(point) / window->devicePixelRatio()).toPoint();
#endif
}

QPoint Utils::fromNativeGlobalPosition(const QWindow *window, const QPoint &point)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if (!FRAMELESSHELPER_CONFIG(private_qt) || (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    return fromNativePixels(window, point);
#else
    return QHighDpi::fromNativeGlobalPosition(point, window);
#endif
}

int Utils::horizontalAdvance(const QFontMetrics &fm, const QString &str)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    return fm.horizontalAdvance(str);
#else // (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
    return fm.width();
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
}

qreal Utils::getRelativeScaleFactor(const quint32 oldDpi, const quint32 newDpi)
{
    if (newDpi == oldDpi) {
        return qreal(1);
    }
    static const quint32 defaultDpi = defaultScreenDpi();
    if ((oldDpi < defaultDpi) || (newDpi < defaultDpi)) {
        return qreal(1);
    }
    // We need to round the scale factor according to Qt's rounding policy.
    const qreal oldDpr = roundScaleFactor(qreal(oldDpi) / qreal(defaultDpi));
    const qreal newDpr = roundScaleFactor(qreal(newDpi) / qreal(defaultDpi));
    return qreal(newDpr / oldDpr);
}

QSizeF Utils::rescaleSize(const QSizeF &oldSize, const quint32 oldDpi, const quint32 newDpi)
{
    if (oldSize.isEmpty()) {
        return {};
    }
    if (newDpi == oldDpi) {
        return oldSize;
    }
    const qreal scaleFactor = getRelativeScaleFactor(oldDpi, newDpi);
    if (qFuzzyIsNull(scaleFactor)) {
        return {};
    }
    if (qFuzzyCompare(scaleFactor, qreal(1))) {
        return oldSize;
    }
    return QSizeF(oldSize * scaleFactor);
}

QSize Utils::rescaleSize(const QSize &oldSize, const quint32 oldDpi, const quint32 newDpi)
{
    return rescaleSize(QSizeF(oldSize), oldDpi, newDpi).toSize();
}

bool Utils::isValidGeometry(const QRectF &rect)
{
    // The position of the rectangle is not relevant.
    return ((rect.right() > rect.left()) && (rect.bottom() > rect.top()));
}

bool Utils::isValidGeometry(const QRect &rect)
{
    return isValidGeometry(QRectF(rect));
}

quint32 Utils::defaultScreenDpi()
{
#ifdef Q_OS_MACOS
    return 72;
#else // !Q_OS_MACOS
    return 96;
#endif // Q_OS_MACOS
}

QColor Utils::getAccentColor()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    return QGuiApplication::palette().color(QPalette::Accent);
#else // (QT_VERSION < QT_VERSION_CHECK(6, 6, 0))
#  ifdef Q_OS_WINDOWS
    return getAccentColor_windows();
#  elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    return getAccentColor_linux();
#  elif defined(Q_OS_MACOS)
    return getAccentColor_macos();
#  else
    return QGuiApplication::palette().color(QPalette::Highlight);
#  endif
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
}

bool Utils::isWindowAccelerated(const QWindow *window)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    switch (window->surfaceType()) {
    case QSurface::RasterGLSurface:
#if FRAMELESSHELPER_CONFIG(private_qt)
        return qt_window_private(const_cast<QWindow *>(window))->compositing;
#else
        return true;
#endif
    case QSurface::OpenGLSurface:
    case QSurface::VulkanSurface:
    case QSurface::MetalSurface:
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
    case QSurface::Direct3DSurface:
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
        return true;
    default:
        break;
    }
    return false;
}

bool Utils::isWindowTransparent(const QWindow *window)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    // On most platforms, QWindow::format() will just return the
    // user set format if there is one, otherwise it will return
    // an invalid surface format. That means, most of the time
    // the following check will not be useful. But since this is
    // what the QPA code does, we just mirror it here.
    return window->format().hasAlpha();
}

FRAMELESSHELPER_END_NAMESPACE
