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

#ifdef Q_OS_MACOS

#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include "framelessconfig_p.h"
#include "framelesshelpercore_global_p.h"
#include <functional>
#include <memory>
#include <QtCore/qhash.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
#  include <QtCore/qoperatingsystemversion.h>
#else
#  include <QtCore/qsysinfo.h>
#endif
#include <QtGui/qwindow.h>
#include <objc/runtime.h>
#include <AppKit/AppKit.h>

QT_BEGIN_NAMESPACE
[[nodiscard]] extern Q_CORE_EXPORT bool qt_mac_applicationIsInDarkMode(); // Since 5.12
[[nodiscard]] extern Q_GUI_EXPORT QColor qt_mac_toQColor(const NSColor *color); // Since 5.8
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE
using Callback = std::function<void()>;
FRAMELESSHELPER_END_NAMESPACE

@interface MyKeyValueObserver : NSObject
@end

@implementation MyKeyValueObserver
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object
        change:(NSDictionary<NSKeyValueChangeKey, id> *)change context:(void *)context
{
    Q_UNUSED(keyPath);
    Q_UNUSED(object);
    Q_UNUSED(change);

    (*reinterpret_cast<FRAMELESSHELPER_PREPEND_NAMESPACE(Callback) *>(context))();
}
@end

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcUtilsMac, "wangwenx190.framelesshelper.core.utils.mac")
#  define INFO qCInfo(lcUtilsMac)
#  define DEBUG qCDebug(lcUtilsMac)
#  define WARNING qCWarning(lcUtilsMac)
#  define CRITICAL qCCritical(lcUtilsMac)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

class MacOSNotificationObserver
{
    FRAMELESSHELPER_CLASS(MacOSNotificationObserver)

public:
    explicit MacOSNotificationObserver(NSObject *object, NSNotificationName name, const Callback &callback) {
        Q_ASSERT(name);
        Q_ASSERT(callback);
        if (!name || !callback) {
            return;
        }
        observer = [[NSNotificationCenter defaultCenter] addObserverForName:name
            object:object queue:nil usingBlock:^(NSNotification *) {
                callback();
            }
        ];
    }

    explicit MacOSNotificationObserver() = default;

    ~MacOSNotificationObserver()
    {
        remove();
    }

    void remove()
    {
        if (!observer) {
            return;
        }
        [[NSNotificationCenter defaultCenter] removeObserver:observer];
        observer = nil;
    }

private:
    NSObject *observer = nil;
};

class MacOSKeyValueObserver
{
    FRAMELESSHELPER_CLASS(MacOSKeyValueObserver)

public:
    // Note: MacOSKeyValueObserver must not outlive the object observed!
    explicit MacOSKeyValueObserver(NSObject *obj, NSString *key, const Callback &cb,
        const NSKeyValueObservingOptions options = NSKeyValueObservingOptionNew)
    {
        Q_ASSERT(obj);
        Q_ASSERT(key);
        Q_ASSERT(cb);
        if (!obj || !key || !cb) {
            return;
        }
        object = obj;
        keyPath = key;
        callback = std::make_unique<Callback>(cb);
        addObserver(options);
    }

    explicit MacOSKeyValueObserver() = default;

    ~MacOSKeyValueObserver()
    {
        removeObserver();
    }

    void removeObserver()
    {
        if (!object) {
            return;
        }
        [object removeObserver:observer forKeyPath:keyPath context:callback.get()];
        object = nil;
    }

private:
    void addObserver(const NSKeyValueObservingOptions options)
    {
        [object addObserver:observer forKeyPath:keyPath options:options context:callback.get()];
    }

private:
    NSObject *object = nil;
    NSString *keyPath = nil;
    std::unique_ptr<Callback> callback = nil;

    static inline MyKeyValueObserver *observer = [[MyKeyValueObserver alloc] init];
};

