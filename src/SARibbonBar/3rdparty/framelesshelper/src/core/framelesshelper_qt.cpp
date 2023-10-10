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

struct FramelessQtHelperData
{
    SystemParameters params = {};
    FramelessHelperQt *eventFilter = nullptr;
    bool cursorShapeChanged = false;
    bool leftButtonPressed = false;
};

using FramelessQtHelperInternal = QHash<WId, FramelessQtHelperData>;

Q_GLOBAL_STATIC(FramelessQtHelperInternal, g_framelessQtHelperData)

FramelessHelperQt::FramelessHelperQt(QObject *parent) : QObject(parent) {}

FramelessHelperQt::~FramelessHelperQt() = default;

void FramelessHelperQt::addWindow(FramelessParamsConst params)
{
    Q_ASSERT(params);
    if (!params) {
        return;
    }
    const WId windowId = params->getWindowId();
    const auto it = g_framelessQtHelperData()->constFind(windowId);
    if (it != g_framelessQtHelperData()->constEnd()) {
        return;
    }
    FramelessQtHelperData data = {};
    data.params = *params;
    QWindow *window = params->getWindowHandle();
    // Give it a parent so that it can be automatically deleted by Qt.
    data.eventFilter = new FramelessHelperQt(window);
    g_framelessQtHelperData()->insert(windowId, data);
    const auto shouldApplyFramelessFlag = []() -> bool {
#ifdef Q_OS_MACOS
        return false;
#elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
        return !Utils::isCustomDecorationSupported();
#elif defined(Q_OS_WINDOWS)
        return true;
#endif // Q_OS_MACOS
    }();
#if (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    window->setProperty("_q_mac_wantsLayer", 1);
#endif // (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    if (shouldApplyFramelessFlag) {
        params->setWindowFlags(params->getWindowFlags() | Qt::FramelessWindowHint);
    } else {
#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
        std::ignore = Utils::tryHideSystemTitleBar(windowId, true);
#elif defined(Q_OS_MACOS)
        Utils::setSystemTitleBarVisible(windowId, false);
#endif // Q_OS_LINUX
    }
    window->installEventFilter(data.eventFilter);
    FramelessHelperEnableThemeAware();
}

void FramelessHelperQt::removeWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return;
    }
    const auto it = g_framelessQtHelperData()->constFind(windowId);
    if (it == g_framelessQtHelperData()->constEnd()) {
        return;
    }
    g_framelessQtHelperData()->erase(it);
#ifdef Q_OS_MACOS
    Utils::removeWindowProxy(windowId);
#endif
}

bool FramelessHelperQt::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    if (!object || !event) {
        return false;
    }
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
    // First detect whether we got a theme change event or not, if so,
    // inform the user the system theme has changed.
    if (Utils::isThemeChangeEvent(event)) {
        // Sometimes the FramelessManager instance may be destroyed already.
        if (FramelessManager * const manager = FramelessManager::instance()) {
            if (FramelessManagerPrivate * const managerPriv = FramelessManagerPrivate::get(manager)) {
                managerPriv->notifySystemThemeHasChangedOrNot();
            }
        }
        return QObject::eventFilter(object, event);
    }
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
    // We are only interested in events that are dispatched to top level windows.
    if (!object->isWindowType()) {
        return QObject::eventFilter(object, event);
    }
    const QEvent::Type type = event->type();
    // We are only interested in some specific mouse events (plus DPR change event).
    if ((type != QEvent::MouseButtonPress) && (type != QEvent::MouseButtonRelease)
            && (type != QEvent::MouseButtonDblClick) && (type != QEvent::MouseMove)
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
            && (type != QEvent::DevicePixelRatioChange)
#else // QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
            && (type != QEvent::ScreenChangeInternal) // Qt's internal event to notify screen change and DPR change.
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
            ) {
        return QObject::eventFilter(object, event);
    }
    const auto window = qobject_cast<QWindow *>(object);
    const WId windowId = window->winId();
    const auto it = g_framelessQtHelperData()->find(windowId);
    if (it == g_framelessQtHelperData()->end()) {
        return QObject::eventFilter(object, event);
    }
    const FramelessQtHelperData &data = it.value();
    FramelessQtHelperData &muData = it.value();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    if (type == QEvent::DevicePixelRatioChange)
#else // QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
    if (type == QEvent::ScreenChangeInternal)
#endif // (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    {
        data.params.forceChildrenRepaint(500);
        return QObject::eventFilter(object, event);
    }
    const auto mouseEvent = static_cast<QMouseEvent *>(event);
    const Qt::MouseButton button = mouseEvent->button();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    const QPoint scenePos = mouseEvent->scenePosition().toPoint();
    const QPoint globalPos = mouseEvent->globalPosition().toPoint();
#else
    const QPoint scenePos = mouseEvent->windowPos().toPoint();
    const QPoint globalPos = mouseEvent->screenPos().toPoint();
#endif
    const bool windowFixedSize = data.params.isWindowFixedSize();
    const bool ignoreThisEvent = data.params.shouldIgnoreMouseEvents(scenePos);
    const bool insideTitleBar = data.params.isInsideTitleBarDraggableArea(scenePos);
    const bool dontOverrideCursor = data.params.getProperty(kDontOverrideCursorVar, false).toBool();
    const bool dontToggleMaximize = data.params.getProperty(kDontToggleMaximizeVar, false).toBool();
    switch (type) {
    case QEvent::MouseButtonPress: {
        if (button == Qt::LeftButton) {
            muData.leftButtonPressed = true;
            if (!windowFixedSize) {
                const Qt::Edges edges = Utils::calculateWindowEdges(window, scenePos);
                if (edges != Qt::Edges{}) {
                    std::ignore = Utils::startSystemResize(window, edges, globalPos);
                    event->accept();
                    return true;
                }
            }
        }
    } break;
    case QEvent::MouseButtonRelease: {
        if (button == Qt::LeftButton) {
            muData.leftButtonPressed = false;
        }
        if (button == Qt::RightButton) {
            if (!ignoreThisEvent && insideTitleBar) {
                data.params.showSystemMenu(globalPos);
                event->accept();
                return true;
            }
        }
    } break;
    case QEvent::MouseButtonDblClick: {
        if (!dontToggleMaximize && (button == Qt::LeftButton) && !windowFixedSize && !ignoreThisEvent && insideTitleBar) {
            Qt::WindowState newWindowState = Qt::WindowNoState;
            if (data.params.getWindowState() != Qt::WindowMaximized) {
                newWindowState = Qt::WindowMaximized;
            }
            data.params.setWindowState(newWindowState);
            event->accept();
            return true;
        }
    } break;
    case QEvent::MouseMove: {
        if (!dontOverrideCursor && !windowFixedSize) {
            const Qt::CursorShape cs = Utils::calculateCursorShape(window, scenePos);
            if (cs == Qt::ArrowCursor) {
                if (data.cursorShapeChanged) {
                    data.params.unsetCursor();
                    muData.cursorShapeChanged = false;
                }
            } else {
                data.params.setCursor(cs);
                muData.cursorShapeChanged = true;
            }
        }
        if (data.leftButtonPressed) {
            if (!ignoreThisEvent && insideTitleBar) {
                std::ignore = Utils::startSystemMove(window, globalPos);
                event->accept();
                return true;
            }
        }
    } break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

FRAMELESSHELPER_END_NAMESPACE
