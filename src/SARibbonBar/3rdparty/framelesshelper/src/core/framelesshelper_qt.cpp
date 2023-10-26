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

#include "framelesshelper_qt.h"

#if !FRAMELESSHELPER_CONFIG(native_impl)

#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include "framelessconfig_p.h"
#include "framelesshelpercore_global_p.h"
#include "utils.h"
#include <QtCore/qloggingcategory.h>
#include <QtGui/qevent.h>
#include <QtGui/qwindow.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessHelperQt, "wangwenx190.framelesshelper.core.impl.qt")
#  define INFO qCInfo(lcFramelessHelperQt)
#  define DEBUG qCDebug(lcFramelessHelperQt)
#  define WARNING qCWarning(lcFramelessHelperQt)
#  define CRITICAL qCCritical(lcFramelessHelperQt)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

struct FramelessDataQt : public FramelessData
{
    FramelessHelperQt *framelessHelperImpl = nullptr;
    bool cursorShapeChanged = false;
    bool leftButtonPressed = false;

    FramelessDataQt();
    ~FramelessDataQt() override;
};
using FramelessDataQtPtr = std::shared_ptr<FramelessDataQt>;

[[nodiscard]] static inline FramelessDataQtPtr tryGetData(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data) {
        return nullptr;
    }
    return std::dynamic_pointer_cast<FramelessDataQt>(data);
}

class FramelessHelperQtPrivate
{
    FRAMELESSHELPER_PRIVATE_CLASS(FramelessHelperQt)

public:
    explicit FramelessHelperQtPrivate(FramelessHelperQt *q);
    ~FramelessHelperQtPrivate();

    const QObject *window = nullptr;
};

FramelessDataQt::FramelessDataQt() = default;

FramelessDataQt::~FramelessDataQt() = default;

[[nodiscard]] FramelessDataPtr FramelessData::create()
{
    return std::make_shared<FramelessDataQt>();
}

FramelessHelperQtPrivate::FramelessHelperQtPrivate(FramelessHelperQt *q) : q_ptr(q)
{
    Q_ASSERT(q_ptr);
}

FramelessHelperQtPrivate::~FramelessHelperQtPrivate() = default;

FramelessHelperQt::FramelessHelperQt(QObject *parent) : QObject(parent), d_ptr(std::make_unique<FramelessHelperQtPrivate>(this))
{
}

FramelessHelperQt::~FramelessHelperQt() = default;

void FramelessHelperQt::addWindow(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    const FramelessDataQtPtr data = tryGetData(window);
    if (!data || data->frameless || !data->callbacks) {
        return;
    }
    QWindow *qWindow = data->callbacks->getWindowHandle();
    Q_ASSERT(qWindow);
    if (!qWindow) {
        return;
    }
    data->frameless = true;
    static const auto shouldApplyFramelessFlag = []() -> bool {
#ifdef Q_OS_MACOS
        return false;
#elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
        return !Utils::isCustomDecorationSupported();
#elif defined(Q_OS_WINDOWS)
        return true;
#endif // Q_OS_MACOS
    }();
#if (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    qWindow->setProperty("_q_mac_wantsLayer", 1);
#endif // (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    if (shouldApplyFramelessFlag) {
        data->callbacks->setWindowFlags(data->callbacks->getWindowFlags() | Qt::FramelessWindowHint);
    } else {
#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
        std::ignore = Utils::tryHideSystemTitleBar(data->callbacks->getWindowId(), true);
#elif defined(Q_OS_MACOS)
        Utils::setSystemTitleBarVisible(data->callbacks->getWindowId(), false);
#endif // Q_OS_LINUX
    }
    if (!data->framelessHelperImpl) {
        data->framelessHelperImpl = new FramelessHelperQt(qWindow);
        data->framelessHelperImpl->d_func()->window = window;
        qWindow->installEventFilter(data->framelessHelperImpl);
    }
    FramelessHelperEnableThemeAware();
}

void FramelessHelperQt::removeWindow(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    const FramelessDataQtPtr data = tryGetData(window);
    if (!data || !data->frameless || !data->callbacks) {
        return;
    }
    if (data->framelessHelperImpl) {
        QWindow *qWindow = data->callbacks->getWindowHandle();
        Q_ASSERT(qWindow);
        if (qWindow) {
            qWindow->removeEventFilter(data->framelessHelperImpl);
            delete data->framelessHelperImpl;
            data->framelessHelperImpl = nullptr;
        }
    }
#ifdef Q_OS_MACOS
    Utils::removeWindowProxy(data->callbacks->getWindowId());
#endif // Q_OS_MACOS
}

