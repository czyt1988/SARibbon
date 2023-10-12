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
#include <QtWidgets/qwidget.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(mica_material)
class MicaMaterial;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
class WindowBorderPainter;
#endif

class FramelessWidgetsHelperPrivate;
class FRAMELESSHELPER_WIDGETS_API FramelessWidgetsHelper : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(FramelessWidgetsHelper)
    Q_PROPERTY(QWidget* titleBarWidget READ titleBarWidget WRITE setTitleBarWidget NOTIFY titleBarWidgetChanged FINAL)
    Q_PROPERTY(bool windowFixedSize READ isWindowFixedSize WRITE setWindowFixedSize NOTIFY windowFixedSizeChanged FINAL)
    Q_PROPERTY(bool blurBehindWindowEnabled READ isBlurBehindWindowEnabled WRITE setBlurBehindWindowEnabled NOTIFY blurBehindWindowEnabledChanged FINAL)
    Q_PROPERTY(QWidget* window READ window NOTIFY windowChanged FINAL)
    Q_PROPERTY(bool extendsContentIntoTitleBar READ isContentExtendedIntoTitleBar WRITE extendsContentIntoTitleBar NOTIFY extendsContentIntoTitleBarChanged FINAL)

public:
    explicit FramelessWidgetsHelper(QObject *parent = nullptr);
    ~FramelessWidgetsHelper() override;

    Q_NODISCARD static FramelessWidgetsHelper *get(QObject *object);

    Q_NODISCARD QWidget *titleBarWidget() const;
    Q_NODISCARD bool isWindowFixedSize() const;
    Q_NODISCARD bool isBlurBehindWindowEnabled() const;
    Q_NODISCARD QWidget *window() const;
    Q_NODISCARD bool isContentExtendedIntoTitleBar() const;

#if FRAMELESSHELPER_CONFIG(mica_material)
    Q_NODISCARD MicaMaterial *micaMaterial() const;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    Q_NODISCARD WindowBorderPainter *windowBorder() const;
#endif

    Q_NODISCARD bool isReady() const;
    void waitForReady();

public Q_SLOTS:
    void extendsContentIntoTitleBar(const bool value = true);

    void setTitleBarWidget(QWidget *widget);
    void setSystemButton(QWidget *widget, const Global::SystemButtonType buttonType);
    void setHitTestVisible(QWidget *widget, const bool visible = true);
    void setHitTestVisible(const QRect &rect, const bool visible = true);
    void setHitTestVisible(QObject *object, const bool visible = true);

    void showSystemMenu(const QPoint &pos);
    void windowStartSystemMove2(const QPoint &pos);
    void windowStartSystemResize2(const Qt::Edges edges, const QPoint &pos);

    void moveWindowToDesktopCenter();
    void bringWindowToFront();
    void setWindowFixedSize(const bool value);
    void setBlurBehindWindowEnabled(const bool value);

Q_SIGNALS:
    void extendsContentIntoTitleBarChanged();
    void titleBarWidgetChanged();
    void windowFixedSizeChanged();
    void blurBehindWindowEnabledChanged();
    void windowChanged();
    void ready();
};

FRAMELESSHELPER_END_NAMESPACE