class MacOSThemeObserver
{
    FRAMELESSHELPER_CLASS(MacOSThemeObserver)

public:
    explicit MacOSThemeObserver()
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 2))
        static const bool isMojave = (QOperatingSystemVersion::current() >= QOperatingSystemVersion::MacOSMojave);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 1))
        static const bool isMojave = (QOperatingSystemVersion::current() > QOperatingSystemVersion::MacOSHighSierra);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        static const bool isMojave = (QOperatingSystemVersion::current() > QOperatingSystemVersion::MacOSSierra);
#else
        static const bool isMojave = (QSysInfo::macVersion() > QSysInfo::MV_SIERRA);
#endif
        if (isMojave) {
            m_appearanceObserver = std::make_unique<MacOSKeyValueObserver>(NSApp, @"effectiveAppearance", [](){
                QT_WARNING_PUSH
                QT_WARNING_DISABLE_DEPRECATED
                NSAppearance.currentAppearance = NSApp.effectiveAppearance; // FIXME: use latest API.
                QT_WARNING_POP
                MacOSThemeObserver::notifySystemThemeChange();
            });
        }
        m_systemColorObserver = std::make_unique<MacOSNotificationObserver>(nil, NSSystemColorsDidChangeNotification,
            [](){ MacOSThemeObserver::notifySystemThemeChange(); });
    }

    ~MacOSThemeObserver() = default;

    static void notifySystemThemeChange()
    {
        // Sometimes the FramelessManager instance may be destroyed already.
        if (FramelessManager * const manager = FramelessManager::instance()) {
            if (FramelessManagerPrivate * const managerPriv = FramelessManagerPrivate::get(manager)) {
                managerPriv->notifySystemThemeHasChangedOrNot();
            }
        }
    }

private:
    std::unique_ptr<MacOSNotificationObserver> m_systemColorObserver = nil;
    std::unique_ptr<MacOSKeyValueObserver> m_appearanceObserver = nil;
};

class NSWindowProxy : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(NSWindowProxy)

public:
    explicit NSWindowProxy(QWindow *qtWindow, NSWindow *macWindow, QObject *parent = nil) : QObject(parent)
    {
        Q_ASSERT(qtWindow);
        Q_ASSERT(macWindow);
        Q_ASSERT(!instances.contains(macWindow));
        if (!qtWindow || !macWindow || instances.contains(macWindow)) {
            return;
        }
        qwindow = qtWindow;
        nswindow = macWindow;
        instances.insert(macWindow, this);
        if (!windowClass) {
            windowClass = [nswindow class];
            Q_ASSERT(windowClass);
            replaceImplementations();
        }
    }

    ~NSWindowProxy() override
    {
        instances.remove(nswindow);
        if (instances.count() <= 0) {
            windowClass = nil;
        }
        nswindow = nil;
    }