bool FramelessHelperQt::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    if (!object || !event) {
        return false;
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
    if (Utils::isThemeChangeEvent(event)) {
        // Sometimes the FramelessManager instance may be destroyed already.
        if (FramelessManager *manager = FramelessManager::instance()) {
            if (FramelessManagerPrivate *managerPriv = FramelessManagerPrivate::get(manager)) {
                managerPriv->notifySystemThemeHasChangedOrNot();
            }
        }
        return false;
    }
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
    Q_D(FramelessHelperQt);
    if (!d->window || !object->isWindowType()) {
        return false;
    }
    const QEvent::Type type = event->type();
    // We are only interested in some specific mouse events (plus the DPR change event).
    if ((type != QEvent::MouseButtonPress) && (type != QEvent::MouseButtonRelease)
            && (type != QEvent::MouseButtonDblClick) && (type != QEvent::MouseMove)
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
            && (type != QEvent::DevicePixelRatioChange)
#else // QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
            && (type != QEvent::ScreenChangeInternal) // Qt's internal event to notify screen change and DPR change.
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
            ) {
        return false;
    }
    const FramelessDataQtPtr data = tryGetData(d->window);
    if (!data || !data->frameless || !data->callbacks) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    if (type == QEvent::DevicePixelRatioChange)
#else // QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
    if (type == QEvent::ScreenChangeInternal)
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    {
        data->callbacks->forceChildrenRepaint();
        return false;
    }
    const auto qWindow = qobject_cast<QWindow *>(object);
    const auto mouseEvent = static_cast<QMouseEvent *>(event);
    const Qt::MouseButton button = mouseEvent->button();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    const QPoint scenePos = mouseEvent->scenePosition().toPoint();
    const QPoint globalPos = mouseEvent->globalPosition().toPoint();
#else
    const QPoint scenePos = mouseEvent->windowPos().toPoint();
    const QPoint globalPos = mouseEvent->screenPos().toPoint();
#endif
    const bool windowFixedSize = data->callbacks->isWindowFixedSize();
    const bool ignoreThisEvent = data->callbacks->shouldIgnoreMouseEvents(scenePos);
    const bool insideTitleBar = data->callbacks->isInsideTitleBarDraggableArea(scenePos);
    const bool dontOverrideCursor = data->callbacks->getProperty(kDontOverrideCursorVar, false).toBool();
    const bool dontToggleMaximize = data->callbacks->getProperty(kDontToggleMaximizeVar, false).toBool();
    switch (type) {
    case QEvent::MouseButtonPress:
        if (button == Qt::LeftButton) {
            data->leftButtonPressed = true;
            if (!windowFixedSize) {
                const Qt::Edges edges = Utils::calculateWindowEdges(qWindow, scenePos);
                if (edges != Qt::Edges{}) {
                    std::ignore = Utils::startSystemResize(qWindow, edges, globalPos);
                    event->accept();
                    return true;
                }
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        if (button == Qt::LeftButton) {
            data->leftButtonPressed = false;
        } else if (button == Qt::RightButton) {
            if (!ignoreThisEvent && insideTitleBar) {
                data->callbacks->showSystemMenu(globalPos);
                event->accept();
                return true;
            }
        }
        break;
    case QEvent::MouseButtonDblClick:
        if (!dontToggleMaximize && (button == Qt::LeftButton) && !windowFixedSize && !ignoreThisEvent && insideTitleBar) {
            Qt::WindowState newWindowState = Qt::WindowNoState;
            if (data->callbacks->getWindowState() != Qt::WindowMaximized) {
                newWindowState = Qt::WindowMaximized;
            }
            data->callbacks->setWindowState(newWindowState);
            event->accept();
            return true;
        }
        break;
    case QEvent::MouseMove: {
        if (!dontOverrideCursor && !windowFixedSize) {
            const Qt::CursorShape cs = Utils::calculateCursorShape(qWindow, scenePos);
            if (cs == Qt::ArrowCursor) {
                if (data->cursorShapeChanged) {
                    data->callbacks->unsetCursor();
                    data->cursorShapeChanged = false;
                }
            } else {
                data->callbacks->setCursor(cs);
                data->cursorShapeChanged = true;
            }
        }
        if (data->leftButtonPressed) {
            if (!ignoreThisEvent && insideTitleBar) {
                std::ignore = Utils::startSystemMove(qWindow, globalPos);
                event->accept();
                return true;
            }
        }
    } break;
    default:
        break;
    }
    return false;
}

FRAMELESSHELPER_END_NAMESPACE

#endif // !native_impl
