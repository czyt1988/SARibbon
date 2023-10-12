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
#include <QtGui/qfont.h>
#include <optional>

QT_BEGIN_NAMESPACE
class QMouseEvent;
QT_END_NAMESPACE

#if FRAMELESSHELPER_CONFIG(titlebar)

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(system_button)
class StandardSystemButton;
#endif
class ChromePalette;

class StandardTitleBar;
class FRAMELESSHELPER_WIDGETS_API StandardTitleBarPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PRIVATE_QT_CLASS(StandardTitleBar)

public:
    struct FontMetrics
    {
        int width = 0;
        int height = 0;
        int ascent = 0;
    };

    explicit StandardTitleBarPrivate(StandardTitleBar *q);
    ~StandardTitleBarPrivate() override;

    Q_NODISCARD QRect windowIconRect() const;
    Q_NODISCARD bool windowIconVisible_real() const;
    Q_NODISCARD bool isInTitleBarIconArea(const QPoint &pos) const;

    Q_NODISCARD QFont defaultFont() const;
    Q_NODISCARD FontMetrics titleLabelSize() const;
    Q_NODISCARD int titleLabelMaxWidth() const;

    Q_SLOT void updateMaximizeButton();
    Q_SLOT void updateTitleBarColor();
    Q_SLOT void updateChromeButtonColor();
    Q_SLOT void retranslateUi();

    Q_NODISCARD bool mouseEventHandler(QMouseEvent *event);

    void initialize();

#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    StandardSystemButton *minimizeButton = nullptr;
    StandardSystemButton *maximizeButton = nullptr;
    StandardSystemButton *closeButton = nullptr;
#endif
    QPointer<QWidget> window = nullptr;
    bool extended = false;
    Qt::Alignment labelAlignment = {};
    bool hideWhenClose = false;
    ChromePalette *chromePalette = nullptr;
    bool titleLabelVisible = true;
    std::optional<QSize> windowIconSize = std::nullopt;
    bool windowIconVisible = false;
    std::optional<QFont> titleFont = std::nullopt;
    bool closeTriggered = false;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
};

FRAMELESSHELPER_END_NAMESPACE

#endif
