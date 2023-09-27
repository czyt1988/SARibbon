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
#include "framelesshelper_qt.h"
#include "framelessconfig_p.h"
#include "framelesshelpercore_global_p.h"
#include "utils.h"
#ifdef Q_OS_WINDOWS
#  include "framelesshelper_win.h"
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

using FramelessManagerData = QList<WId>;

Q_GLOBAL_STATIC(FramelessManagerData, g_framelessManagerData)

static constexpr const int kEventDelayInterval = 1000;

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

[[nodiscard]] static inline bool usePureQtImplementation()
{
    static const auto result = []() -> bool {
#ifdef Q_OS_WINDOWS
        return FramelessConfig::instance()->isSet(Option::UseCrossPlatformQtImplementation);
#else
        return true;
#endif
    }();
    return result;
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
    framelesshelpercore_initResource();
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
#if ((QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)) && !defined(Q_OS_WINDOWS))
    QStyleHints * const styleHints = QGuiApplication::styleHints();
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
    QObject(parent), d_ptr(new FramelessManagerPrivate(this))
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

void FramelessManager::addWindow(FramelessParamsConst params)
{
    Q_ASSERT(params);
    if (!params) {
        return;
    }
    const WId windowId = params->getWindowId();
    if (g_framelessManagerData()->contains(windowId)) {
        return;
    }
    g_framelessManagerData()->append(windowId);
    static const bool pureQt = usePureQtImplementation();
    if (pureQt) {
        FramelessHelperQt::addWindow(params);
    }
#ifdef Q_OS_WINDOWS
    if (!pureQt) {
        FramelessHelperWin::addWindow(params);
    }
    std::ignore = Utils::installWindowProcHook(windowId, params);
#endif
    connect(params->getWindowHandle(), &QWindow::destroyed, FramelessManager::instance(), [this, windowId](){ removeWindow(windowId); });
}

void FramelessManager::removeWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }
    if (!g_framelessManagerData()->contains(windowId)) {
        return;
    }
    g_framelessManagerData()->removeAll(windowId);
    static const bool pureQt = usePureQtImplementation();
    if (pureQt) {
        FramelessHelperQt::removeWindow(windowId);
    }
#ifdef Q_OS_WINDOWS
    if (!pureQt) {
        FramelessHelperWin::removeWindow(windowId);
    }
    std::ignore = Utils::uninstallWindowProcHook(windowId);
    std::ignore = Utils::removeMicaWindow(windowId);
#endif
}

FRAMELESSHELPER_END_NAMESPACE