public Q_SLOTS:

    void replaceImplementations()
    {
        Method method = class_getInstanceMethod(windowClass, @selector(setStyleMask:));
        Q_ASSERT(method);
        oldSetStyleMask = reinterpret_cast<setStyleMaskPtr>(method_setImplementation(method, reinterpret_cast<IMP>(setStyleMask)));
        Q_ASSERT(oldSetStyleMask);

        method = class_getInstanceMethod(windowClass, @selector(setTitlebarAppearsTransparent:));
        Q_ASSERT(method);
        oldSetTitlebarAppearsTransparent = reinterpret_cast<setTitlebarAppearsTransparentPtr>(method_setImplementation(method, reinterpret_cast<IMP>(setTitlebarAppearsTransparent)));
        Q_ASSERT(oldSetTitlebarAppearsTransparent);

#if 0
        method = class_getInstanceMethod(windowClass, @selector(canBecomeKeyWindow));
        Q_ASSERT(method);
        oldCanBecomeKeyWindow = reinterpret_cast<canBecomeKeyWindowPtr>(method_setImplementation(method, reinterpret_cast<IMP>(canBecomeKeyWindow)));
        Q_ASSERT(oldCanBecomeKeyWindow);

        method = class_getInstanceMethod(windowClass, @selector(canBecomeMainWindow));
        Q_ASSERT(method);
        oldCanBecomeMainWindow = reinterpret_cast<canBecomeMainWindowPtr>(method_setImplementation(method, reinterpret_cast<IMP>(canBecomeMainWindow)));
        Q_ASSERT(oldCanBecomeMainWindow);
#endif

        method = class_getInstanceMethod(windowClass, @selector(sendEvent:));
        Q_ASSERT(method);
        oldSendEvent = reinterpret_cast<sendEventPtr>(method_setImplementation(method, reinterpret_cast<IMP>(sendEvent)));
        Q_ASSERT(oldSendEvent);
    }

    void restoreImplementations()
    {
        Method method = class_getInstanceMethod(windowClass, @selector(setStyleMask:));
        Q_ASSERT(method);
        method_setImplementation(method, reinterpret_cast<IMP>(oldSetStyleMask));
        oldSetStyleMask = nil;

        method = class_getInstanceMethod(windowClass, @selector(setTitlebarAppearsTransparent:));
        Q_ASSERT(method);
        method_setImplementation(method, reinterpret_cast<IMP>(oldSetTitlebarAppearsTransparent));
        oldSetTitlebarAppearsTransparent = nil;

#if 0
        method = class_getInstanceMethod(windowClass, @selector(canBecomeKeyWindow));
        Q_ASSERT(method);
        method_setImplementation(method, reinterpret_cast<IMP>(oldCanBecomeKeyWindow));
        oldCanBecomeKeyWindow = nil;

        method = class_getInstanceMethod(windowClass, @selector(canBecomeMainWindow));
        Q_ASSERT(method);
        method_setImplementation(method, reinterpret_cast<IMP>(oldCanBecomeMainWindow));
        oldCanBecomeMainWindow = nil;
#endif

        method = class_getInstanceMethod(windowClass, @selector(sendEvent:));
        Q_ASSERT(method);
        method_setImplementation(method, reinterpret_cast<IMP>(oldSendEvent));
        oldSendEvent = nil;
    }

    void setSystemTitleBarVisible(const bool visible)
    {
        NSView * const nsview = [nswindow contentView];
        Q_ASSERT(nsview);
        if (!nsview) {
            return;
        }
        nsview.wantsLayer = YES;
        nswindow.styleMask |= NSWindowStyleMaskResizable;
        if (visible) {
            nswindow.styleMask &= ~NSWindowStyleMaskFullSizeContentView;
        } else {
            nswindow.styleMask |= NSWindowStyleMaskFullSizeContentView;
        }
        nswindow.titlebarAppearsTransparent = (visible ? NO : YES);
        nswindow.titleVisibility = (visible ? NSWindowTitleVisible : NSWindowTitleHidden);
        nswindow.hasShadow = YES;
        nswindow.showsToolbarButton = NO;
        nswindow.movableByWindowBackground = NO;
        nswindow.movable = NO;
        // For some unknown reason, we don't need the following hack in Qt versions below or equal to 6.2.4.
#if (QT_VERSION > QT_VERSION_CHECK(6, 2, 4))
        [nswindow standardWindowButton:NSWindowCloseButton].hidden = (visible ? NO : YES);
        [nswindow standardWindowButton:NSWindowMiniaturizeButton].hidden = (visible ? NO : YES);
        [nswindow standardWindowButton:NSWindowZoomButton].hidden = (visible ? NO : YES);
#endif
    }

    void setBlurBehindWindowEnabled(const bool enable)
    {
        if (enable) {
            if (blurEffect) {
                return;
            }
            NSView * const view = [nswindow contentView];
#if 1
            const Class visualEffectViewClass = NSClassFromString(@"NSVisualEffectView");
            if (!visualEffectViewClass) {
                return;
            }
            NSVisualEffectView * const blurView = [[visualEffectViewClass alloc] initWithFrame:view.bounds];
#else
            NSVisualEffectView * const blurView = [[NSVisualEffectView alloc] initWithFrame:view.bounds];
#endif
            blurView.material = NSVisualEffectMaterialUnderWindowBackground;
            blurView.blendingMode = NSVisualEffectBlendingModeBehindWindow;
            blurView.state = NSVisualEffectStateFollowsWindowActiveState;
            const NSView * const parent = [view superview];
            [parent addSubview:blurView positioned:NSWindowBelow relativeTo:view];
            blurEffect = blurView;
            updateBlurTheme();
            updateBlurSize();
            connect(FramelessManager::instance(),
                &FramelessManager::systemThemeChanged, this, &NSWindowProxy::updateBlurTheme);
            connect(qwindow, &QWindow::widthChanged, this, &NSWindowProxy::updateBlurSize);
            connect(qwindow, &QWindow::heightChanged, this, &NSWindowProxy::updateBlurSize);
        } else {
            if (!blurEffect) {
                return;
            }
            if (widthChangeConnection) {
                disconnect(widthChangeConnection);
                widthChangeConnection = {};
            }
            if (heightChangeConnection) {
                disconnect(heightChangeConnection);
                heightChangeConnection = {};
            }
            if (themeChangeConnection) {
                disconnect(themeChangeConnection);
                themeChangeConnection = {};
            }
            [blurEffect removeFromSuperview];
            blurEffect = nil;
        }
    }

    void updateBlurSize()
    {
        if (!blurEffect) {
            return;
        }
        const NSView * const view = [nswindow contentView];
        blurEffect.frame = view.frame;
    }

    void updateBlurTheme()
    {
        if (!blurEffect) {
            return;
        }
        const auto view = static_cast<NSVisualEffectView *>(blurEffect);
        if (FramelessManager::instance()->systemTheme() == SystemTheme::Dark) {
            view.appearance = [NSAppearance appearanceNamed:@"NSAppearanceNameVibrantDark"];
        } else {
            view.appearance = [NSAppearance appearanceNamed:@"NSAppearanceNameVibrantLight"];
        }
    }

