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

#include "framelesswidgetshelper.h"
#include "framelesswidgetshelper_p.h"
#if FRAMELESSHELPER_CONFIG(window)
#  include "framelesswidget.h"
#  include "framelesswidget_p.h"
#  include "framelessmainwindow.h"
#  include "framelessmainwindow_p.h"
#  include "framelessdialog.h"
#  include "framelessdialog_p.h"
#endif
#include "widgetssharedhelper_p.h"
#include <FramelessHelper/Core/framelessmanager.h>
#include <FramelessHelper/Core/utils.h>
#include <FramelessHelper/Core/private/framelessconfig_p.h>
#include <FramelessHelper/Core/private/framelesshelpercore_global_p.h>
#include <QtCore/qhash.h>
#include <QtCore/qtimer.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qwindow.h>
#include <QtGui/qpalette.h>
#include <QtGui/qcursor.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qapplication.h>

#ifndef QWIDGETSIZE_MAX
#  define QWIDGETSIZE_MAX ((1 << 24) - 1)
#endif // QWIDGETSIZE_MAX

extern Q_WIDGETS_EXPORT QWidget *qt_button_down;

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessWidgetsHelper, "wangwenx190.framelesshelper.widgets.framelesswidgetshelper")
#  define INFO qCInfo(lcFramelessWidgetsHelper)
#  define DEBUG qCDebug(lcFramelessWidgetsHelper)
#  define WARNING qCWarning(lcFramelessWidgetsHelper)
#  define CRITICAL qCCritical(lcFramelessWidgetsHelper)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

struct FramelessWidgetsHelperData
{
    bool ready = false;
    SystemParameters params = {};
    QPointer<QWidget> titleBarWidget = nullptr;
    QList<QPointer<QWidget>> hitTestVisibleWidgets = {};
    QPointer<QWidget> windowIconButton = nullptr;
    QPointer<QWidget> contextHelpButton = nullptr;
    QPointer<QWidget> minimizeButton = nullptr;
    QPointer<QWidget> maximizeButton = nullptr;
    QPointer<QWidget> closeButton = nullptr;
    QList<QRect> hitTestVisibleRects = {};
};

using FramelessWidgetsHelperInternal = QHash<WId, FramelessWidgetsHelperData>;

Q_GLOBAL_STATIC(FramelessWidgetsHelperInternal, g_framelessWidgetsHelperData)

[[nodiscard]] static inline bool isWidgetFixedSize(const QWidget * const widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return false;
    }
    // "Qt::MSWindowsFixedSizeDialogHint" is used cross-platform actually.
    if (widget->windowFlags() & Qt::MSWindowsFixedSizeDialogHint) {
        return true;
    }
    // Caused by setFixedWidth/Height/Size().
    const QSize minSize = widget->minimumSize();
    const QSize maxSize = widget->maximumSize();
    if (!minSize.isEmpty() && !maxSize.isEmpty() && (minSize == maxSize)) {
        return true;
    }
    // Usually set by the user.
    const QSizePolicy sizePolicy = widget->sizePolicy();
    if ((sizePolicy.horizontalPolicy() == QSizePolicy::Fixed)
        && (sizePolicy.verticalPolicy() == QSizePolicy::Fixed)) {
        return true;
    }
    return false;
}

