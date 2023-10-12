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

#if FRAMELESSHELPER_CONFIG(titlebar)

#include <FramelessHelper/Core/chromepalette.h>
#if FRAMELESSHELPER_CONFIG(system_button)
#  include <FramelessHelper/Widgets/standardsystembutton.h>
#endif
#include <QtWidgets/qwidget.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

class StandardTitleBarPrivate;
class FRAMELESSHELPER_WIDGETS_API StandardTitleBar : public QWidget
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(StandardTitleBar)
    Q_PROPERTY(Qt::Alignment titleLabelAlignment READ titleLabelAlignment WRITE setTitleLabelAlignment NOTIFY titleLabelAlignmentChanged FINAL)
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    Q_PROPERTY(StandardSystemButton* minimizeButton READ minimizeButton CONSTANT FINAL)
    Q_PROPERTY(StandardSystemButton* maximizeButton READ maximizeButton CONSTANT FINAL)
    Q_PROPERTY(StandardSystemButton* closeButton READ closeButton CONSTANT FINAL)
#endif
    Q_PROPERTY(bool extended READ isExtended WRITE setExtended NOTIFY extendedChanged FINAL)
    Q_PROPERTY(bool hideWhenClose READ isHideWhenClose WRITE setHideWhenClose NOTIFY hideWhenCloseChanged FINAL)
    Q_PROPERTY(ChromePalette* chromePalette READ chromePalette CONSTANT FINAL)
    Q_PROPERTY(bool titleLabelVisible READ titleLabelVisible WRITE setTitleLabelVisible NOTIFY titleLabelVisibleChanged FINAL)
    Q_PROPERTY(QSize windowIconSize READ windowIconSize WRITE setWindowIconSize NOTIFY windowIconSizeChanged FINAL)
    Q_PROPERTY(bool windowIconVisible READ windowIconVisible WRITE setWindowIconVisible NOTIFY windowIconVisibleChanged FINAL)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont NOTIFY titleFontChanged FINAL)

public:
    explicit StandardTitleBar(QWidget *parent = nullptr);
    ~StandardTitleBar() override;

    Q_NODISCARD Qt::Alignment titleLabelAlignment() const;
    void setTitleLabelAlignment(const Qt::Alignment value);

#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    Q_NODISCARD StandardSystemButton *minimizeButton() const;
    Q_NODISCARD StandardSystemButton *maximizeButton() const;
    Q_NODISCARD StandardSystemButton *closeButton() const;
#endif // Q_OS_MACOS

    Q_NODISCARD bool isExtended() const;
    void setExtended(const bool value);

    Q_NODISCARD bool isHideWhenClose() const;
    void setHideWhenClose(const bool value);

    Q_NODISCARD ChromePalette *chromePalette() const;

    Q_NODISCARD bool titleLabelVisible() const;
    void setTitleLabelVisible(const bool value);

    Q_NODISCARD QSize windowIconSize() const;
    void setWindowIconSize(const QSize &value);

    Q_NODISCARD bool windowIconVisible() const;
    void setWindowIconVisible(const bool value);

    Q_NODISCARD QFont titleFont() const;
    void setTitleFont(const QFont &value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void extendedChanged();
    void titleLabelAlignmentChanged();
    void hideWhenCloseChanged();
    void titleLabelVisibleChanged();
    void windowIconSizeChanged();
    void windowIconVisibleChanged();
    void titleFontChanged();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
