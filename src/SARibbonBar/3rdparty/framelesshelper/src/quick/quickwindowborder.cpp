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

#include "quickwindowborder.h"
#include "quickwindowborder_p.h"

#if FRAMELESSHELPER_CONFIG(border_painter)

#include <FramelessHelper/Core/windowborderpainter.h>
#include <QtCore/qloggingcategory.h>
#include <QtQuick/qquickwindow.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  include <QtQuick/private/qquickitem_p.h>
#endif

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcQuickWindowBorder, "wangwenx190.framelesshelper.quick.quickwindowborder")
#  define INFO qCInfo(lcQuickWindowBorder)
#  define DEBUG qCDebug(lcQuickWindowBorder)
#  define WARNING qCWarning(lcQuickWindowBorder)
#  define CRITICAL qCCritical(lcQuickWindowBorder)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

[[nodiscard]] static inline QuickGlobal::WindowEdges edgesToQuickEdges(const WindowEdges edges)
{
    QuickGlobal::WindowEdges result = {};
    if (edges & WindowEdge::Left) {
        result |= QuickGlobal::WindowEdge::Left;
    }
    if (edges & WindowEdge::Top) {
        result |= QuickGlobal::WindowEdge::Top;
    }
    if (edges & WindowEdge::Right) {
        result |= QuickGlobal::WindowEdge::Right;
    }
    if (edges & WindowEdge::Bottom) {
        result |= QuickGlobal::WindowEdge::Bottom;
    }
    return result;
}

[[nodiscard]] static inline WindowEdges quickEdgesToEdges(const QuickGlobal::WindowEdges edges)
{
    WindowEdges result = {};
    if (edges & QuickGlobal::WindowEdge::Left) {
        result |= WindowEdge::Left;
    }
    if (edges & QuickGlobal::WindowEdge::Top) {
        result |= WindowEdge::Top;
    }
    if (edges & QuickGlobal::WindowEdge::Right) {
        result |= WindowEdge::Right;
    }
    if (edges & QuickGlobal::WindowEdge::Bottom) {
        result |= WindowEdge::Bottom;
    }
    return result;
}