static inline void forceWidgetRepaint(QWidget * const widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
#ifdef Q_OS_WINDOWS
    // There's some additional things to do for top level windows on Windows.
    if (widget->isWindow()) {
        // Don't crash if the QWindow instance has not been created yet.
        if (QWindow * const window = widget->windowHandle()) {
            // Sync the internal window frame margins with the latest DPI, otherwise
            // we will get wrong window sizes after the DPI change.
            std::ignore = Utils::updateInternalWindowFrameMargins(window, true);
        }
    }
#endif // Q_OS_WINDOWS
    // Don't do unnecessary repaints if the widget is hidden.
    if (!widget->isVisible()) {
        return;
    }
    // Tell the widget to repaint itself, but it may not happen due to QWidget's
    // internal painting optimizations.
    widget->update();
    // Try to force the widget to repaint itself, in case:
    //   (1) It's a child widget;
    //   (2) It's a top level window but not minimized/maximized/fullscreen.
    if (!widget->isWindow() || !(widget->windowState() & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen))) {
        // A widget will most likely repaint itself if it's size is changed.
        if (!isWidgetFixedSize(widget)) {
            const QSize originalSize = widget->size();
            static constexpr const auto margins = QMargins{10, 10, 10, 10};
            widget->resize(originalSize.shrunkBy(margins));
            widget->resize(originalSize.grownBy(margins));
            widget->resize(originalSize);
        }
        // However, some widgets won't repaint themselves unless their position is changed.
        const QPoint originalPosition = widget->pos();
        static constexpr const auto offset = QPoint{10, 10};
        widget->move(originalPosition - offset);
        widget->move(originalPosition + offset);
        widget->move(originalPosition);
    }
    // Let's try again with the ordinary way.
    widget->update();
    // ### TODO: I observed the font size is often wrong after DPI changes,
    // do we need to refresh the font settings here as well?
}

FramelessWidgetsHelperPrivate::FramelessWidgetsHelperPrivate(FramelessWidgetsHelper *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
}

FramelessWidgetsHelperPrivate::~FramelessWidgetsHelperPrivate() = default;

