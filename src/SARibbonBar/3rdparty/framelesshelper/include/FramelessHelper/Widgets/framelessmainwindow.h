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
#include <QtWidgets/qmainwindow.h>

#if FRAMELESSHELPER_CONFIG(window)

FRAMELESSHELPER_BEGIN_NAMESPACE

class FramelessMainWindowPrivate;
class FRAMELESSHELPER_WIDGETS_API FramelessMainWindow : public QMainWindow
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(FramelessMainWindow)
    Q_PROPERTY(bool hidden READ isHidden NOTIFY hiddenChanged FINAL)
    Q_PROPERTY(bool normal READ isNormal NOTIFY normalChanged FINAL)
    Q_PROPERTY(bool zoomed READ isZoomed NOTIFY zoomedChanged FINAL)

public:
    explicit FramelessMainWindow(QWidget *parent = nullptr, const Qt::WindowFlags flags = {});
    ~FramelessMainWindow() override;

    Q_NODISCARD bool isNormal() const;
    Q_NODISCARD bool isZoomed() const;

public Q_SLOTS:
    void toggleMaximized();
    void toggleFullScreen();

Q_SIGNALS:
    void hiddenChanged();
    void normalChanged();
    void zoomedChanged();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
