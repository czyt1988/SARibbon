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

#if (FRAMELESSHELPER_CONFIG(private_qt) && FRAMELESSHELPER_CONFIG(window))

#include <QtQuick/qquickwindow.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(border_painter)
class QuickWindowBorder;
#endif

class FramelessQuickWindow;
class FRAMELESSHELPER_QUICK_API FramelessQuickWindowPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PRIVATE_QT_CLASS(FramelessQuickWindow)

public:
    explicit FramelessQuickWindowPrivate(FramelessQuickWindow *q);
    ~FramelessQuickWindowPrivate() override;

    QQuickWindow::Visibility savedVisibility = QQuickWindow::Windowed;
#if FRAMELESSHELPER_CONFIG(border_painter)
    QuickWindowBorder *windowBorder = nullptr;
#endif
};

FRAMELESSHELPER_END_NAMESPACE

#endif