FramelessWidgetsHelperPrivate *FramelessWidgetsHelperPrivate::get(FramelessWidgetsHelper *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const FramelessWidgetsHelperPrivate *FramelessWidgetsHelperPrivate::get(const FramelessWidgetsHelper *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

void FramelessWidgetsHelperPrivate::emitSignalForAllInstances(const char *signal)
{
    Q_ASSERT(signal);
    Q_ASSERT(*signal != '\0');
    if (!signal || (*signal == '\0')) {
        return;
    }
    if (!window) {
        return;
    }
    const auto instances = window->findChildren<FramelessWidgetsHelper *>();
    if (instances.isEmpty()) {
        return;
    }
    for (auto &&instance : std::as_const(instances)) {
        QMetaObject::invokeMethod(instance, signal);
    }
}

void FramelessWidgetsHelperPrivate::setProperty(const char *name, const QVariant &value)
{
    Q_ASSERT(name);
    Q_ASSERT(*name != '\0');
    Q_ASSERT(value.isValid());
    Q_ASSERT(!value.isNull());
    if (!name || (*name == '\0') || !value.isValid() || value.isNull()) {
        return;
    }
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    window->setProperty(name, value);
}

QVariant FramelessWidgetsHelperPrivate::getProperty(const char *name, const QVariant &defaultValue)
{
    Q_ASSERT(name);
    Q_ASSERT(*name != '\0');
    if (!name || (*name == '\0')) {
        return {};
    }
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
    const QVariant value = window->property(name);
    return ((value.isValid() && !value.isNull()) ? value : defaultValue);
}

#if FRAMELESSHELPER_CONFIG(mica_material)
MicaMaterial *FramelessWidgetsHelperPrivate::getMicaMaterialIfAny() const
{
    if (!window) {
        return nullptr;
    }
    if (const WidgetsSharedHelper * const helper = findOrCreateSharedHelper(window)) {
        return helper->rawMicaMaterial();
    }
    return nullptr;
}
#endif

#if FRAMELESSHELPER_CONFIG(border_painter)
WindowBorderPainter *FramelessWidgetsHelperPrivate::getWindowBorderIfAny() const
{
    if (!window) {
        return nullptr;
    }
    if (const WidgetsSharedHelper * const helper = findOrCreateSharedHelper(window)) {
        return helper->rawWindowBorder();
    }
    return nullptr;
}
#endif

WidgetsSharedHelper *FramelessWidgetsHelperPrivate::findOrCreateSharedHelper(QWidget *window)
{
    Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
#if FRAMELESSHELPER_CONFIG(window)
    if (const auto widget = qobject_cast<FramelessWidget *>(window)) {
        if (const auto widgetPriv = FramelessWidgetPrivate::get(widget)) {
            return widgetPriv->sharedHelper;
        }
    }
    if (const auto mainWindow = qobject_cast<FramelessMainWindow *>(window)) {
        if (const auto mainWindowPriv = FramelessMainWindowPrivate::get(mainWindow)) {
            return mainWindowPriv->sharedHelper;
        }
    }
    if (const auto dialog = qobject_cast<FramelessDialog *>(window)) {
        if (const auto dialogPriv = FramelessDialogPrivate::get(dialog)) {
            return dialogPriv->sharedHelper;
        }
    }
#endif
    QWidget * const topLevelWindow = window->window();
    WidgetsSharedHelper *helper = topLevelWindow->findChild<WidgetsSharedHelper *>();
    if (!helper) {
        helper = new WidgetsSharedHelper(topLevelWindow);
        helper->setup(topLevelWindow);
    }
    return helper;
}

FramelessWidgetsHelper *FramelessWidgetsHelperPrivate::findOrCreateFramelessHelper(QObject *object)
{
    Q_ASSERT(object);
    if (!object) {
        return nullptr;
    }
    QObject *parent = nullptr;
    if (const auto widget = qobject_cast<QWidget *>(object)) {
        parent = widget->window();
    } else {
        parent = object;
    }
    FramelessWidgetsHelper *instance = parent->findChild<FramelessWidgetsHelper *>();
    if (!instance) {
        instance = new FramelessWidgetsHelper(parent);
        instance->extendsContentIntoTitleBar();
        if (parent->inherits("QDialog")) {
            instance->setWindowFixedSize(true);
        }
    }
    return instance;
}

void FramelessWidgetsHelperPrivate::repaintAllChildren(const quint32 delay) const
{
    if (!window) {
        return;
    }
    const auto update = [this]() -> void {
        forceWidgetRepaint(window);
        const QList<QWidget *> widgets = window->findChildren<QWidget *>();
        if (widgets.isEmpty()) {
            return;
        }
        for (auto &&widget : std::as_const(widgets)) {
            forceWidgetRepaint(widget);
        }
    };
    if (delay > 0) {
        QTimer::singleShot(delay, this, update);
    } else {
        update();
    }
}

quint32 FramelessWidgetsHelperPrivate::readyWaitTime() const
{
    return qpaWaitTime;
}

void FramelessWidgetsHelperPrivate::setReadyWaitTime(const quint32 time)
{
    if (qpaWaitTime == time) {
        return;
    }
    qpaWaitTime = time;
}

void FramelessWidgetsHelperPrivate::attach()
{
    QWidget * const tlw = findTopLevelWindow();
    Q_ASSERT(tlw);
    if (!tlw) {
        return;
    }
    if (window == tlw) {
        return;
    }
    window = tlw;

    if (!window->testAttribute(Qt::WA_DontCreateNativeAncestors)) {
        window->setAttribute(Qt::WA_DontCreateNativeAncestors);
    }
    if (!window->testAttribute(Qt::WA_NativeWindow)) {
        window->setAttribute(Qt::WA_NativeWindow);
    }

    FramelessWidgetsHelperData * const data = getWindowDataMutable();
    if (!data || data->ready) {
        return;
    }

    Q_Q(FramelessWidgetsHelper);

    SystemParameters params = {};
    params.getWindowId = [this]() -> WId { return window->winId(); };
    params.getWindowFlags = [this]() -> Qt::WindowFlags { return window->windowFlags(); };
    params.setWindowFlags = [this](const Qt::WindowFlags flags) -> void { window->setWindowFlags(flags); };
    params.getWindowSize = [this]() -> QSize { return window->size(); };
    params.setWindowSize = [this](const QSize &size) -> void { window->resize(size); };
    params.getWindowPosition = [this]() -> QPoint { return window->pos(); };
    params.setWindowPosition = [this](const QPoint &pos) -> void { window->move(pos); };
    params.getWindowScreen = [this]() -> QScreen * {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        return window->screen();
#else
        return window->windowHandle()->screen();
#endif
    };
    params.isWindowFixedSize = [q]() -> bool { return q->isWindowFixedSize(); };
    params.setWindowFixedSize = [q](const bool value) -> void { q->setWindowFixedSize(value); };
    params.getWindowState = [this]() -> Qt::WindowState { return Utils::windowStatesToWindowState(window->windowState()); };
    params.setWindowState = [this](const Qt::WindowState state) -> void { window->setWindowState(state); };
    params.getWindowHandle = [this]() -> QWindow * { return window->windowHandle(); };
    params.windowToScreen = [this](const QPoint &pos) -> QPoint { return window->mapToGlobal(pos); };
    params.screenToWindow = [this](const QPoint &pos) -> QPoint { return window->mapFromGlobal(pos); };
    params.isInsideSystemButtons = [this](const QPoint &pos, SystemButtonType *button) -> bool { return isInSystemButtons(pos, button); };
    params.isInsideTitleBarDraggableArea = [this](const QPoint &pos) -> bool { return isInTitleBarDraggableArea(pos); };
    params.getWindowDevicePixelRatio = [this]() -> qreal { return window->devicePixelRatioF(); };
    params.setSystemButtonState = [this](const SystemButtonType button, const ButtonState state) -> void { setSystemButtonState(button, state); };
    params.shouldIgnoreMouseEvents = [this](const QPoint &pos) -> bool { return shouldIgnoreMouseEvents(pos); };
    params.showSystemMenu = [q](const QPoint &pos) -> void { q->showSystemMenu(pos); };
    params.setProperty = [this](const char *name, const QVariant &value) -> void { setProperty(name, value); };
    params.getProperty = [this](const char *name, const QVariant &defaultValue) -> QVariant { return getProperty(name, defaultValue); };
    params.setCursor = [this](const QCursor &cursor) -> void { window->setCursor(cursor); };
    params.unsetCursor = [this]() -> void { window->unsetCursor(); };
    params.getWidgetHandle = [this]() -> QObject * { return window; };
    params.forceChildrenRepaint = [this](const int delay) -> void { repaintAllChildren(delay); };
    params.resetQtGrabbedControl = []() -> bool {
        if (qt_button_down) {
            static constexpr const auto invalidPos = QPoint{ -99999, -99999 };
            const auto event = std::make_unique<QMouseEvent>(
                QEvent::MouseButtonRelease,
                invalidPos,
                invalidPos,
                invalidPos,
                Qt::LeftButton,
                QGuiApplication::mouseButtons() ^ Qt::LeftButton,
                QGuiApplication::keyboardModifiers());
            QApplication::sendEvent(qt_button_down, event.get());
            qt_button_down = nullptr;
            return true;
        }
        return false;
    };

    FramelessManager::instance()->addWindow(&params);

    data->params = params;
    data->ready = true;

    // We have to wait for a little time before moving the top level window
    // , because the platform window may not finish initializing by the time
    // we reach here, and all the modifications from the Qt side will be lost
    // due to QPA will reset the position and size of the window during it's
    // initialization process.
    QTimer::singleShot(qpaWaitTime, this, [this, q](){
        qpaReady = true;
        if (FramelessConfig::instance()->isSet(Option::CenterWindowBeforeShow)) {
            q->moveWindowToDesktopCenter();
        }
        if (FramelessConfig::instance()->isSet(Option::EnableBlurBehindWindow)) {
            q->setBlurBehindWindowEnabled(true);
        }
        emitSignalForAllInstances("windowChanged");
        emitSignalForAllInstances("ready");
    });
}

void FramelessWidgetsHelperPrivate::detach()
{
    if (!window) {
        return;
    }
    const WId windowId = window->winId();
    const auto it = g_framelessWidgetsHelperData()->constFind(windowId);
    if (it == g_framelessWidgetsHelperData()->constEnd()) {
        return;
    }
    g_framelessWidgetsHelperData()->erase(it);
    FramelessManager::instance()->removeWindow(windowId);
    window = nullptr;
    emitSignalForAllInstances("windowChanged");
}

QWidget *FramelessWidgetsHelperPrivate::findTopLevelWindow() const
{
    Q_Q(const FramelessWidgetsHelper);
    const QObject * const p = q->parent();
    Q_ASSERT(p);
    if (p) {
        if (const auto parentWidget = qobject_cast<const QWidget *>(p)) {
            return parentWidget->window();
        }
    }
    return nullptr;
}

const FramelessWidgetsHelperData *FramelessWidgetsHelperPrivate::getWindowData() const
{
    //Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
    const WId windowId = window->winId();
    auto it = g_framelessWidgetsHelperData()->find(windowId);
    if (it == g_framelessWidgetsHelperData()->end()) {
        it = g_framelessWidgetsHelperData()->insert(windowId, {});
    }
    return &it.value();
}

FramelessWidgetsHelperData *FramelessWidgetsHelperPrivate::getWindowDataMutable() const
{
    //Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
    const WId windowId = window->winId();
    auto it = g_framelessWidgetsHelperData()->find(windowId);
    if (it == g_framelessWidgetsHelperData()->end()) {
        it = g_framelessWidgetsHelperData()->insert(windowId, {});
    }
    return &it.value();
}

QRect FramelessWidgetsHelperPrivate::mapWidgetGeometryToScene(const QWidget * const widget) const
{
    Q_ASSERT(widget);
    if (!widget) {
        return {};
    }
    if (!window) {
        return {};
    }
    const QPoint originPoint = widget->mapTo(window, QPoint(0, 0));
    const QSize size = widget->size();
    return QRect(originPoint, size);
}

bool FramelessWidgetsHelperPrivate::isInSystemButtons(const QPoint &pos, SystemButtonType *button) const
{
    Q_ASSERT(button);
    if (!button) {
        return false;
    }
    const FramelessWidgetsHelperData *data = getWindowData();
    if (!data) {
        return false;
    }
    *button = SystemButtonType::Unknown;
    if (data->windowIconButton && data->windowIconButton->isVisible() && data->windowIconButton->isEnabled()) {
        if (mapWidgetGeometryToScene(data->windowIconButton).contains(pos)) {
            *button = SystemButtonType::WindowIcon;
            return true;
        }
    }
    if (data->contextHelpButton && data->contextHelpButton->isVisible() && data->contextHelpButton->isEnabled()) {
        if (mapWidgetGeometryToScene(data->contextHelpButton).contains(pos)) {
            *button = SystemButtonType::Help;
            return true;
        }
    }
    if (data->minimizeButton && data->minimizeButton->isVisible() && data->minimizeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(data->minimizeButton).contains(pos)) {
            *button = SystemButtonType::Minimize;
            return true;
        }
    }
    if (data->maximizeButton && data->maximizeButton->isVisible() && data->maximizeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(data->maximizeButton).contains(pos)) {
            *button = SystemButtonType::Maximize;
            return true;
        }
    }
    if (data->closeButton && data->closeButton->isVisible() && data->closeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(data->closeButton).contains(pos)) {
            *button = SystemButtonType::Close;
            return true;
        }
    }
    return false;
}

