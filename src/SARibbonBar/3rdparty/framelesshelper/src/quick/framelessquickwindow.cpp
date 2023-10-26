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

#include "framelessquickwindow_p.h"
#include "framelessquickwindow_p_p.h"

#if (FRAMELESSHELPER_CONFIG(private_qt) && FRAMELESSHELPER_CONFIG(window))

#include "framelessquickhelper.h"
#if FRAMELESSHELPER_CONFIG(border_painter)
#  include "quickwindowborder.h"
#endif
#ifdef Q_OS_WINDOWS
#  include <FramelessHelper/Core/framelesshelper_windows.h>
#endif // Q_OS_WINDOWS
#include <QtCore/qloggingcategory.h>
#include <QtQuick/private/qquickitem_p.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessQuickWindow, "wangwenx190.framelesshelper.quick.framelessquickwindow")
#  define INFO qCInfo(lcFramelessQuickWindow)
#  define DEBUG qCDebug(lcFramelessQuickWindow)
#  define WARNING qCWarning(lcFramelessQuickWindow)
#  define CRITICAL qCCritical(lcFramelessQuickWindow)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

FramelessQuickWindowPrivate::FramelessQuickWindowPrivate(FramelessQuickWindow *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
}

FramelessQuickWindowPrivate::~FramelessQuickWindowPrivate() = default;

FramelessQuickWindowPrivate *FramelessQuickWindowPrivate::get(FramelessQuickWindow *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const FramelessQuickWindowPrivate *FramelessQuickWindowPrivate::get(const FramelessQuickWindow *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

FramelessQuickWindow::FramelessQuickWindow(QWindow *parent)
    : QQuickWindowQmlImpl(parent), d_ptr(std::make_unique<FramelessQuickWindowPrivate>(this))
{
    QQuickItem * const rootItem = contentItem();
    FramelessQuickHelper::get(rootItem)->extendsContentIntoTitleBar();
#if FRAMELESSHELPER_CONFIG(border_painter)
    Q_D(FramelessQuickWindow);
    d->windowBorder = new QuickWindowBorder;
    d->windowBorder->setParent(rootItem);
    d->windowBorder->setParentItem(rootItem);
    d->windowBorder->setZ(999); // Make sure it always stays on the top.
    QQuickItemPrivate::get(d->windowBorder)->anchors()->setFill(rootItem);
#endif
    connect(this, &FramelessQuickWindow::visibilityChanged, this, [this](){
        Q_EMIT hiddenChanged();
        Q_EMIT normalChanged();
        Q_EMIT minimizedChanged();
        Q_EMIT maximizedChanged();
        Q_EMIT zoomedChanged();
        Q_EMIT fullScreenChanged();
    });
}

FramelessQuickWindow::~FramelessQuickWindow() = default;

bool FramelessQuickWindow::isHidden() const
{
    return (visibility() == FramelessQuickWindow::Hidden);
}

bool FramelessQuickWindow::isNormal() const
{
    return (visibility() == FramelessQuickWindow::Windowed);
}

bool FramelessQuickWindow::isMinimized() const
{
    return (visibility() == FramelessQuickWindow::Minimized);
}

bool FramelessQuickWindow::isMaximized() const
{
    return (visibility() == FramelessQuickWindow::Maximized);
}

bool FramelessQuickWindow::isZoomed() const
{
    return (isMaximized() || (visibility() == FramelessQuickWindow::FullScreen));
}

bool FramelessQuickWindow::isFullScreen() const
{
    return (visibility() == FramelessQuickWindow::FullScreen);
}

void FramelessQuickWindow::showMinimized2()
{
#ifdef Q_OS_WINDOWS
    // Work-around a QtQuick bug: https://bugreports.qt.io/browse/QTBUG-69711
    // Don't use "SW_SHOWMINIMIZED" because it will activate the current window
    // instead of the next window in the Z order, which is not the default behavior
    // of native Win32 applications.
    ::ShowWindow(reinterpret_cast<HWND>(winId()), SW_MINIMIZE);
#else
    showMinimized();
#endif
}

void FramelessQuickWindow::toggleMaximized()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void FramelessQuickWindow::toggleFullScreen()
{
    Q_D(FramelessQuickWindow);
    if (isFullScreen()) {
        setVisibility(d->savedVisibility);
    } else {
        d->savedVisibility = visibility();
        showFullScreen();
    }
}

void FramelessQuickWindow::classBegin()
{
    QQuickWindowQmlImpl::classBegin();
}

void FramelessQuickWindow::componentComplete()
{
    QQuickWindowQmlImpl::componentComplete();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