private:
    static BOOL canBecomeKeyWindow(id obj, SEL sel)
    {
        if (instances.contains(reinterpret_cast<NSWindow *>(obj))) {
            return YES;
        }

        if (oldCanBecomeKeyWindow) {
            return oldCanBecomeKeyWindow(obj, sel);
        }

        return YES;
    }

    static BOOL canBecomeMainWindow(id obj, SEL sel)
    {
        if (instances.contains(reinterpret_cast<NSWindow *>(obj))) {
            return YES;
        }

        if (oldCanBecomeMainWindow) {
            return oldCanBecomeMainWindow(obj, sel);
        }

        return YES;
    }

    static void setStyleMask(id obj, SEL sel, NSWindowStyleMask styleMask)
    {
        if (instances.contains(reinterpret_cast<NSWindow *>(obj))) {
            styleMask |= NSWindowStyleMaskFullSizeContentView;
        }

        if (oldSetStyleMask) {
            oldSetStyleMask(obj, sel, styleMask);
        }
    }

    static void setTitlebarAppearsTransparent(id obj, SEL sel, BOOL transparent)
    {
        if (instances.contains(reinterpret_cast<NSWindow *>(obj))) {
            transparent = YES;
        }

        if (oldSetTitlebarAppearsTransparent) {
            oldSetTitlebarAppearsTransparent(obj, sel, transparent);
        }
    }

    static void sendEvent(id obj, SEL sel, NSEvent *event)
    {
        if (oldSendEvent) {
            oldSendEvent(obj, sel, event);
        }

#if 0
        const auto nswindow = reinterpret_cast<NSWindow *>(obj);
        const auto it = instances.find(nswindow);
        if (it == instances.end()) {
            return;
        }

        NSWindowProxy * const proxy = it.value();
        if (event.type == NSEventTypeLeftMouseDown) {
            proxy->lastMouseDownEvent = event;
            QCoreApplication::processEvents();
            proxy->lastMouseDownEvent = nil;
        }
#endif
    }