bool FramelessWidgetsHelperPrivate::isInTitleBarDraggableArea(const QPoint &pos) const
{
    const FramelessWidgetsHelperData *data = getWindowData();
    if (!data) {
        return false;
    }
    if (!data->titleBarWidget) {
        // There's no title bar at all, the mouse will always be in the client area.
        return false;
    }
    if (!data->titleBarWidget->isVisible() || !data->titleBarWidget->isEnabled()) {
        // The title bar is hidden or disabled for some reason, treat it as there's no title bar.
        return false;
    }
    if (!window) {
        // The FramelessWidgetsHelper object has not been attached to a specific window yet,
        // so we assume there's no title bar.
        return false;
    }
    const QRect windowRect = {QPoint(0, 0), window->size()};
    const QRect titleBarRect = mapWidgetGeometryToScene(data->titleBarWidget);
    if (!titleBarRect.intersects(windowRect)) {
        // The title bar is totally outside of the window for some reason,
        // also treat it as there's no title bar.
        return false;
    }
    QRegion region = titleBarRect;
    const auto systemButtons = {
        data->windowIconButton, data->contextHelpButton,
        data->minimizeButton, data->maximizeButton,
        data->closeButton
    };
    for (auto &&button : std::as_const(systemButtons)) {
        if (button && button->isVisible() && button->isEnabled()) {
            region -= mapWidgetGeometryToScene(button);
        }
    }
    if (!data->hitTestVisibleWidgets.isEmpty()) {
        for (auto &&widget : std::as_const(data->hitTestVisibleWidgets)) {
            if (widget && widget->isVisible() && widget->isEnabled()) {
                region -= mapWidgetGeometryToScene(widget);
            }
        }
    }
    if (!data->hitTestVisibleRects.isEmpty()) {
        for (auto &&rect : std::as_const(data->hitTestVisibleRects)) {
            if (rect.isValid()) {
                region -= rect;
            }
        }
    }
    return region.contains(pos);
}

