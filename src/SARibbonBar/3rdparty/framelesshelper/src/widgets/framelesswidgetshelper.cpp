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
#include <FramelessHelper/Core/private/framelessmanager_p.h>
#include <FramelessHelper/Core/private/framelessconfig_p.h>
#include <FramelessHelper/Core/private/framelesshelpercore_global_p.h>
#include <QtCore/qhash.h>
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

static constexpr const auto kRepaintTimerInterval = 500;

struct FramelessWidgetsHelperExtraData : public FramelessExtraData
{
    QPointer<QWidget> titleBarWidget = nullptr;
    QList<QPointer<QWidget>> hitTestVisibleWidgets = {};
    QPointer<QWidget> windowIconButton = nullptr;
    QPointer<QWidget> contextHelpButton = nullptr;
    QPointer<QWidget> minimizeButton = nullptr;
    QPointer<QWidget> maximizeButton = nullptr;
    QPointer<QWidget> closeButton = nullptr;
    QList<QRect> hitTestVisibleRects = {};

    FramelessWidgetsHelperExtraData();
    ~FramelessWidgetsHelperExtraData() override;

    [[nodiscard]] static FramelessExtraDataPtr create();
};
using FramelessWidgetsHelperExtraDataPtr = std::shared_ptr<FramelessWidgetsHelperExtraData>;

FramelessWidgetsHelperExtraData::FramelessWidgetsHelperExtraData() = default;

FramelessWidgetsHelperExtraData::~FramelessWidgetsHelperExtraData() = default;

FramelessExtraDataPtr FramelessWidgetsHelperExtraData::create()
{
    return std::make_shared<FramelessWidgetsHelperExtraData>();
}

[[nodiscard]] static inline FramelessWidgetsHelperExtraDataPtr tryGetExtraData(const FramelessDataPtr &data, const bool create)
{
    Q_ASSERT(data);
    if (!data) {
        return nullptr;
    }
    auto it = data->extraData.find(ExtraDataType::FramelessWidgetsHelper);
    if (it == data->extraData.end()) {
        if (create) {
            it = data->extraData.insert(ExtraDataType::FramelessWidgetsHelper, FramelessWidgetsHelperExtraData::create());
        } else {
            return nullptr;
        }
    }
    return std::dynamic_pointer_cast<FramelessWidgetsHelperExtraData>(it.value());
}

[[nodiscard]] static inline FramelessWidgetsHelperExtraDataPtr tryGetExtraData(const QWidget *window, const bool create)
{
    Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data) {
        return nullptr;
    }
    return tryGetExtraData(data, create);
}

[[nodiscard]] static inline bool isWidgetFixedSize(const QWidget *widget)
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

static inline void forceWidgetRepaint(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
#if (defined(Q_OS_WINDOWS) && (QT_VERSION < QT_VERSION_CHECK(6, 5, 3)))
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
            static constexpr const auto margins = QMargins{ 1, 1, 1, 1 };
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
    repaintTimer.setTimerType(Qt::VeryCoarseTimer);
    repaintTimer.setInterval(kRepaintTimerInterval);
    connect(&repaintTimer, &QTimer::timeout, this, &FramelessWidgetsHelperPrivate::doRepaintAllChildren);
}

FramelessWidgetsHelperPrivate::~FramelessWidgetsHelperPrivate()
{
    detach();
}

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

void FramelessWidgetsHelperPrivate::repaintAllChildren()
{
    repaintTimer.start();
}

