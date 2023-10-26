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

#include "framelesswidget.h"
#include "framelesswidget_p.h"

#if FRAMELESSHELPER_CONFIG(window)

#include "framelesswidgetshelper.h"
#include "widgetssharedhelper_p.h"
#include <FramelessHelper/Core/utils.h>
#include <QtCore/qloggingcategory.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessWidget, "wangwenx190.framelesshelper.widgets.framelesswidget")
#  define INFO qCInfo(lcFramelessWidget)
#  define DEBUG qCDebug(lcFramelessWidget)
#  define WARNING qCWarning(lcFramelessWidget)
#  define CRITICAL qCCritical(lcFramelessWidget)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

FramelessWidgetPrivate::FramelessWidgetPrivate(FramelessWidget *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
}

FramelessWidgetPrivate::~FramelessWidgetPrivate() = default;

FramelessWidgetPrivate *FramelessWidgetPrivate::get(FramelessWidget *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const FramelessWidgetPrivate *FramelessWidgetPrivate::get(const FramelessWidget *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

FramelessWidget::FramelessWidget(QWidget *parent)
    : QWidget(parent, Qt::Window),  d_ptr(std::make_unique<FramelessWidgetPrivate>(this))
{
    FramelessWidgetsHelper::get(this)->extendsContentIntoTitleBar();
    Q_D(FramelessWidget);
    d->sharedHelper = new WidgetsSharedHelper(d);
    d->sharedHelper->setup(this);
}

FramelessWidget::~FramelessWidget() = default;

bool FramelessWidget::isNormal() const
{
    return (Utils::windowStatesToWindowState(windowState()) == Qt::WindowNoState);
}

bool FramelessWidget::isZoomed() const
{
    return (isMaximized() || isFullScreen());
}

void FramelessWidget::toggleMaximized()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void FramelessWidget::toggleFullScreen()
{
    Q_D(FramelessWidget);
    if (isFullScreen()) {
        setWindowState(d->savedWindowState);
    } else {
        d->savedWindowState = Utils::windowStatesToWindowState(windowState());
        showFullScreen();
    }
}

FRAMELESSHELPER_END_NAMESPACE

#endif