bool FramelessWidgetsHelperPrivate::shouldIgnoreMouseEvents(const QPoint &pos) const
{
    if (!window) {
        return false;
    }
    const auto withinFrameBorder = [this, &pos]() -> bool {
        if (pos.y() < kDefaultResizeBorderThickness) {
            return true;
        }
#ifdef Q_OS_WINDOWS
        if (Utils::isWindowFrameBorderVisible()) {
            return false;
        }
#endif
        return ((pos.x() < kDefaultResizeBorderThickness)
                || (pos.x() >= (window->width() - kDefaultResizeBorderThickness)));
    }();
    return ((Utils::windowStatesToWindowState(window->windowState()) == Qt::WindowNoState) && withinFrameBorder);
}

void FramelessWidgetsHelperPrivate::setSystemButtonState(const SystemButtonType button, const ButtonState state)
{
    Q_UNUSED(button);
    Q_UNUSED(state);
}

void FramelessWidgetsHelper::moveWindowToDesktopCenter()
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    Utils::moveWindowToDesktopCenter(&d->getWindowData()->params, true);
}

void FramelessWidgetsHelper::bringWindowToFront()
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::bringWindowToFront(d->window->winId());
#else
    if (d->window->isHidden()) {
        d->window->show();
    }
    if (d->window->isMinimized()) {
        d->window->setWindowState(d->window->windowState() & ~Qt::WindowMinimized);
    }
    d->window->raise();
    d->window->activateWindow();
