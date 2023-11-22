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

#include "framelessquickhelper.h"
#include "framelessquickhelper_p.h"
#if FRAMELESSHELPER_CONFIG(mica_material)
#  include "quickmicamaterial.h"
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
#  include "quickwindowborder.h"
#endif
#include <FramelessHelper/Core/framelessmanager.h>
#include <FramelessHelper/Core/utils.h>
#include <FramelessHelper/Core/private/framelessmanager_p.h>
#include <FramelessHelper/Core/private/framelessconfig_p.h>
#include <FramelessHelper/Core/private/framelesshelpercore_global_p.h>
#ifdef Q_OS_WINDOWS
#  include <FramelessHelper/Core/private/winverhelper_p.h>
#endif // Q_OS_WINDOWS
#include <QtCore/qeventloop.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qcursor.h>
#include <QtGui/qguiapplication.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#    include <QtGui/qpa/qplatformwindow.h> // For QWINDOWSIZE_MAX
#  else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#    include <QtGui/private/qwindow_p.h> // For QWINDOWSIZE_MAX
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#  include <QtQuick/private/qquickitem_p.h>
#endif

#ifndef QWINDOWSIZE_MAX
#  define QWINDOWSIZE_MAX ((1 << 24) - 1)
#endif // QWINDOWSIZE_MAX

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessQuickHelper, "wangwenx190.framelesshelper.quick.framelessquickhelper")
#  define INFO qCInfo(lcFramelessQuickHelper)
#  define DEBUG qCDebug(lcFramelessQuickHelper)
#  define WARNING qCWarning(lcFramelessQuickHelper)
#  define CRITICAL qCCritical(lcFramelessQuickHelper)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static constexpr const auto kRepaintTimerInterval = 300;

struct FramelessQuickHelperExtraData : public FramelessExtraData
{
    QPointer<QQuickItem> titleBarItem = nullptr;
    QList<QPointer<QQuickItem>> hitTestVisibleItems = {};
    QPointer<QQuickItem> windowIconButton = nullptr;
    QPointer<QQuickItem> contextHelpButton = nullptr;
    QPointer<QQuickItem> minimizeButton = nullptr;
    QPointer<QQuickItem> maximizeButton = nullptr;
    QPointer<QQuickItem> closeButton = nullptr;
    QList<QRect> hitTestVisibleRects = {};

    FramelessQuickHelperExtraData();
    ~FramelessQuickHelperExtraData() override;

    [[nodiscard]] static FramelessExtraDataPtr create();
};
using FramelessQuickHelperExtraDataPtr = std::shared_ptr<FramelessQuickHelperExtraData>;

FramelessQuickHelperExtraData::FramelessQuickHelperExtraData() = default;

FramelessQuickHelperExtraData::~FramelessQuickHelperExtraData() = default;

FramelessExtraDataPtr FramelessQuickHelperExtraData::create()
{
    return std::make_shared<FramelessQuickHelperExtraData>();
}

[[nodiscard]] static inline FramelessQuickHelperExtraDataPtr tryGetExtraData(const FramelessDataPtr &data, const bool create)
{
    Q_ASSERT(data);
    if (!data) {
        return nullptr;
    }
    auto it = data->extraData.find(ExtraDataType::FramelessQuickHelper);
    if (it == data->extraData.end()) {
        if (create) {
            it = data->extraData.insert(ExtraDataType::FramelessQuickHelper, FramelessQuickHelperExtraData::create());
        } else {
            return nullptr;
        }
    }
    return std::dynamic_pointer_cast<FramelessQuickHelperExtraData>(it.value());
}

[[nodiscard]] static inline FramelessQuickHelperExtraDataPtr tryGetExtraData(const QQuickWindow *window, const bool create)
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

