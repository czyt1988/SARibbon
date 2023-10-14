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

#pragma once

#include <FramelessHelper/Quick/framelesshelperquick_global.h>
#include <QtQuick/qquickpainteditem.h>

#if FRAMELESSHELPER_CONFIG(border_painter)

FRAMELESSHELPER_BEGIN_NAMESPACE

class QuickWindowBorderPrivate;
class FRAMELESSHELPER_QUICK_API QuickWindowBorder : public QQuickPaintedItem
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(QuickWindowBorder)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(WindowBorder)
#endif

    Q_PROPERTY(qreal thickness READ thickness WRITE setThickness NOTIFY thicknessChanged FINAL)
    Q_PROPERTY(QuickGlobal::WindowEdges edges READ edges WRITE setEdges NOTIFY edgesChanged FINAL)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged FINAL)
    Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor NOTIFY inactiveColorChanged FINAL)

    Q_PROPERTY(qreal nativeThickness READ nativeThickness NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(QuickGlobal::WindowEdges nativeEdges READ nativeEdges NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(QColor nativeActiveColor READ nativeActiveColor NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(QColor nativeInactiveColor READ nativeInactiveColor NOTIFY nativeBorderChanged FINAL)

public:
    explicit QuickWindowBorder(QQuickItem *parent = nullptr);
    ~QuickWindowBorder() override;

    void paint(QPainter *painter) override;

    Q_NODISCARD qreal thickness() const;
    Q_NODISCARD QuickGlobal::WindowEdges edges() const;
    Q_NODISCARD QColor activeColor() const;
    Q_NODISCARD QColor inactiveColor() const;

    Q_NODISCARD qreal nativeThickness() const;
    Q_NODISCARD QuickGlobal::WindowEdges nativeEdges() const;
    Q_NODISCARD QColor nativeActiveColor() const;
    Q_NODISCARD QColor nativeInactiveColor() const;

public Q_SLOTS:
    void setThickness(const qreal value);
    void setEdges(const QuickGlobal::WindowEdges value);
    void setActiveColor(const QColor &value);
    void setInactiveColor(const QColor &value);

protected:
    void itemChange(const ItemChange change, const ItemChangeData &value) override;
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void thicknessChanged();
    void edgesChanged();
    void activeColorChanged();
    void inactiveColorChanged();
    void nativeBorderChanged();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
