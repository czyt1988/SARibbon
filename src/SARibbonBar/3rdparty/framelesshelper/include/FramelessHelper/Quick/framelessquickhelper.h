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
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickwindow.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(mica_material)
class QuickMicaMaterial;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
class QuickWindowBorder;
#endif

class FramelessQuickHelperPrivate;
class FRAMELESSHELPER_QUICK_API FramelessQuickHelper : public QQuickItem
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(FramelessQuickHelper)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(FramelessHelper)
#endif
#ifdef QML_ATTACHED
    QML_ATTACHED(FramelessQuickHelper)
#endif
    Q_PROPERTY(QQuickItem* titleBarItem READ titleBarItem WRITE setTitleBarItem NOTIFY titleBarItemChanged FINAL)
    Q_PROPERTY(bool windowFixedSize READ isWindowFixedSize WRITE setWindowFixedSize NOTIFY windowFixedSizeChanged FINAL)
    Q_PROPERTY(bool blurBehindWindowEnabled READ isBlurBehindWindowEnabled WRITE setBlurBehindWindowEnabled NOTIFY blurBehindWindowEnabledChanged FINAL)
    Q_PROPERTY(QQuickWindow* window READ window NOTIFY windowChanged2 FINAL)
    Q_PROPERTY(bool extendsContentIntoTitleBar READ isContentExtendedIntoTitleBar WRITE extendsContentIntoTitleBar NOTIFY extendsContentIntoTitleBarChanged FINAL)

public:
    explicit FramelessQuickHelper(QQuickItem *parent = nullptr);
    ~FramelessQuickHelper() override;

    Q_NODISCARD static FramelessQuickHelper *get(QObject *object);
    Q_NODISCARD static FramelessQuickHelper *qmlAttachedProperties(QObject *parentObject);

    Q_NODISCARD QQuickItem *titleBarItem() const;
    Q_NODISCARD bool isWindowFixedSize() const;
    Q_NODISCARD bool isBlurBehindWindowEnabled() const;
    Q_NODISCARD bool isContentExtendedIntoTitleBar() const;

#if FRAMELESSHELPER_CONFIG(mica_material)
    Q_NODISCARD QuickMicaMaterial *micaMaterial() const;
#endif
#if FRAMELESSHELPER_CONFIG(border_painter)
    Q_NODISCARD QuickWindowBorder *windowBorder() const;
#endif

    Q_NODISCARD bool isReady() const;
    void waitForReady();

public Q_SLOTS:
    void extendsContentIntoTitleBar(const bool value = true);

    void setTitleBarItem(QQuickItem *value);
    void setSystemButton(QQuickItem *item, const QuickGlobal::SystemButtonType buttonType);
    void setHitTestVisible(QQuickItem *item, const bool visible = true);
    void setHitTestVisible_rect(const QRect &rect, const bool visible = true);
    void setHitTestVisible_object(QObject *object, const bool visible = true);
    void setHitTestVisible_item(QQuickItem *item, const bool visible = true);

    void showSystemMenu(const QPoint &pos);
    void windowStartSystemMove2(const QPoint &pos);
    void windowStartSystemResize2(const Qt::Edges edges, const QPoint &pos);

    void moveWindowToDesktopCenter();
    void bringWindowToFront();
    void setWindowFixedSize(const bool value);
    void setBlurBehindWindowEnabled(const bool value);

protected:
    void itemChange(const ItemChange change, const ItemChangeData &value) override;
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void extendsContentIntoTitleBarChanged();
    void titleBarItemChanged();
    void windowFixedSizeChanged();
    void blurBehindWindowEnabledChanged();
    void windowChanged2();
    void ready();
};

FRAMELESSHELPER_END_NAMESPACE

QML_DECLARE_TYPEINFO(FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessQuickHelper), QML_HAS_ATTACHED_PROPERTIES)