FramelessQuickHelperPrivate::FramelessQuickHelperPrivate(FramelessQuickHelper *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    repaintTimer.setTimerType(Qt::VeryCoarseTimer);
    repaintTimer.setInterval(kRepaintTimerInterval);
    connect(&repaintTimer, &QTimer::timeout, this, &FramelessQuickHelperPrivate::doRepaintAllChildren);
    // Workaround a MOC limitation: we can't emit a signal from the parent class.
    connect(q_ptr, &FramelessQuickHelper::windowChanged, q_ptr, &FramelessQuickHelper::windowChanged2);
}

FramelessQuickHelperPrivate::~FramelessQuickHelperPrivate()
{
    detach();
}

FramelessQuickHelperPrivate *FramelessQuickHelperPrivate::get(FramelessQuickHelper *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const FramelessQuickHelperPrivate *FramelessQuickHelperPrivate::get(const FramelessQuickHelper *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

void FramelessQuickHelperPrivate::attach()
{
    Q_Q(FramelessQuickHelper);
    QQuickWindow *window = q->window();
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    const WId windowId = window->winId();

    const FramelessDataPtr data = FramelessManagerPrivate::createData(window, windowId);
    Q_ASSERT(data);
    if (!data || data->frameless) {
        return;
    }

    if (!data->callbacks) {
        QPointer<QWindow> w = window;
        data->callbacks = FramelessCallbacks::create();
        data->callbacks->getWindowId = [window]() -> WId { return window->winId(); };
        data->callbacks->getWindowFlags = [window]() -> Qt::WindowFlags { return window->flags(); };
        data->callbacks->setWindowFlags = [window](const Qt::WindowFlags flags) -> void { window->setFlags(flags); };
        data->callbacks->getWindowSize = [window]() -> QSize { return window->size(); };
        data->callbacks->setWindowSize = [window](const QSize &size) -> void { window->resize(size); };
        data->callbacks->getWindowPosition = [window]() -> QPoint { return window->position(); };
        data->callbacks->setWindowPosition = [window](const QPoint &pos) -> void { window->setX(pos.x()); window->setY(pos.y()); };
        data->callbacks->getWindowScreen = [window]() -> QScreen * { return window->screen(); };
        data->callbacks->isWindowFixedSize = [q]() -> bool { return q->isWindowFixedSize(); };
        data->callbacks->setWindowFixedSize = [q](const bool value) -> void { q->setWindowFixedSize(value); };
        data->callbacks->getWindowState = [window]() -> Qt::WindowState { return window->windowState(); };
        data->callbacks->setWindowState = [window](const Qt::WindowState state) -> void { window->setWindowState(state); };
        data->callbacks->getWindowHandle = [w]() -> QWindow * { return w; };
        data->callbacks->windowToScreen = [window](const QPoint &pos) -> QPoint { return window->mapToGlobal(pos); };
        data->callbacks->screenToWindow = [window](const QPoint &pos) -> QPoint { return window->mapFromGlobal(pos); };
        data->callbacks->isInsideSystemButtons = [this](const QPoint &pos, SystemButtonType *button) -> bool {
            QuickGlobal::SystemButtonType button2 = QuickGlobal::SystemButtonType::Unknown;
            const bool result = isInSystemButtons(pos, &button2);
            *button = FRAMELESSHELPER_ENUM_QUICK_TO_CORE(SystemButtonType, button2);
            return result;
        };
        data->callbacks->isInsideTitleBarDraggableArea = [this](const QPoint &pos) -> bool { return isInTitleBarDraggableArea(pos); };
        data->callbacks->getWindowDevicePixelRatio = [window]() -> qreal { return window->effectiveDevicePixelRatio(); };
        data->callbacks->setSystemButtonState = [this](const SystemButtonType button, const ButtonState state) -> void {
            setSystemButtonState(FRAMELESSHELPER_ENUM_CORE_TO_QUICK(SystemButtonType, button), FRAMELESSHELPER_ENUM_CORE_TO_QUICK(ButtonState, state));
        };
        data->callbacks->shouldIgnoreMouseEvents = [this](const QPoint &pos) -> bool { return shouldIgnoreMouseEvents(pos); };
        data->callbacks->showSystemMenu = [q](const QPoint &pos) -> void { q->showSystemMenu(pos); };
        data->callbacks->setProperty = [this](const char *name, const QVariant &value) -> void { setProperty(name, value); };
        data->callbacks->getProperty = [this](const char *name, const QVariant &defaultValue) -> QVariant { return getProperty(name, defaultValue); };
        data->callbacks->setCursor = [window](const QCursor &cursor) -> void { window->setCursor(cursor); };
        data->callbacks->unsetCursor = [window]() -> void { window->unsetCursor(); };
        data->callbacks->getWidgetHandle = []() -> QObject * { return nullptr; };
        data->callbacks->forceChildrenRepaint = [this]() -> void { repaintAllChildren(); };
        data->callbacks->resetQtGrabbedControl = []() -> bool { return false; };
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
        emitSignalForAllInstances("ready");
    });
}

void FramelessQuickHelperPrivate::detach()
{
    Q_Q(FramelessQuickHelper);
    const QQuickWindow *window = q->window();
    if (!window) {
        return;
    }
    std::ignore = FramelessManager::instance()->removeWindow(window);
}

void FramelessQuickHelperPrivate::emitSignalForAllInstances(const char *signal)
{
    Q_ASSERT(signal);
    Q_ASSERT(*signal != '\0');
    if (!signal || (*signal == '\0')) {
        return;
    }
    Q_Q(FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    const auto rootObject = (window->contentItem() ?
            qobject_cast<const QObject *>(window->contentItem())
            : qobject_cast<const QObject *>(window));
    const auto instances = rootObject->findChildren<FramelessQuickHelper *>();
    if (instances.isEmpty()) {
        return;
    }
    for (auto &&instance : std::as_const(instances)) {
        QMetaObject::invokeMethod(instance, signal);
    }
}

void FramelessQuickHelperPrivate::setProperty(const char *name, const QVariant &value)
{
    Q_ASSERT(name);
    Q_ASSERT(*name != '\0');
    Q_ASSERT(value.isValid());
    Q_ASSERT(!value.isNull());
    if (!name || (*name == '\0') || !value.isValid() || value.isNull()) {
        return;
    }
    Q_Q(FramelessQuickHelper);
    QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    window->setProperty(name, value);
}

QVariant FramelessQuickHelperPrivate::getProperty(const char *name, const QVariant &defaultValue)
{
    Q_ASSERT(name);
    Q_ASSERT(*name != '\0');
    if (!name || (*name == '\0')) {
        return {};
    }
    Q_Q(FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return {};
    }
    const QVariant value = window->property(name);
    return ((value.isValid() && !value.isNull()) ? value : defaultValue);
}

#if FRAMELESSHELPER_CONFIG(mica_material)
QuickMicaMaterial *FramelessQuickHelperPrivate::findOrCreateMicaMaterial() const
{
    Q_Q(const FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return nullptr;
    }
    QQuickItem * const rootItem = window->contentItem();
    if (const auto item = rootItem->findChild<QuickMicaMaterial *>()) {
        return item;
    }
    if (const auto item = window->findChild<QuickMicaMaterial *>()) {
        return item;
    }
    const auto item = new QuickMicaMaterial;
    item->setParent(rootItem);
    item->setParentItem(rootItem);
    item->setZ(-999); // Make sure it always stays on the bottom.
#if FRAMELESSHELPER_CONFIG(private_qt)
    QQuickItemPrivate::get(item)->anchors()->setFill(rootItem);
#endif
    return item;
}
#endif

#if FRAMELESSHELPER_CONFIG(border_painter)
QuickWindowBorder *FramelessQuickHelperPrivate::findOrCreateWindowBorder() const
{
    Q_Q(const FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return nullptr;
    }
    QQuickItem * const rootItem = window->contentItem();
    if (const auto item = rootItem->findChild<QuickWindowBorder *>()) {
        return item;
    }
    if (const auto item = window->findChild<QuickWindowBorder *>()) {
        return item;
    }
    const auto item = new QuickWindowBorder;
    item->setParent(rootItem);
    item->setParentItem(rootItem);
    item->setZ(999); // Make sure it always stays on the top.
#if FRAMELESSHELPER_CONFIG(private_qt)
    QQuickItemPrivate::get(item)->anchors()->setFill(rootItem);
#endif
    return item;
}
#endif

FramelessQuickHelper *FramelessQuickHelperPrivate::findOrCreateFramelessHelper(QObject *object)
{
    Q_ASSERT(object);
    if (!object) {
        return nullptr;
    }
    QObject *parent = nullptr;
    QQuickItem *parentItem = nullptr;
    if (const auto objWindow = qobject_cast<QQuickWindow *>(object)) {
        if (QQuickItem * const item = objWindow->contentItem()) {
            parent = item;
            parentItem = item;
        } else {
            parent = objWindow;
        }
    } else if (const auto item = qobject_cast<QQuickItem *>(object)) {
        if (QQuickWindow * const itemWindow = item->window()) {
            if (QQuickItem * const contentItem = itemWindow->contentItem()) {
                parent = contentItem;
                parentItem = contentItem;
            } else {
                parent = itemWindow;
                parentItem = item;
            }
        } else {
            parent = item;
            parentItem = item;
        }
    } else {
        parent = object;
    }
    FramelessQuickHelper *instance = parent->findChild<FramelessQuickHelper *>();
    if (!instance) {
        instance = new FramelessQuickHelper;
        instance->setParentItem(parentItem);
        instance->setParent(parent);
        // No need to do this here, we'll do it once the item has been assigned to a specific window.
        //instance->extendsContentIntoTitleBar();
    }
    return instance;
}

void FramelessQuickHelperPrivate::repaintAllChildren()
{
    repaintTimer.start();
}

void FramelessQuickHelperPrivate::doRepaintAllChildren()
{
    repaintTimer.stop();
    Q_Q(const FramelessQuickHelper);
    QQuickWindow *window = q->window();
    if (!window) {
        return;
    }
#if (defined(Q_OS_WINDOWS) && (QT_VERSION != QT_VERSION_CHECK(6, 5, 3)) && (QT_VERSION != QT_VERSION_CHECK(6, 6, 0)))
    // Sync the internal window frame margins with the latest DPI, otherwise
    // we will get wrong window sizes after the DPI change.
    std::ignore = Utils::updateInternalWindowFrameMargins(window, true);
#endif // Q_OS_WINDOWS
    // No need to repaint the window when it's hidden.
    if (!window->isVisible()) {
        return;
    }
    if (!((window->windowState() & (Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen)) || q->isWindowFixedSize())) {
        const QSize originalSize = window->size();
        static constexpr const auto margins = QMargins{ 1, 1, 1, 1 };
        window->resize(originalSize.shrunkBy(margins));
        window->resize(originalSize.grownBy(margins));
        window->resize(originalSize);
    }
    window->requestUpdate();
#if 0 // Calling QWindow::requestUpdate() should be enough.
    const QList<QQuickItem *> items = window->findChildren<QQuickItem *>();
    for (auto &&item : std::as_const(items)) {
        // Only items with the "QQuickItem::ItemHasContents" flag enabled are allowed to call "update()".
        // And don't repaint the item if it's hidden.
        if ((item->flags() & QQuickItem::ItemHasContents) && item->isVisible()) {
            item->update();
        }
    }
#endif
}

quint32 FramelessQuickHelperPrivate::readyWaitTime() const
{
    return qpaWaitTime;
}

void FramelessQuickHelperPrivate::setReadyWaitTime(const quint32 time)
{
    if (qpaWaitTime == time) {
        return;
    }
    qpaWaitTime = time;
}

QRect FramelessQuickHelperPrivate::mapItemGeometryToScene(const QQuickItem * const item) const
{
    Q_ASSERT(item);
    if (!item) {
        return {};
    }
    const QPointF originPoint = item->mapToScene(QPointF(0.0, 0.0));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const QSizeF size = item->size();
#else
    const QSizeF size = {item->width(), item->height()};
#endif
    return QRectF(originPoint, size).toRect();
}

bool FramelessQuickHelperPrivate::isInSystemButtons(const QPoint &pos, QuickGlobal::SystemButtonType *button) const
{
    Q_ASSERT(button);
    if (!button) {
        return false;
    }
    Q_Q(const FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return false;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    *button = QuickGlobal::SystemButtonType::Unknown;
    if (extraData->windowIconButton && extraData->windowIconButton->isVisible() && extraData->windowIconButton->isEnabled()) {
        if (mapItemGeometryToScene(extraData->windowIconButton).contains(pos)) {
            *button = QuickGlobal::SystemButtonType::WindowIcon;
            return true;
        }
    }
    if (extraData->contextHelpButton && extraData->contextHelpButton->isVisible() && extraData->contextHelpButton->isEnabled()) {
        if (mapItemGeometryToScene(extraData->contextHelpButton).contains(pos)) {
            *button = QuickGlobal::SystemButtonType::Help;
            return true;
        }
    }
    if (extraData->minimizeButton && extraData->minimizeButton->isVisible() && extraData->minimizeButton->isEnabled()) {
        if (mapItemGeometryToScene(extraData->minimizeButton).contains(pos)) {
            *button = QuickGlobal::SystemButtonType::Minimize;
            return true;
        }
    }
    if (extraData->maximizeButton && extraData->maximizeButton->isVisible() && extraData->maximizeButton->isEnabled()) {
        if (mapItemGeometryToScene(extraData->maximizeButton).contains(pos)) {
            *button = QuickGlobal::SystemButtonType::Maximize;
            return true;
        }
    }
    if (extraData->closeButton && extraData->closeButton->isVisible() && extraData->closeButton->isEnabled()) {
        if (mapItemGeometryToScene(extraData->closeButton).contains(pos)) {
            *button = QuickGlobal::SystemButtonType::Close;
            return true;
        }
    }
    return false;
}

bool FramelessQuickHelperPrivate::isInTitleBarDraggableArea(const QPoint &pos) const
{
    Q_Q(const FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        // The FramelessQuickHelper item has not been attached to a specific window yet,
        // so we assume there's no title bar.
        return false;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(window, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    if (!extraData->titleBarItem) {
        // There's no title bar at all, the mouse will always be in the client area.
        return false;
    }
    if (!extraData->titleBarItem->isVisible() || !extraData->titleBarItem->isEnabled()) {
        // The title bar is hidden or disabled for some reason, treat it as there's no title bar.
        return false;
    }
    const QRect windowRect = {QPoint(0, 0), window->size()};
    const QRect titleBarRect = mapItemGeometryToScene(extraData->titleBarItem);
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
            region -= mapItemGeometryToScene(button);
        }
    }
    if (!extraData->hitTestVisibleItems.isEmpty()) {
        for (auto &&item : std::as_const(extraData->hitTestVisibleItems)) {
            if (item && item->isVisible() && item->isEnabled()) {
                region -= mapItemGeometryToScene(item);
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

bool FramelessQuickHelperPrivate::shouldIgnoreMouseEvents(const QPoint &pos) const
{
    Q_Q(const FramelessQuickHelper);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return false;
    }
    const auto withinFrameBorder = [q, &pos, window]() -> bool {
        if (q->isWindowFixedSize()) {
            return false;
        }
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
    return ((window->visibility() == QQuickWindow::Windowed) && withinFrameBorder);
}

void FramelessQuickHelperPrivate::setSystemButtonState(const QuickGlobal::SystemButtonType button,
                                                       const QuickGlobal::ButtonState state)
{
    Q_UNUSED(button);
    Q_UNUSED(state);
}

void FramelessQuickHelperPrivate::rebindWindow()
{
    Q_Q(FramelessQuickHelper);
    QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    QQuickItem * const rootItem = window->contentItem();
    const QObject * const p = q->parent();
    const QQuickItem * const pItem = q->parentItem();
    if (rootItem) {
        if ((pItem != rootItem) || (p != rootItem)) {
            q->setParentItem(rootItem);
            q->setParent(rootItem);
        }
    } else {
        if (pItem != nullptr) {
            q->setParentItem(nullptr);
        }
        if (p != window) {
            q->setParent(window);
        }
    }
    if (extendIntoTitleBar.value_or(true)) {
        q->extendsContentIntoTitleBar(true);
    }
}

FramelessQuickHelper::FramelessQuickHelper(QQuickItem *parent)
    : QQuickItem(parent), d_ptr(std::make_unique<FramelessQuickHelperPrivate>(this))
{
}

FramelessQuickHelper::~FramelessQuickHelper() = default;

FramelessQuickHelper *FramelessQuickHelper::get(QObject *object)
{
    Q_ASSERT(object);
    if (!object) {
        return nullptr;
    }
    return FramelessQuickHelperPrivate::findOrCreateFramelessHelper(object);
}

FramelessQuickHelper *FramelessQuickHelper::qmlAttachedProperties(QObject *parentObject)
{
    Q_ASSERT(parentObject);
    if (!parentObject) {
        return nullptr;
    }
    return get(parentObject);
}

#if FRAMELESSHELPER_CONFIG(mica_material)
QuickMicaMaterial *FramelessQuickHelper::micaMaterial() const
{
    Q_D(const FramelessQuickHelper);
    return d->findOrCreateMicaMaterial();
}
#endif

#if FRAMELESSHELPER_CONFIG(border_painter)
QuickWindowBorder *FramelessQuickHelper::windowBorder() const
{
    Q_D(const FramelessQuickHelper);
    return d->findOrCreateWindowBorder();
}
#endif

void FramelessQuickHelper::setHitTestVisible(QQuickItem *item, const bool visible)
{
    setHitTestVisible_item(item, visible);
}

void FramelessQuickHelper::setHitTestVisible_item(QQuickItem *item, const bool visible)
{
    Q_ASSERT(item);
    if (!item) {
        return;
    }
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(w, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return;
    }
    if (visible) {
        extraData->hitTestVisibleItems.append(item);
    } else {
        extraData->hitTestVisibleItems.removeAll(item);
    }
}

void FramelessQuickHelper::setHitTestVisible_rect(const QRect &rect, const bool visible)
{
    Q_ASSERT(Utils::isValidGeometry(rect));
    if (!Utils::isValidGeometry(rect)) {
        return;
    }
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(w, false);
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

void FramelessQuickHelper::setHitTestVisible_object(QObject *object, const bool visible)
{
    Q_ASSERT(object);
    if (!object) {
        return;
    }
    const auto item = qobject_cast<QQuickItem *>(object);
    Q_ASSERT(item);
    if (!item) {
        return;
    }
    setHitTestVisible_item(item, visible);
}

bool FramelessQuickHelper::isContentExtendedIntoTitleBar() const
{
    const QQuickWindow * const w = window();
    if (!w) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(w);
    return (data && data->frameless);
}

void FramelessQuickHelper::extendsContentIntoTitleBar(const bool value)
{
    if (isContentExtendedIntoTitleBar() == value) {
        return;
    }
    Q_D(FramelessQuickHelper);
    if (value) {
        d->attach();
    } else {
        d->detach();
    }
    d->extendIntoTitleBar = value;
    d->emitSignalForAllInstances("extendsContentIntoTitleBarChanged");
}

QQuickItem *FramelessQuickHelper::titleBarItem() const
{
    const QQuickWindow * const w = window();
    if (!w) {
        return nullptr;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(w, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return nullptr;
    }
    return extraData->titleBarItem;
}

void FramelessQuickHelper::setTitleBarItem(QQuickItem *value)
{
    Q_ASSERT(value);
    if (!value) {
        return;
    }
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(w, false);
    Q_ASSERT(extraData);
    if (!extraData || (extraData->titleBarItem == value)) {
        return;
    }
    extraData->titleBarItem = value;
    Q_D(FramelessQuickHelper);
    d->emitSignalForAllInstances("titleBarItemChanged");
}

void FramelessQuickHelper::setSystemButton(QQuickItem *item, const QuickGlobal::SystemButtonType buttonType)
{
    Q_ASSERT(item);
    Q_ASSERT(buttonType != QuickGlobal::SystemButtonType::Unknown);
    if (!item || (buttonType == QuickGlobal::SystemButtonType::Unknown)) {
        return;
    }
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    const FramelessQuickHelperExtraDataPtr extraData = tryGetExtraData(w, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return;
    }
    switch (buttonType) {
    case QuickGlobal::SystemButtonType::WindowIcon:
        extraData->windowIconButton = item;
        break;
    case QuickGlobal::SystemButtonType::Help:
        extraData->contextHelpButton = item;
        break;
    case QuickGlobal::SystemButtonType::Minimize:
        extraData->minimizeButton = item;
        break;
    case QuickGlobal::SystemButtonType::Maximize:
    case QuickGlobal::SystemButtonType::Restore:
        extraData->maximizeButton = item;
        break;
    case QuickGlobal::SystemButtonType::Close:
        extraData->closeButton = item;
        break;
    case QuickGlobal::SystemButtonType::Unknown:
        Q_UNREACHABLE();
    }
}

void FramelessQuickHelper::showSystemMenu(const QPoint &pos)
{
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    const WId windowId = w->winId();
    const QPoint nativePos = Utils::toNativeGlobalPosition(w, pos);
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::showSystemMenu(windowId, nativePos, false);
#elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    Utils::openSystemMenu(windowId, nativePos);
#else
    Q_UNUSED(windowId);
    Q_UNUSED(nativePos);
#endif
}

void FramelessQuickHelper::windowStartSystemMove2(const QPoint &pos)
{
    QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    std::ignore = Utils::startSystemMove(w, pos);
}

void FramelessQuickHelper::windowStartSystemResize2(const Qt::Edges edges, const QPoint &pos)
{
    QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    if (edges == Qt::Edges{}) {
        return;
    }
    std::ignore = Utils::startSystemResize(w, edges, pos);
}

void FramelessQuickHelper::moveWindowToDesktopCenter()
{
    const QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    std::ignore = Utils::moveWindowToDesktopCenter(w->winId(), true);
}

void FramelessQuickHelper::bringWindowToFront()
{
    QQuickWindow * const w = window();
    if (!w) {
        return;
    }
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::bringWindowToFront(w->winId());
#else
    if (w->visibility() == QQuickWindow::Hidden) {
        w->show();
    }
    if (w->visibility() == QQuickWindow::Minimized) {
#  if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        w->setWindowStates(w->windowStates() & ~Qt::WindowMinimized);
#  else
        w->showNormal();
#  endif
    }
    w->raise();
    w->requestActivate();
#endif
}

bool FramelessQuickHelper::isWindowFixedSize() const
{
    const QQuickWindow * const w = window();
    if (!w) {
        return false;
    }
    if (w->flags() & Qt::MSWindowsFixedSizeDialogHint) {
        return true;
    }
    const QSize minSize = w->minimumSize();
    const QSize maxSize = w->maximumSize();
    if (!minSize.isEmpty() && !maxSize.isEmpty() && (minSize == maxSize)) {
        return true;
    }
    return false;
}

void FramelessQuickHelper::setWindowFixedSize(const bool value)
{
    QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    if (isWindowFixedSize() == value) {
        return;
    }
    if (value) {
        const QSize size = w->size();
        w->setMinimumSize(size);
        w->setMaximumSize(size);
    } else {
        w->setMinimumSize(kDefaultWindowSize);
        w->setMaximumSize(QSize(QWINDOWSIZE_MAX, QWINDOWSIZE_MAX));
    }
#ifdef Q_OS_WINDOWS
    std::ignore = Utils::setAeroSnappingEnabled(w->winId(), !value);
#endif
    Q_D(FramelessQuickHelper);
    d->emitSignalForAllInstances("windowFixedSizeChanged");
}

bool FramelessQuickHelper::isBlurBehindWindowEnabled() const
{
    Q_D(const FramelessQuickHelper);
    return d->blurBehindWindowEnabled;
}

void FramelessQuickHelper::setBlurBehindWindowEnabled(const bool value)
{
#if (!defined(Q_OS_WINDOWS) || FRAMELESSHELPER_CONFIG(native_impl))
    QQuickWindow * const w = window();
    if (!w) {
        return;
    }
    Q_D(FramelessQuickHelper);
    if (d->blurBehindWindowEnabled == value) {
        return;
    }
    if (Utils::isBlurBehindWindowSupported()) {
        QuickGlobal::BlurMode mode = QuickGlobal::BlurMode::Disable;
        if (value) {
            if (!d->savedWindowBackgroundColor.isValid()) {
                d->savedWindowBackgroundColor = w->color();
            }
            w->setColor(kDefaultTransparentColor);
            mode = QuickGlobal::BlurMode::Default;
        } else {
            if (d->savedWindowBackgroundColor.isValid()) {
                w->setColor(d->savedWindowBackgroundColor);
                d->savedWindowBackgroundColor = {};
            }
            mode = QuickGlobal::BlurMode::Disable;
        }
        if (Utils::setBlurBehindWindowEnabled(w->winId(), FRAMELESSHELPER_ENUM_QUICK_TO_CORE(BlurMode, mode), {})) {
            d->blurBehindWindowEnabled = value;
            d->emitSignalForAllInstances("blurBehindWindowEnabledChanged");
        } else {
            WARNING << "Failed to enable/disable blur behind window.";
        }
    } else {
        d->blurBehindWindowEnabled = value;
#if FRAMELESSHELPER_CONFIG(mica_material)
        micaMaterial()->setVisible(d->blurBehindWindowEnabled);
#endif
        d->emitSignalForAllInstances("blurBehindWindowEnabledChanged");
    }
#else // Windows && !native_impl
    Q_UNUSED(value);
#endif
}

bool FramelessQuickHelper::isReady() const
{
    Q_D(const FramelessQuickHelper);
    return d->qpaReady;
}

void FramelessQuickHelper::waitForReady()
{
    Q_D(FramelessQuickHelper);
    if (d->qpaReady) {
        return;
    }
#if 1
    QEventLoop loop{};
    const QMetaObject::Connection connection = connect(this, &FramelessQuickHelper::ready, &loop, &QEventLoop::quit);
    loop.exec();
    disconnect(connection);
#else
    while (!d->qpaReady) {
        QCoreApplication::processEvents();
    }
#endif
}

void FramelessQuickHelper::itemChange(const ItemChange change, const ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);
    if ((change == ItemSceneChange) && value.window) {
        Q_D(FramelessQuickHelper);
        d->rebindWindow();
    }
}

void FramelessQuickHelper::classBegin()
{
    QQuickItem::classBegin();
}

void FramelessQuickHelper::componentComplete()
{
    QQuickItem::componentComplete();
}

FRAMELESSHELPER_END_NAMESPACE
