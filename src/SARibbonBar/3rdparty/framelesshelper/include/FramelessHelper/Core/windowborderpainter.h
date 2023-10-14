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

#include <FramelessHelper/Core/framelesshelpercore_global.h>

#if FRAMELESSHELPER_CONFIG(border_painter)

FRAMELESSHELPER_BEGIN_NAMESPACE

class WindowBorderPainterPrivate;
class FRAMELESSHELPER_CORE_API WindowBorderPainter : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(WindowBorderPainter)

    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged FINAL)
    Q_PROPERTY(Global::WindowEdges edges READ edges WRITE setEdges NOTIFY edgesChanged FINAL)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged FINAL)
    Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor NOTIFY inactiveColorChanged FINAL)

    Q_PROPERTY(int nativeThickness READ nativeThickness NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(Global::WindowEdges nativeEdges READ nativeEdges NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(QColor nativeActiveColor READ nativeActiveColor NOTIFY nativeBorderChanged FINAL)
    Q_PROPERTY(QColor nativeInactiveColor READ nativeInactiveColor NOTIFY nativeBorderChanged FINAL)

public:
    explicit WindowBorderPainter(QObject *parent = nullptr);
    ~WindowBorderPainter() override;

    Q_NODISCARD int thickness() const;
    Q_NODISCARD Global::WindowEdges edges() const;
    Q_NODISCARD QColor activeColor() const;
    Q_NODISCARD QColor inactiveColor() const;

    Q_NODISCARD int nativeThickness() const;
    Q_NODISCARD Global::WindowEdges nativeEdges() const;
    Q_NODISCARD QColor nativeActiveColor() const;
    Q_NODISCARD QColor nativeInactiveColor() const;

public Q_SLOTS:
    void paint(QPainter *painter, const QSize &size, const bool active);
    void setThickness(const int value);
    void setEdges(const Global::WindowEdges value);
    void setActiveColor(const QColor &value);
    void setInactiveColor(const QColor &value);

Q_SIGNALS:
    void thicknessChanged();
    void edgesChanged();
    void activeColorChanged();
    void inactiveColorChanged();
    void nativeBorderChanged();
    void shouldRepaint();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