void FramelessWidgetsHelperPrivate::doRepaintAllChildren()
{
    repaintTimer.stop();
    if (repaintedOnce) {
        if (!window) {
            return;
        }
        forceWidgetRepaint(window);
        const QList<QWidget *> widgets = window->findChildren<QWidget *>();
        for (auto &&widget : std::as_const(widgets)) {
            forceWidgetRepaint(widget);
        }
    } else {
        repaintedOnce = true;
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
    QWidget *tlw = findTopLevelWindow();
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

    const WId windowId = window->winId();
    const FramelessDataPtr data = FramelessManagerPrivate::createData(window, windowId);
    Q_ASSERT(data);
    if (!data || data->frameless) {
        return;
    }

    Q_Q(FramelessWidgetsHelper);

    if (!data->callbacks) {
        data->callbacks = FramelessCallbacks::create();
        data->callbacks->getWindowId = [this]() -> WId { return window->winId(); };
        data->callbacks->getWindowFlags = [this]() -> Qt::WindowFlags { return window->windowFlags(); };
        data->callbacks->setWindowFlags = [this](const Qt::WindowFlags flags) -> void { window->setWindowFlags(flags); };
        data->callbacks->getWindowSize = [this]() -> QSize { return window->size(); };
        data->callbacks->setWindowSize = [this](const QSize &size) -> void { window->resize(size); };
        data->callbacks->getWindowPosition = [this]() -> QPoint { return window->pos(); };
        data->callbacks->setWindowPosition = [this](const QPoint &pos) -> void { window->move(pos); };
        data->callbacks->getWindowScreen = [this]() -> QScreen * {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
            return window->screen();
#else
            return window->windowHandle()->screen();
#endif
        };
        data->callbacks->isWindowFixedSize = [q]() -> bool { return q->isWindowFixedSize(); };
        data->callbacks->setWindowFixedSize = [q](const bool value) -> void { q->setWindowFixedSize(value); };
        data->callbacks->getWindowState = [this]() -> Qt::WindowState { return Utils::windowStatesToWindowState(window->windowState()); };
        data->callbacks->setWindowState = [this](const Qt::WindowState state) -> void { window->setWindowState(state); };
        data->callbacks->getWindowHandle = [this]() -> QWindow * { return window->windowHandle(); };
        data->callbacks->windowToScreen = [this](const QPoint &pos) -> QPoint { return window->mapToGlobal(pos); };
        data->callbacks->screenToWindow = [this](const QPoint &pos) -> QPoint { return window->mapFromGlobal(pos); };
        data->callbacks->isInsideSystemButtons = [this](const QPoint &pos, SystemButtonType *button) -> bool { return isInSystemButtons(pos, button); };
        data->callbacks->isInsideTitleBarDraggableArea = [this](const QPoint &pos) -> bool { return isInTitleBarDraggableArea(pos); };
        data->callbacks->getWindowDevicePixelRatio = [this]() -> qreal { return window->devicePixelRatioF(); };
        data->callbacks->setSystemButtonState = [this](const SystemButtonType button, const ButtonState state) -> void { setSystemButtonState(button, state); };
        data->callbacks->shouldIgnoreMouseEvents = [this](const QPoint &pos) -> bool { return shouldIgnoreMouseEvents(pos); };
        data->callbacks->showSystemMenu = [q](const QPoint &pos) -> void { q->showSystemMenu(pos); };
        data->callbacks->setProperty = [this](const char *name, const QVariant &value) -> void { setProperty(name, value); };
        data->callbacks->getProperty = [this](const char *name, const QVariant &defaultValue) -> QVariant { return getProperty(name, defaultValue); };
        data->callbacks->setCursor = [this](const QCursor &cursor) -> void { window->setCursor(cursor); };
        data->callbacks->unsetCursor = [this]() -> void { window->unsetCursor(); };
        data->callbacks->getWidgetHandle = [this]() -> QObject * { return window; };
        data->callbacks->forceChildrenRepaint = [this]() -> void { repaintAllChildren(); };
        data->callbacks->resetQtGrabbedControl = []() -> bool {
            if (qt_button_down) {
                static constexpr const auto invalidPos = QPoint{ -99999, -99999 };
                const auto event = new QMouseEvent(
                    QEvent::MouseButtonRelease,
                    invalidPos,
                    invalidPos,
                    invalidPos,
                    Qt::LeftButton,
                    QGuiApplication::mouseButtons() ^ Qt::LeftButton,
                    QGuiApplication::keyboardModifiers());
                QApplication::postEvent(qt_button_down, event);
                qt_button_down = nullptr;
                return true;
            }
            return false;
        };
    }

    std::ignore = tryGetExtraData(data, true);

    std::ignore = FramelessManager::instance()->addWindow(window, windowId);

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
    std::ignore = FramelessManager::instance()->removeWindow(window);
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
    if (!window) {
        return false;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    *button = SystemButtonType::Unknown;
    if (extraData->windowIconButton && extraData->windowIconButton->isVisible() && extraData->windowIconButton->isEnabled()) {
        if (mapWidgetGeometryToScene(extraData->windowIconButton).contains(pos)) {
            *button = SystemButtonType::WindowIcon;
            return true;
        }
    }
    if (extraData->contextHelpButton && extraData->contextHelpButton->isVisible() && extraData->contextHelpButton->isEnabled()) {
        if (mapWidgetGeometryToScene(extraData->contextHelpButton).contains(pos)) {
            *button = SystemButtonType::Help;
            return true;
        }
    }
    if (extraData->minimizeButton && extraData->minimizeButton->isVisible() && extraData->minimizeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(extraData->minimizeButton).contains(pos)) {
            *button = SystemButtonType::Minimize;
            return true;
        }
    }
    if (extraData->maximizeButton && extraData->maximizeButton->isVisible() && extraData->maximizeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(extraData->maximizeButton).contains(pos)) {
            *button = SystemButtonType::Maximize;
            return true;
        }
    }
    if (extraData->closeButton && extraData->closeButton->isVisible() && extraData->closeButton->isEnabled()) {
        if (mapWidgetGeometryToScene(extraData->closeButton).contains(pos)) {
            *button = SystemButtonType::Close;
            return true;
        }
    }
    return false;
}

