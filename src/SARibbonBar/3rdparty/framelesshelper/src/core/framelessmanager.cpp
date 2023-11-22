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

#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include "framelesshelpercore_global_p.h"
#if FRAMELESSHELPER_CONFIG(native_impl)
#  ifdef Q_OS_WINDOWS
#    include "framelesshelper_win.h"
#  elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#  elif defined(Q_OS_MACOS)
#  else
#  endif
#else
#  include "framelesshelper_qt.h"
#endif
#include "framelessconfig_p.h"
#include "utils.h"
#ifdef Q_OS_WINDOWS
#  include "winverhelper_p.h"
#endif
#include <QtCore/qvariant.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qfontdatabase.h>
#include <QtGui/qwindow.h>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
#  include <QtGui/qguiapplication.h>
#  include <QtGui/qstylehints.h>
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessManager, "wangwenx190.framelesshelper.core.framelessmanager")
#  define INFO qCInfo(lcFramelessManager)
#  define DEBUG qCDebug(lcFramelessManager)
#  define WARNING qCWarning(lcFramelessManager)
#  define CRITICAL qCCritical(lcFramelessManager)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static constexpr const int kEventDelayInterval = 1000;

struct InternalData
{
    FramelessDataHash dataMap = {};
    QHash<WId, QObject *> windowMap = {};

    InternalData();
    ~InternalData();

private:
    FRAMELESSHELPER_CLASS(InternalData)
};

InternalData::InternalData() = default;

InternalData::~InternalData() = default;

Q_GLOBAL_STATIC(InternalData, g_internalData)

#if FRAMELESSHELPER_CONFIG(bundle_resource)
[[nodiscard]] static inline QString iconFontFamilyName()
{
    static const auto result = []() -> QString {
#ifdef Q_OS_WINDOWS
        if (WindowsVersionHelper::isWin11OrGreater()) {
            return FRAMELESSHELPER_STRING_LITERAL("Segoe Fluent Icons");
        }
        if (WindowsVersionHelper::isWin10OrGreater()) {
            return FRAMELESSHELPER_STRING_LITERAL("Segoe MDL2 Assets");
        }
#endif // Q_OS_WINDOWS
        return FRAMELESSHELPER_STRING_LITERAL("iconfont");
    }();
    return result;
}
#endif

InternalEventFilter::InternalEventFilter(const QObject *window, QObject *parent) : QObject(parent), m_window(window)
{
    Q_ASSERT(m_window);
    Q_ASSERT(m_window->isWidgetType() || m_window->isWindowType());
}

InternalEventFilter::~InternalEventFilter() = default;

bool InternalEventFilter::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    Q_ASSERT(m_window);
    if (!object || !event || !m_window || (object != m_window)) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(m_window);
    if (!data || !data->frameless || !data->callbacks) {
        return false;
    }
    if (event->type() == QEvent::WinIdChange) {
        const WId windowId = data->callbacks->getWindowId();
        Q_ASSERT(windowId);
        if (windowId) {
            FramelessManagerPrivate::updateWindowId(m_window, windowId);
        }
    }
    return false;
}