#endif
}

void FramelessWidgetsHelper::showSystemMenu(const QPoint &pos)
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    const WId windowId = d->window->winId();
    const QPoint nativePos = Utils::toNativeGlobalPosition(d->window->windowHandle(), pos);
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::showSystemMenu(windowId, nativePos, false, &d->getWindowData()->params);
#elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    Utils::openSystemMenu(windowId, nativePos);
#else
    Q_UNUSED(windowId);
    Q_UNUSED(nativePos);
#endif
}

void FramelessWidgetsHelper::windowStartSystemMove2(const QPoint &pos)
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    std::ignore = Utils::startSystemMove(d->window->windowHandle(), pos);
}

void FramelessWidgetsHelper::windowStartSystemResize2(const Qt::Edges edges, const QPoint &pos)
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    if (edges == Qt::Edges{}) {
        return;
    }
    std::ignore = Utils::startSystemResize(d->window->windowHandle(), edges, pos);
}

void FramelessWidgetsHelper::setSystemButton(QWidget *widget, const SystemButtonType buttonType)
{
    Q_ASSERT(widget);
    Q_ASSERT(buttonType != SystemButtonType::Unknown);
    if (!widget || (buttonType == SystemButtonType::Unknown)) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    FramelessWidgetsHelperData *data = d->getWindowDataMutable();
    if (!data) {
        return;
    }
    switch (buttonType) {
    case SystemButtonType::WindowIcon:
        data->windowIconButton = widget;
        break;
    case SystemButtonType::Help:
        data->contextHelpButton = widget;
        break;
    case SystemButtonType::Minimize:
        data->minimizeButton = widget;
        break;
    case SystemButtonType::Maximize:
    case SystemButtonType::Restore:
        data->maximizeButton = widget;
        break;
    case SystemButtonType::Close:
        data->closeButton = widget;
        break;
    case SystemButtonType::Unknown:
        Q_UNREACHABLE();
    }
}