bool FramelessWidgetsHelperPrivate::isInTitleBarDraggableArea(const QPoint &pos) const
{
    if (!window) {
        // The FramelessWidgetsHelper object has not been attached to a specific window yet,
        // so we assume there's no title bar.
        return false;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    if (!extraData->titleBarWidget) {
        // There's no title bar at all, the mouse will always be in the client area.
        return false;
    }
    if (!extraData->titleBarWidget->isVisible() || !extraData->titleBarWidget->isEnabled()) {
        // The title bar is hidden or disabled for some reason, treat it as there's no title bar.
        return false;
    }
    const QRect windowRect = {QPoint(0, 0), window->size()};
    const QRect titleBarRect = mapWidgetGeometryToScene(extraData->titleBarWidget);
    if (!titleBarRect.intersects(windowRect)) {
        // The title bar is totally outside of the window for some reason,
        // also treat it as there's no title bar.
        return false;
    }
    QRegion region = titleBarRect;
    const auto systemButtons = {
        extraData->windowIconButton, extraData->contextHelpButton,
        extraData->minimizeButton, extraData->maximizeButton,
        extraData->closeButton
    };
    for (auto &&button : std::as_const(systemButtons)) {
        if (button && button->isVisible() && button->isEnabled()) {
            region -= mapWidgetGeometryToScene(button);
        }
    }
    if (!extraData->hitTestVisibleWidgets.isEmpty()) {
        for (auto &&widget : std::as_const(extraData->hitTestVisibleWidgets)) {
            if (widget && widget->isVisible() && widget->isEnabled()) {
                region -= mapWidgetGeometryToScene(widget);
            }
        }
    }
    if (!extraData->hitTestVisibleRects.isEmpty()) {
        for (auto &&rect : std::as_const(extraData->hitTestVisibleRects)) {
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
    std::ignore = Utils::moveWindowToDesktopCenter(d->window->winId(), true);
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
    std::ignore = Utils::showSystemMenu(windowId, nativePos, false);
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
    if (!d->window) {
        return;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(d->window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return;
    }
    switch (buttonType) {
    case SystemButtonType::WindowIcon:
        extraData->windowIconButton = widget;
        break;
    case SystemButtonType::Help:
        extraData->contextHelpButton = widget;
        break;
    case SystemButtonType::Minimize:
        extraData->minimizeButton = widget;
        break;
    case SystemButtonType::Maximize:
    case SystemButtonType::Restore:
        extraData->maximizeButton = widget;
        break;
    case SystemButtonType::Close:
        extraData->closeButton = widget;
        break;
    case SystemButtonType::Unknown:
        Q_UNREACHABLE();
    }
}

FramelessWidgetsHelper::FramelessWidgetsHelper(QObject *parent)
    : QObject(parent), d_ptr(std::make_unique<FramelessWidgetsHelperPrivate>(this))
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
#if (!defined(Q_OS_WINDOWS) || FRAMELESSHELPER_CONFIG(native_impl))
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
#else // Windows && !native_impl
    Q_UNUSED(enable);
#endif
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
    if (!d->window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(d->window);
    return (data && data->frameless);
}

void FramelessWidgetsHelper::setTitleBarWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(d->window, false);
    Q_ASSERT(extraData);
    if (!extraData || (extraData->titleBarWidget == widget)) {
        return;
    }
    extraData->titleBarWidget = widget;
    d->emitSignalForAllInstances("titleBarWidgetChanged");
}

QWidget *FramelessWidgetsHelper::titleBarWidget() const
{
    Q_D(const FramelessWidgetsHelper);
    if (!d->window) {
        return nullptr;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(d->window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return nullptr;
    }
    return extraData->titleBarWidget;
}

void FramelessWidgetsHelper::setHitTestVisible(QWidget *widget, const bool visible)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(d->window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return;
    }
    if (visible) {
        extraData->hitTestVisibleWidgets.append(widget);
    } else {
        extraData->hitTestVisibleWidgets.removeAll(widget);
    }
}

void FramelessWidgetsHelper::setHitTestVisible(const QRect &rect, const bool visible)
{
    Q_ASSERT(Utils::isValidGeometry(rect));
    if (!Utils::isValidGeometry(rect)) {
        return;
    }
    Q_D(FramelessWidgetsHelper);
    if (!d->window) {
        return;
    }
    const FramelessWidgetsHelperExtraDataPtr extraData = tryGetExtraData(d->window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return;
    }
    if (visible) {
        extraData->hitTestVisibleRects.append(rect);
    } else {
        extraData->hitTestVisibleRects.removeAll(rect);
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