FramelessManagerPrivate::FramelessManagerPrivate(FramelessManager *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

FramelessManagerPrivate::~FramelessManagerPrivate() = default;

FramelessManagerPrivate *FramelessManagerPrivate::get(FramelessManager *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const FramelessManagerPrivate *FramelessManagerPrivate::get(const FramelessManager *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

void FramelessManagerPrivate::initializeIconFont()
{
#if FRAMELESSHELPER_CONFIG(bundle_resource)
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;
    FramelessHelperCoreInitResource();
    // We always register this font because it's our only fallback.
    const int id = QFontDatabase::addApplicationFont(FRAMELESSHELPER_STRING_LITERAL(":/org.wangwenx190.FramelessHelper/resources/fonts/iconfont.ttf"));
    if (id < 0) {
        WARNING << "Failed to load icon font.";
    } else {
        DEBUG << "Successfully registered icon font.";
    }
#endif // FRAMELESSHELPER_CORE_NO_BUNDLE_RESOURCE
}

QFont FramelessManagerPrivate::getIconFont()
{
#if FRAMELESSHELPER_CONFIG(bundle_resource)
    static const auto font = []() -> QFont {
        QFont f = {};
        f.setFamily(iconFontFamilyName());
#  ifdef Q_OS_MACOS
        f.setPointSize(10);
#  else // !Q_OS_MACOS
        f.setPointSize(8);
#  endif // Q_OS_MACOS
        return f;
    }();
    return font;
#else // !FRAMELESSHELPER_CONFIG(bundle_resource)
    return {};
#endif // FRAMELESSHELPER_CONFIG(bundle_resource)
}

void FramelessManagerPrivate::notifySystemThemeHasChangedOrNot()
{
    themeTimer.start();
}

void FramelessManagerPrivate::notifyWallpaperHasChangedOrNot()
{
    wallpaperTimer.start();
}

void FramelessManagerPrivate::doNotifySystemThemeHasChangedOrNot()
{
    const SystemTheme currentSystemTheme = (Utils::shouldAppsUseDarkMode() ? SystemTheme::Dark : SystemTheme::Light);
    const QColor currentAccentColor = Utils::getAccentColor();
#ifdef Q_OS_WINDOWS
    const DwmColorizationArea currentColorizationArea = Utils::getDwmColorizationArea();
#endif
    bool notify = false;
    if (systemTheme != currentSystemTheme) {
        systemTheme = currentSystemTheme;
        notify = true;
    }
    if (accentColor != currentAccentColor) {
        accentColor = currentAccentColor;
        notify = true;
    }
#ifdef Q_OS_WINDOWS
    if (colorizationArea != currentColorizationArea) {
        colorizationArea = currentColorizationArea;
        notify = true;
    }
#endif
    // Don't emit the signal if the user has overrided the global theme.
    if (notify && !isThemeOverrided()) {
        Q_Q(FramelessManager);
        Q_EMIT q->systemThemeChanged();
        DEBUG.nospace() << "System theme changed. Current theme: " << systemTheme
                        << ", accent color: " << accentColor.name(QColor::HexArgb).toUpper()
#ifdef Q_OS_WINDOWS
                        << ", colorization area: " << colorizationArea
#endif
                        << '.';
    }
}

void FramelessManagerPrivate::doNotifyWallpaperHasChangedOrNot()
{
    const QString currentWallpaper = Utils::getWallpaperFilePath();
    const WallpaperAspectStyle currentWallpaperAspectStyle = Utils::getWallpaperAspectStyle();
    bool notify = false;
    if (wallpaper != currentWallpaper) {
        wallpaper = currentWallpaper;
        notify = true;
    }
    if (wallpaperAspectStyle != currentWallpaperAspectStyle) {
        wallpaperAspectStyle = currentWallpaperAspectStyle;
        notify = true;
    }
    if (notify) {
        Q_Q(FramelessManager);
        Q_EMIT q->wallpaperChanged();
        DEBUG.nospace() << "Wallpaper changed. Current wallpaper: " << wallpaper
                        << ", aspect style: " << wallpaperAspectStyle << '.';
    }
}

FramelessDataPtr FramelessManagerPrivate::getData(const QObject *window)
{
    Q_ASSERT(window);
    Q_ASSERT(window->isWidgetType() || window->isWindowType());
    if (!window || !(window->isWidgetType() || window->isWindowType())) {
        return nullptr;
    }
    return g_internalData()->dataMap.value(const_cast<QObject *>(window));
}

FramelessDataPtr FramelessManagerPrivate::createData(const QObject *window, const WId windowId)
{
    Q_ASSERT(window);
    Q_ASSERT(window->isWidgetType() || window->isWindowType());
    Q_ASSERT(windowId);
    if (!window || !(window->isWidgetType() || window->isWindowType()) || !windowId) {
        return nullptr;
    }
    const auto win = const_cast<QObject *>(window);
    auto it = g_internalData()->dataMap.find(win);
    if (it == g_internalData()->dataMap.end()) {
        FramelessDataPtr data = FramelessData::create();
        data->window = win;
        data->windowId = windowId;
        it = g_internalData()->dataMap.insert(win, data);
        g_internalData()->windowMap.insert(windowId, win);
    }
    return it.value();
}

WId FramelessManagerPrivate::getWindowId(const QObject *window)
{
    Q_ASSERT(window);
    Q_ASSERT(window->isWidgetType() || window->isWindowType());
    if (!window || !(window->isWidgetType() || window->isWindowType())) {
        return 0;
    }
    if (const FramelessDataPtr data = getData(window)) {
        return data->windowId;
    }
    return 0;
}

QObject *FramelessManagerPrivate::getWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return nullptr;
    }
    return g_internalData()->windowMap.value(windowId);
}

void FramelessManagerPrivate::updateWindowId(const QObject *window, const WId newWindowId)
{
    Q_ASSERT(window);
    Q_ASSERT(window->isWidgetType() || window->isWindowType());
    Q_ASSERT(newWindowId);
    if (!window || !(window->isWidgetType() || window->isWindowType()) || !newWindowId) {
        return;
    }
    const auto win = const_cast<QObject *>(window);
    const FramelessDataPtr data = g_internalData()->dataMap.value(win);
    Q_ASSERT(data);
    if (!data) {
        return;
    }
    const WId oldWindowId = data->windowId;
    data->windowId = newWindowId;
    g_internalData()->windowMap.remove(oldWindowId);
    g_internalData()->windowMap.insert(newWindowId, win);
    data->frameless = false;
    std::ignore = FramelessManager::instance()->addWindow(window, newWindowId);
}

bool FramelessManagerPrivate::isThemeOverrided() const
{
    return (overrideTheme.value_or(SystemTheme::Unknown) != SystemTheme::Unknown);
}

void FramelessManagerPrivate::initialize()
{
    themeTimer.setInterval(kEventDelayInterval);
    themeTimer.callOnTimeout(this, [this](){
        themeTimer.stop();
        doNotifySystemThemeHasChangedOrNot();
    });
    wallpaperTimer.setInterval(kEventDelayInterval);
    wallpaperTimer.callOnTimeout(this, [this](){
        wallpaperTimer.stop();
        doNotifyWallpaperHasChangedOrNot();
    });
    systemTheme = (Utils::shouldAppsUseDarkMode() ? SystemTheme::Dark : SystemTheme::Light);
    accentColor = Utils::getAccentColor();
#ifdef Q_OS_WINDOWS
    colorizationArea = Utils::getDwmColorizationArea();
#endif
    wallpaper = Utils::getWallpaperFilePath();
    wallpaperAspectStyle = Utils::getWallpaperAspectStyle();
    DEBUG.nospace() << "Current system theme: " << systemTheme
                    << ", accent color: " << accentColor.name(QColor::HexArgb).toUpper()
#ifdef Q_OS_WINDOWS
                    << ", colorization area: " << colorizationArea
#endif
                    << ", wallpaper: " << wallpaper
                    << ", aspect style: " << wallpaperAspectStyle
                    << '.';
    // We are doing some tricks in our Windows message handling code, so
    // we don't use Qt's theme notifier on Windows. But for other platforms
    // we want to use as many Qt functionalities as possible.
#if ((QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)) && !FRAMELESSHELPER_CONFIG(native_impl))
    QStyleHints *styleHints = QGuiApplication::styleHints();
    Q_ASSERT(styleHints);
    if (styleHints) {
        connect(styleHints, &QStyleHints::colorSchemeChanged, this, [this](const Qt::ColorScheme colorScheme){
            Q_UNUSED(colorScheme);
            notifySystemThemeHasChangedOrNot();
        });
    }
#endif // ((QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)) && !defined(Q_OS_WINDOWS))
    static bool flagSet = false;
    if (!flagSet) {
        flagSet = true;
        // Set a global flag so that people can check whether FramelessHelper is being
        // used without actually accessing the FramelessHelper interface.
        static constexpr const char flag[] = "__FRAMELESSHELPER__";
        const auto ver = quint64(FramelessHelperVersion().version.num);
        qputenv(flag, QByteArray::number(ver));
        qApp->setProperty(flag, ver);
    }
}

FramelessManager::FramelessManager(QObject *parent) :
    QObject(parent), d_ptr(std::make_unique<FramelessManagerPrivate>(this))
{
}

FramelessManager::~FramelessManager() = default;

FramelessManager *FramelessManager::instance()
{
    static FramelessManager manager;
    return &manager;
}

SystemTheme FramelessManager::systemTheme() const
{
    Q_D(const FramelessManager);
    // The user's choice has top priority.
    if (d->isThemeOverrided()) {
        return d->overrideTheme.value();
    }
    return d->systemTheme;
}

QColor FramelessManager::systemAccentColor() const
{
    Q_D(const FramelessManager);
    return d->accentColor;
}

QString FramelessManager::wallpaper() const
{
    Q_D(const FramelessManager);
    return d->wallpaper;
}

WallpaperAspectStyle FramelessManager::wallpaperAspectStyle() const
{
    Q_D(const FramelessManager);
    return d->wallpaperAspectStyle;
}

void FramelessManager::setOverrideTheme(const SystemTheme theme)
{
    Q_D(FramelessManager);
    if ((!d->overrideTheme.has_value() && (theme == SystemTheme::Unknown))
        || (d->overrideTheme.has_value() && (d->overrideTheme.value() == theme))) {
        return;
    }
    if (theme == SystemTheme::Unknown) {
        d->overrideTheme = std::nullopt;
    } else {
        d->overrideTheme = theme;
    }
    Q_EMIT systemThemeChanged();
}

bool FramelessManager::addWindow(const QObject *window, const WId windowId)
{
    Q_ASSERT(window);
    Q_ASSERT(window->isWidgetType() || window->isWindowType());
    Q_ASSERT(windowId);
    if (!window || !(window->isWidgetType() || window->isWindowType()) || !windowId) {
        return false;
    }
    FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (data && data->frameless) {
        return false;
    }
    if (!data) {
        data = FramelessData::create();
        data->window = const_cast<QObject *>(window);
        data->windowId = windowId;
        g_internalData()->dataMap.insert(data->window, data);
        g_internalData()->windowMap.insert(windowId, data->window);
    }
#if FRAMELESSHELPER_CONFIG(native_impl)
#  ifdef Q_OS_WINDOWS
    std::ignore = Utils::installWindowProcHook(windowId);
    FramelessHelperWin::addWindow(window);
#  elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#  elif defined(Q_OS_MACOS)
#  else
#  endif
#else
    FramelessHelperQt::addWindow(window);
#endif
    if (!data->internalEventHandler) {
        data->internalEventHandler = new InternalEventFilter(data->window, data->window);
        data->window->installEventFilter(data->internalEventHandler);
    }
    return true;
}

bool FramelessManager::removeWindow(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    const auto it = g_internalData()->dataMap.constFind(const_cast<QObject *>(window));
    if (it == g_internalData()->dataMap.constEnd()) {
        return false;
    }
    const FramelessDataPtr data = it.value();
    Q_ASSERT(data);
    Q_ASSERT(data->window);
    Q_ASSERT(data->windowId);
    if (!data || !data->window || !data->windowId) {
        return false;
    }
    if (data->internalEventHandler) {
        data->window->removeEventFilter(data->internalEventHandler);
        delete data->internalEventHandler;
        data->internalEventHandler = nullptr;
    }
#if FRAMELESSHELPER_CONFIG(native_impl)
#  ifdef Q_OS_WINDOWS
    FramelessHelperWin::removeWindow(window);
    std::ignore = Utils::uninstallWindowProcHook(data->windowId);
#  elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#  elif defined(Q_OS_MACOS)
#  else
#  endif
#else
    FramelessHelperQt::removeWindow(window);
#endif
    g_internalData()->dataMap.erase(it);
    g_internalData()->windowMap.remove(data->windowId);
    return true;
}

FRAMELESSHELPER_END_NAMESPACE