FramelessWidgetsHelper::FramelessWidgetsHelper(QObject *parent)
    : QObject(parent), d_ptr(new FramelessWidgetsHelperPrivate(this))
{
}

FramelessWidgetsHelper::~FramelessWidgetsHelper() = default;

FramelessWidgetsHelper *FramelessWidgetsHelper::get(QObject *object)
{
    Q_ASSERT(object);
    if (!object) {
        return nullptr;
    }
    return FramelessWidgetsHelperPrivate::findOrCreateFramelessHelper(object);
}

QWidget *FramelessWidgetsHelper::window() const
{
    Q_D(const FramelessWidgetsHelper);
    return d->window;
}

#if FRAMELESSHELPER_CONFIG(mica_material)
MicaMaterial *FramelessWidgetsHelper::micaMaterial() const
{
    Q_D(const FramelessWidgetsHelper);
    return d->getMicaMaterialIfAny();
}
#endif

#if FRAMELESSHELPER_CONFIG(border_painter)
WindowBorderPainter *FramelessWidgetsHelper::windowBorder() const
{
    Q_D(const FramelessWidgetsHelper);
    return d->getWindowBorderIfAny();
}
#endif

bool FramelessWidgetsHelper::isWindowFixedSize() const
{
    Q_D(const FramelessWidgetsHelper);
    if (!d->window) {
        return false;
    }
    return isWidgetFixedSize(d->window);
}

void FramelessWidgetsHelper::setWindowFixedSize(const bool value)
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    if (isWindowFixedSize() == value) {
        return;
    }
    if (value) {
        d->savedSizePolicy = d->window->sizePolicy();
        d->window->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        d->window->setFixedSize(d->window->size());
    } else {
        d->window->setSizePolicy(d->savedSizePolicy);
        d->window->setMinimumSize(kDefaultWindowSize);
        d->window->setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    }
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::setAeroSnappingEnabled(d->window->winId(), !value);
#endif
    d->emitSignalForAllInstances("windowFixedSizeChanged");
}

bool FramelessWidgetsHelper::isBlurBehindWindowEnabled() const
{
    Q_D(const FramelessWidgetsHelper);
    return d->blurBehindWindowEnabled;
}