QuickWindowBorderPrivate::QuickWindowBorderPrivate(QuickWindowBorder *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

QuickWindowBorderPrivate::~QuickWindowBorderPrivate() = default;

QuickWindowBorderPrivate *QuickWindowBorderPrivate::get(QuickWindowBorder *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

const QuickWindowBorderPrivate *QuickWindowBorderPrivate::get(const QuickWindowBorder *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

void QuickWindowBorderPrivate::update()
{
    Q_Q(QuickWindowBorder);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    q->update();
    q->setVisible(window->visibility() == QQuickWindow::Windowed);
}

void QuickWindowBorderPrivate::initialize()
{
    Q_Q(QuickWindowBorder);
    q->setClip(true);
    q->setSmooth(true);
    // We can't enable antialising for this element due to we are drawing
    // some very thin lines that are too fragile.
    q->setAntialiasing(false);

    borderPainter = new WindowBorderPainter(this);
    connect(borderPainter, &WindowBorderPainter::thicknessChanged,
        q, &QuickWindowBorder::thicknessChanged);
    connect(borderPainter, &WindowBorderPainter::edgesChanged,
        q, &QuickWindowBorder::edgesChanged);
    connect(borderPainter, &WindowBorderPainter::activeColorChanged,
        q, &QuickWindowBorder::activeColorChanged);
    connect(borderPainter, &WindowBorderPainter::inactiveColorChanged,
        q, &QuickWindowBorder::inactiveColorChanged);
    connect(borderPainter, &WindowBorderPainter::nativeBorderChanged,
        q, &QuickWindowBorder::nativeBorderChanged);
    connect(borderPainter, &WindowBorderPainter::shouldRepaint, q, [q](){ q->update(); });
}

void QuickWindowBorderPrivate::rebindWindow()
{
    Q_Q(QuickWindowBorder);
    const QQuickWindow * const window = q->window();
    if (!window) {
        return;
    }
    QQuickItem * const rootItem = window->contentItem();
    q->setParent(rootItem);
    q->setParentItem(rootItem);
#if FRAMELESSHELPER_CONFIG(private_qt)
    QQuickItemPrivate::get(q)->anchors()->setFill(rootItem);
#endif
    q->setZ(999); // Make sure we always stays on the top most place.
    if (activeChangeConnection) {
        disconnect(activeChangeConnection);
        activeChangeConnection = {};
    }
    if (visibilityChangeConnection) {
        disconnect(visibilityChangeConnection);
        visibilityChangeConnection = {};
    }
    activeChangeConnection = connect(window, &QQuickWindow::activeChanged,
        this, &QuickWindowBorderPrivate::update);
    visibilityChangeConnection = connect(window, &QQuickWindow::visibilityChanged,
        this, &QuickWindowBorderPrivate::update);
}

QuickWindowBorder::QuickWindowBorder(QQuickItem *parent)
    : QQuickPaintedItem(parent), d_ptr(std::make_unique<QuickWindowBorderPrivate>(this))
{
}

QuickWindowBorder::~QuickWindowBorder() = default;

void QuickWindowBorder::paint(QPainter *painter)
{
    Q_ASSERT(painter);
    if (!painter) {
        return;
    }
    Q_D(QuickWindowBorder);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    const QSize s = size().toSize();
#else
    const QSize s = QSizeF{ width(), height() }.toSize();
#endif
    d->borderPainter->paint(painter, s, (window() && window()->isActive()));
}

qreal QuickWindowBorder::thickness() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->thickness();
}

QuickGlobal::WindowEdges QuickWindowBorder::edges() const
{
    Q_D(const QuickWindowBorder);
    return edgesToQuickEdges(d->borderPainter->edges());
}

QColor QuickWindowBorder::activeColor() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->activeColor();
}

QColor QuickWindowBorder::inactiveColor() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->inactiveColor();
}

qreal QuickWindowBorder::nativeThickness() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->nativeThickness();
}

QuickGlobal::WindowEdges QuickWindowBorder::nativeEdges() const
{
    Q_D(const QuickWindowBorder);
    return edgesToQuickEdges(d->borderPainter->nativeEdges());
}

QColor QuickWindowBorder::nativeActiveColor() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->nativeActiveColor();
}

QColor QuickWindowBorder::nativeInactiveColor() const
{
    Q_D(const QuickWindowBorder);
    return d->borderPainter->nativeInactiveColor();
}

void QuickWindowBorder::setThickness(const qreal value)
{
    Q_D(QuickWindowBorder);
    if (qFuzzyCompare(thickness(), value)) {
        return;
    }
    d->borderPainter->setThickness(std::round(value));
}

void QuickWindowBorder::setEdges(const QuickGlobal::WindowEdges value)
{
    Q_D(QuickWindowBorder);
    if (edges() == value) {
        return;
    }
    d->borderPainter->setEdges(quickEdgesToEdges(value));
}

void QuickWindowBorder::setActiveColor(const QColor &value)
{
    Q_D(QuickWindowBorder);
    if (activeColor() == value) {
        return;
    }
    d->borderPainter->setActiveColor(value);
}

void QuickWindowBorder::setInactiveColor(const QColor &value)
{
    Q_D(QuickWindowBorder);
    if (inactiveColor() == value) {
        return;
    }
    d->borderPainter->setInactiveColor(value);
}

void QuickWindowBorder::itemChange(const ItemChange change, const ItemChangeData &value)
{
    QQuickPaintedItem::itemChange(change, value);
    if ((change == ItemSceneChange) && value.window) {
        Q_D(QuickWindowBorder);
        d->rebindWindow();
    }
}

void QuickWindowBorder::classBegin()
{
    QQuickPaintedItem::classBegin();
}

void QuickWindowBorder::componentComplete()
{
    QQuickPaintedItem::componentComplete();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