private:
    QWindow *qwindow = nil;
    NSWindow *nswindow = nil;
    //NSEvent *lastMouseDownEvent = nil;
    NSView *blurEffect = nil;

    QMetaObject::Connection widthChangeConnection = {};
    QMetaObject::Connection heightChangeConnection = {};
    QMetaObject::Connection themeChangeConnection = {};

    static inline QHash<NSWindow *, NSWindowProxy *> instances = {};

    static inline Class windowClass = nil;

    using setStyleMaskPtr = void(*)(id, SEL, NSWindowStyleMask);
    static inline setStyleMaskPtr oldSetStyleMask = nil;

    using setTitlebarAppearsTransparentPtr = void(*)(id, SEL, BOOL);
    static inline setTitlebarAppearsTransparentPtr oldSetTitlebarAppearsTransparent = nil;

    using canBecomeKeyWindowPtr = BOOL(*)(id, SEL);
    static inline canBecomeKeyWindowPtr oldCanBecomeKeyWindow = nil;

    using canBecomeMainWindowPtr = BOOL(*)(id, SEL);
    static inline canBecomeMainWindowPtr oldCanBecomeMainWindow = nil;

    using sendEventPtr = void(*)(id, SEL, NSEvent *);
    static inline sendEventPtr oldSendEvent = nil;
};

using MacUtilsData = QHash<WId, NSWindowProxy *>;

Q_GLOBAL_STATIC(MacUtilsData, g_macUtilsData);

[[nodiscard]] static inline NSWindow *mac_getNSWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return nil;
    }
    const auto nsview = reinterpret_cast<NSView *>(windowId);
    Q_ASSERT(nsview);
    if (!nsview) {
        return nil;
    }
    return [nsview window];
}

static inline void cleanupProxy()
{
    if (g_macUtilsData()->isEmpty()) {
        return;
    }
    const auto &data = *g_macUtilsData();
    for (auto &&proxy : std::as_const(data)) {
        Q_ASSERT(proxy);
        if (!proxy) {
            continue;
        }
        delete proxy;
    }
    g_macUtilsData()->clear();
}

[[nodiscard]] static inline NSWindowProxy *ensureWindowProxy(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return nil;
    }
    auto it = g_macUtilsData()->find(windowId);
    if (it == g_macUtilsData()->end()) {
        QWindow * const qwindow = Utils::findWindow(windowId);
        Q_ASSERT(qwindow);
        if (!qwindow) {
            return nil;
        }
        NSWindow * const nswindow = mac_getNSWindow(windowId);
        Q_ASSERT(nswindow);
        if (!nswindow) {
            return nil;
        }
        const auto proxy = new NSWindowProxy(qwindow, nswindow);
        it = g_macUtilsData()->insert(windowId, proxy);
    }
    static bool cleanerInstalled = false;
    if (!cleanerInstalled) {
        cleanerInstalled = true;
        qAddPostRoutine(cleanupProxy);
    }
    return it.value();
}

void Utils::setSystemTitleBarVisible(const WId windowId, const bool visible)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }
    NSWindowProxy * const proxy = ensureWindowProxy(windowId);
    proxy->setSystemTitleBarVisible(visible);
}

bool Utils::startSystemMove(QWindow *window, const QPoint &globalPos)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    Q_UNUSED(globalPos);
    return window->startSystemMove();
#else
    const NSWindow * const nswindow = mac_getNSWindow(window->winId());
    Q_ASSERT(nswindow);
    if (!nswindow) {
        return false;
    }
    const CGEventRef clickDown = CGEventCreateMouseEvent(NULL, kCGEventLeftMouseDown,
                         CGPointMake(globalPos.x(), globalPos.y()), kCGMouseButtonLeft);
    NSEvent * const nsevent = [NSEvent eventWithCGEvent:clickDown];
    Q_ASSERT(nsevent);
    if (!nsevent) {
        CFRelease(clickDown);
        return false;
    }
    [nswindow performWindowDragWithEvent:nsevent];
    CFRelease(clickDown);
    return true;
