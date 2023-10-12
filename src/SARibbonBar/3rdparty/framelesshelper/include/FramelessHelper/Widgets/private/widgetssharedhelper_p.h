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

#include <FramelessHelper/Widgets/framelesshelperwidgets_global.h>
#include <QtGui/qscreen.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(mica_material)
class MicaMaterial;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
class WindowBorderPainter;
#endif

class FRAMELESSHELPER_WIDGETS_API WidgetsSharedHelper : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(WidgetsSharedHelper)
#if FRAMELESSHELPER_CONFIG(mica_material)
    Q_PROPERTY(bool micaEnabled READ isMicaEnabled WRITE setMicaEnabled NOTIFY micaEnabledChanged FINAL)
#endif

public:
    explicit WidgetsSharedHelper(QObject *parent = nullptr);
    ~WidgetsSharedHelper() override;

    void setup(QWidget *widget);

#if FRAMELESSHELPER_CONFIG(mica_material)
    Q_NODISCARD bool isMicaEnabled() const;
    void setMicaEnabled(const bool value);
    Q_NODISCARD MicaMaterial *rawMicaMaterial() const;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    Q_NODISCARD WindowBorderPainter *rawWindowBorder() const;
#endif

protected:
    Q_NODISCARD bool eventFilter(QObject *object, QEvent *event) override;

private Q_SLOTS:
    void updateContentsMargins();
    void handleScreenChanged(QScreen *screen);

private:
#if FRAMELESSHELPER_CONFIG(mica_material)
    void repaintMica();
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    void repaintBorder();
#endif
    void emitCustomWindowStateSignals();

Q_SIGNALS:
#if FRAMELESSHELPER_CONFIG(mica_material)
    void micaEnabledChanged();
#endif

private:
    QPointer<QWidget> m_targetWidget;
    QPointer<QScreen> m_screen;
    qreal m_screenDpr = qreal(0);
    QMetaObject::Connection m_screenDpiChangeConnection = {};
    QMetaObject::Connection m_screenChangeConnection = {};
#if FRAMELESSHELPER_CONFIG(mica_material)
    bool m_micaEnabled = false;
    MicaMaterial *m_micaMaterial = nullptr;
    QMetaObject::Connection m_micaRedrawConnection = {};
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    WindowBorderPainter *m_borderPainter = nullptr;
    QMetaObject::Connection m_borderRepaintConnection = {};
#endif
};

FRAMELESSHELPER_END_NAMESPACE
