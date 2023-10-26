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

#include "windowborderpainter.h"
#include "windowborderpainter_p.h"

#if FRAMELESSHELPER_CONFIG(border_painter)

#include "utils.h"
#include "framelessmanager.h"
#ifdef Q_OS_WINDOWS
#  include "winverhelper_p.h"
#endif
#include <QtCore/qloggingcategory.h>
#include <QtGui/qpainter.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcWindowBorderPainter, "wangwenx190.framelesshelper.core.windowborderpainter")
#  define INFO qCInfo(lcWindowBorderPainter)
#  define DEBUG qCDebug(lcWindowBorderPainter)
#  define WARNING qCWarning(lcWindowBorderPainter)
#  define CRITICAL qCCritical(lcWindowBorderPainter)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static constexpr const int kMaximumBorderThickness = 100;

WindowBorderPainterPrivate::WindowBorderPainterPrivate(WindowBorderPainter *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
}

WindowBorderPainterPrivate::~WindowBorderPainterPrivate() = default;

WindowBorderPainterPrivate *WindowBorderPainterPrivate::get(WindowBorderPainter *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

const WindowBorderPainterPrivate *WindowBorderPainterPrivate::get(const WindowBorderPainter *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

WindowBorderPainter::WindowBorderPainter(QObject *parent)
    : QObject(parent), d_ptr(std::make_unique<WindowBorderPainterPrivate>(this))
{
    connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, &WindowBorderPainter::nativeBorderChanged);
    connect(this, &WindowBorderPainter::nativeBorderChanged, this, &WindowBorderPainter::shouldRepaint);
}

WindowBorderPainter::~WindowBorderPainter() = default;

int WindowBorderPainter::thickness() const
{
    Q_D(const WindowBorderPainter);
    return d->thickness.value_or(nativeThickness());
}

WindowEdges WindowBorderPainter::edges() const
{
    Q_D(const WindowBorderPainter);
    return d->edges.value_or(nativeEdges());
}

QColor WindowBorderPainter::activeColor() const
{
    Q_D(const WindowBorderPainter);
    return d->activeColor.value_or(nativeActiveColor());
}

QColor WindowBorderPainter::inactiveColor() const
{
    Q_D(const WindowBorderPainter);
    return d->inactiveColor.value_or(nativeInactiveColor());
}

int WindowBorderPainter::nativeThickness() const
{
#ifdef Q_OS_WINDOWS
    // Qt will scale it to the appropriate value for us automatically,
    // based on the current system DPI and scale factor rounding policy.
    return kDefaultWindowFrameBorderThickness;
#else
    return 0;
#endif
}

WindowEdges WindowBorderPainter::nativeEdges() const
{
#ifdef Q_OS_WINDOWS
    if (Utils::isWindowFrameBorderVisible() && !WindowsVersionHelper::isWin11OrGreater()) {
        return { WindowEdge::Top };
    }
#endif
    return {};
}

QColor WindowBorderPainter::nativeActiveColor() const
{
    return Utils::getFrameBorderColor(true);
}

QColor WindowBorderPainter::nativeInactiveColor() const
{
    return Utils::getFrameBorderColor(false);
}

void WindowBorderPainter::paint(QPainter *painter, const QSize &size, const bool active)
{
    Q_ASSERT(painter);
    Q_ASSERT(!size.isEmpty());
    if (!painter || size.isEmpty()) {
        return;
    }
    Q_D(WindowBorderPainter);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QList<QLineF> lines = {};
#else
    QVector<QLineF> lines = {};
#endif
    static constexpr const auto gap = qreal(0.5);
    const auto leftTop = QPointF{ gap, gap };
    const auto rightTop = QPointF{ qreal(size.width()) - gap, leftTop.y() };
    const auto rightBottom = QPointF{ rightTop.x(), qreal(size.height()) - gap };
    const auto leftBottom = QPointF{ leftTop.x(), rightBottom.y() };
    const WindowEdges edges = d->edges.value_or(nativeEdges());
    if (edges & WindowEdge::Left) {
        lines.append({leftBottom, leftTop});
    }
    if (edges & WindowEdge::Top) {
        lines.append({leftTop, rightTop});
    }
    if (edges & WindowEdge::Right) {
        lines.append({rightTop, rightBottom});
    }
    if (edges & WindowEdge::Bottom) {
        lines.append({rightBottom, leftBottom});
    }
    if (lines.isEmpty()) {
        return;
    }
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    QPen pen = {};
    pen.setColor([this, d, active]() -> QColor {
        QColor color = {};
        if (active) {
            color = d->activeColor.value_or(nativeActiveColor());
        } else {
            color = d->inactiveColor.value_or(nativeInactiveColor());
        }
        if (color.isValid()) {
            return color;
        }
        return (active ? kDefaultBlackColor : kDefaultDarkGrayColor);
    }());
    pen.setWidth(d->thickness.value_or(nativeThickness()));
    painter->setPen(pen);
    painter->drawLines(lines);
    painter->restore();
}

void WindowBorderPainter::setThickness(const int value)
{
    Q_ASSERT(value >= 0);
    Q_ASSERT(value < kMaximumBorderThickness);
    if ((value < 0) || (value >= kMaximumBorderThickness)) {
        return;
    }
    if (thickness() == value) {
        return;
    }
    Q_D(WindowBorderPainter);
    d->thickness = value;
    Q_EMIT thicknessChanged();
    Q_EMIT shouldRepaint();
}

void WindowBorderPainter::setEdges(const Global::WindowEdges value)
{
    if (edges() == value) {
        return;
    }
    Q_D(WindowBorderPainter);
    d->edges = value;
    Q_EMIT edgesChanged();
    Q_EMIT shouldRepaint();
}

void WindowBorderPainter::setActiveColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    if (activeColor() == value) {
        return;
    }
    Q_D(WindowBorderPainter);
    d->activeColor = value;
    Q_EMIT activeColorChanged();
    Q_EMIT shouldRepaint();
}

void WindowBorderPainter::setInactiveColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    if (inactiveColor() == value) {
        return;
    }
    Q_D(WindowBorderPainter);
    d->inactiveColor = value;
    Q_EMIT inactiveColorChanged();
    Q_EMIT shouldRepaint();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