#endif
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
    Q_UNUSED(globalPos);
    // Actually Qt doesn't implement this function, it will do nothing and always returns false.
    return window->startSystemResize(edges);
#else
    // ### TODO
    Q_UNUSED(globalPos);
    return false;
#endif
}

QColor Utils::getAccentColor_macos()
{
    return qt_mac_toQColor([NSColor controlAccentColor]);
}

bool Utils::isTitleBarColorized()
{
    return false;
}

bool Utils::shouldAppsUseDarkMode_macos()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    return qt_mac_applicationIsInDarkMode();
#else
    const auto appearance = [NSApp.effectiveAppearance bestMatchFromAppearancesWithNames:
                            @[NSAppearanceNameAqua, NSAppearanceNameDarkAqua]];
    return [appearance isEqualToString:NSAppearanceNameDarkAqua];
#endif
}

bool Utils::setBlurBehindWindowEnabled(const WId windowId, const BlurMode mode, const QColor &color)
{
    Q_UNUSED(color);
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto blurMode = [mode]() -> BlurMode {
        if ((mode == BlurMode::Disable) || (mode == BlurMode::Default)) {
            return mode;
        }
        WARNING << "The BlurMode::Windows_* enum values are not supported on macOS.";
        return BlurMode::Default;
    }();
    NSWindowProxy * const proxy = ensureWindowProxy(windowId);
    proxy->setBlurBehindWindowEnabled(blurMode == BlurMode::Default);
    return true;
}

QString Utils::getWallpaperFilePath()
{
#if 0
    const NSWorkspace * const sharedWorkspace = [NSWorkspace sharedWorkspace];
    if (!sharedWorkspace) {
        WARNING << "Failed to retrieve the shared workspace.";
        return {};
    }
    NSScreen * const mainScreen = [NSScreen mainScreen];
    if (!mainScreen) {
        WARNING << "Failed to retrieve the main screen.";
        return {};
    }
    const NSURL * const url = [sharedWorkspace desktopImageURLForScreen:mainScreen];
    if (!url) {
        WARNING << "Failed to retrieve the desktop image URL.";
        return {};
    }
    const QUrl path = QUrl::fromNSURL(url);
    if (!path.isValid()) {
        WARNING << "The converted QUrl is not valid.";
        return {};
    }
    return path.toLocalFile();
#else
    // ### TODO
    return {};
#endif
}

WallpaperAspectStyle Utils::getWallpaperAspectStyle()
{
    return WallpaperAspectStyle::Stretch;
}

bool Utils::isBlurBehindWindowSupported()
{
    static const auto result = []() -> bool {
        if (FramelessConfig::instance()->isSet(Option::ForceNonNativeBackgroundBlur)) {
            return false;
        }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        return (QOperatingSystemVersion::current() >= QOperatingSystemVersion::OSXYosemite);
#else
        return (QSysInfo::macVersion() >= QSysInfo::MV_YOSEMITE);
#endif
    }();
    return result;
}

bool Utils::registerThemeChangeNotification()
{
    volatile static MacOSThemeObserver observer;
    Q_UNUSED(observer);
    return true;
}

void Utils::removeWindowProxy(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }
    const auto it = g_macUtilsData()->constFind(windowId);
    if (it == g_macUtilsData()->constEnd()) {
        return;
    }
    if (const auto proxy = it.value()) {
        // We'll restore everything to default in the destructor,
        // so no need to do it manually here.
        delete proxy;
    }
    g_macUtilsData()->erase(it);
}

QColor Utils::getFrameBorderColor(const bool active)
{
    return (active ? getAccentColor() : kDefaultDarkGrayColor);
}

FRAMELESSHELPER_END_NAMESPACE

#include "utils_mac.moc"

#endif // Q_OS_MACOS