void FramelessWidgetsHelper::setBlurBehindWindowEnabled(const bool enable)
{
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    if (d->blurBehindWindowEnabled == enable) {
        return;
    }
    if (Utils::isBlurBehindWindowSupported()) {
        QPalette palette = d->window->palette();
        if (enable) {
            d->savedWindowBackgroundColor = palette.color(QPalette::Window);
        }
        palette.setColor(QPalette::Window, (enable ? kDefaultTransparentColor : d->savedWindowBackgroundColor));
        d->window->setPalette(palette);
        if (Utils::setBlurBehindWindowEnabled(d->window->winId(), (enable ? BlurMode::Default : BlurMode::Disable), {})) {
            d->blurBehindWindowEnabled = enable;
            d->emitSignalForAllInstances("blurBehindWindowEnabledChanged");
        } else {
            WARNING << "Failed to enable/disable blur behind window.";
        }
    } else {
        if (WidgetsSharedHelper * const helper = d->findOrCreateSharedHelper(d->window)) {
            d->blurBehindWindowEnabled = enable;
#if FRAMELESSHELPER_CONFIG(mica_material)
            helper->setMicaEnabled(d->blurBehindWindowEnabled);
#endif
            d->emitSignalForAllInstances("blurBehindWindowEnabledChanged");
        } else {
            DEBUG << "Blur behind window is not supported on current platform.";
        }
    }
}

bool FramelessWidgetsHelper::isReady() const
{
    Q_D(const FramelessWidgetsHelper);
    return d->qpaReady;
}

void FramelessWidgetsHelper::waitForReady()
{
    Q_D(FramelessWidgetsHelper);
    if (d->qpaReady) {
        return;
    }
#if 1
    QEventLoop loop{};
    const QMetaObject::Connection connection = connect(this, &FramelessWidgetsHelper::ready, &loop, &QEventLoop::quit);
    loop.exec();
    disconnect(connection);
#else
    while (!d->qpaReady) {
        QCoreApplication::processEvents();
    }
#endif
}

bool FramelessWidgetsHelper::isContentExtendedIntoTitleBar() const
{
    Q_D(const FramelessWidgetsHelper);
    const FramelessWidgetsHelperData *data = d->getWindowData();
    return (data ? data->ready : false);
}

void FramelessWidgetsHelper::setTitleBarWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    FramelessWidgetsHelperData *data = d->getWindowDataMutable();
    if (!data || (data->titleBarWidget == widget)) {
        return;
    }
    data->titleBarWidget = widget;
    d->emitSignalForAllInstances("titleBarWidgetChanged");
}

QWidget *FramelessWidgetsHelper::titleBarWidget() const
{
    Q_D(const FramelessWidgetsHelper);
    const FramelessWidgetsHelperData *data = d->getWindowData();
    return (data ? data->titleBarWidget : nullptr);
}

void FramelessWidgetsHelper::setHitTestVisible(QWidget *widget, const bool visible)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    FramelessWidgetsHelperData *data = d->getWindowDataMutable();
    if (!data) {
        return;
    }
    if (visible) {
        data->hitTestVisibleWidgets.append(widget);
    } else {
        data->hitTestVisibleWidgets.removeAll(widget);
    }
}

void FramelessWidgetsHelper::setHitTestVisible(const QRect &rect, const bool visible)
{
    Q_ASSERT(rect.isValid());
    if (!rect.isValid()) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    FramelessWidgetsHelperData *data = d->getWindowDataMutable();
    if (!data) {
        return;
    }
    if (visible) {
        data->hitTestVisibleRects.append(rect);
    } else {
        data->hitTestVisibleRects.removeAll(rect);
    }
}

void FramelessWidgetsHelper::setHitTestVisible(QObject *object, const bool visible)
{
    Q_ASSERT(object);
    if (!object) {
        return;
    }
    const auto widget = qobject_cast<QWidget *>(object);
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    setHitTestVisible(widget, visible);
}

void FramelessWidgetsHelper::extendsContentIntoTitleBar(const bool value)
{
    if (isContentExtendedIntoTitleBar() == value) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    if (value) {
        d->attach();
    } else {
        d->detach();
    }
    d->emitSignalForAllInstances("extendsContentIntoTitleBarChanged");
}

FRAMELESSHELPER_END_NAMESPACE
