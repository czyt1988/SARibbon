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

#if (FRAMELESSHELPER_CONFIG(private_qt) && FRAMELESSHELPER_CONFIG(titlebar) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)))

#include <FramelessHelper/Quick/quickchromepalette.h>
#if FRAMELESSHELPER_CONFIG(system_button)
#  include <FramelessHelper/Quick/private/quickstandardsystembutton_p.h>
#endif
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuickTemplates2/private/qquicklabel_p.h>

QT_BEGIN_NAMESPACE
class QQuickRow;
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE

class QuickImageItem;

class FRAMELESSHELPER_QUICK_API QuickStandardTitleBar : public QQuickRectangle
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(QuickStandardTitleBar)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(StandardTitleBar)
#endif // QML_NAMED_ELEMENT
    Q_PROPERTY(Qt::Alignment titleLabelAlignment READ titleLabelAlignment WRITE setTitleLabelAlignment NOTIFY titleLabelAlignmentChanged FINAL)
    Q_PROPERTY(QQuickLabel* titleLabel READ titleLabel CONSTANT FINAL)
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    Q_PROPERTY(QuickStandardSystemButton* minimizeButton READ minimizeButton CONSTANT FINAL)
    Q_PROPERTY(QuickStandardSystemButton* maximizeButton READ maximizeButton CONSTANT FINAL)
    Q_PROPERTY(QuickStandardSystemButton* closeButton READ closeButton CONSTANT FINAL)
#endif // Q_OS_MACOS
    Q_PROPERTY(bool extended READ isExtended WRITE setExtended NOTIFY extendedChanged FINAL)
    Q_PROPERTY(bool hideWhenClose READ isHideWhenClose WRITE setHideWhenClose NOTIFY hideWhenCloseChanged FINAL)
    Q_PROPERTY(QuickChromePalette* chromePalette READ chromePalette CONSTANT FINAL)
    Q_PROPERTY(QSizeF windowIconSize READ windowIconSize WRITE setWindowIconSize NOTIFY windowIconSizeChanged FINAL)
    Q_PROPERTY(bool windowIconVisible READ windowIconVisible WRITE setWindowIconVisible NOTIFY windowIconVisibleChanged FINAL)
    Q_PROPERTY(QVariant windowIcon READ windowIcon WRITE setWindowIcon NOTIFY windowIconChanged FINAL)

public:
    explicit QuickStandardTitleBar(QQuickItem *parent = nullptr);
    ~QuickStandardTitleBar() override;

    Q_NODISCARD Qt::Alignment titleLabelAlignment() const;
    void setTitleLabelAlignment(const Qt::Alignment value);

    Q_NODISCARD QQuickLabel *titleLabel() const;
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    Q_NODISCARD QuickStandardSystemButton *minimizeButton() const;
    Q_NODISCARD QuickStandardSystemButton *maximizeButton() const;
    Q_NODISCARD QuickStandardSystemButton *closeButton() const;
#endif // Q_OS_MACOS

    Q_NODISCARD bool isExtended() const;
    void setExtended(const bool value);

    Q_NODISCARD bool isHideWhenClose() const;
    void setHideWhenClose(const bool value);

    Q_NODISCARD QuickChromePalette *chromePalette() const;

    Q_NODISCARD QSizeF windowIconSize() const;
    void setWindowIconSize(const QSizeF &value);

    Q_NODISCARD bool windowIconVisible() const;
    void setWindowIconVisible(const bool value);

    Q_NODISCARD QVariant windowIcon() const;
    void setWindowIcon(const QVariant &value);

protected:
    void itemChange(const ItemChange change, const ItemChangeData &value) override;
    Q_NODISCARD bool eventFilter(QObject *object, QEvent *event) override;
    void classBegin() override;
    void componentComplete() override;

private Q_SLOTS:
    void updateMaximizeButton();
    void updateTitleLabelText();
    void updateTitleBarColor();
    void updateChromeButtonColor();
    void clickMinimizeButton();
    void clickMaximizeButton();
    void clickCloseButton();
    void retranslateUi();
    void updateWindowIcon();

Q_SIGNALS:
    void titleLabelAlignmentChanged();
    void extendedChanged();
    void hideWhenCloseChanged();
    void windowIconSizeChanged();
    void windowIconVisibleChanged();
    void windowIconChanged();

private:
    void initialize();
    void updateAll();
    Q_NODISCARD bool mouseEventHandler(QMouseEvent *event);
    Q_NODISCARD QRect windowIconRect() const;
    Q_NODISCARD bool isInTitleBarIconArea(const QPoint &pos) const;
    Q_NODISCARD bool windowIconVisible_real() const;

private:
    Qt::Alignment m_labelAlignment = {};
    QuickImageItem *m_windowIcon = nullptr;
    QQuickLabel *m_windowTitleLabel = nullptr;
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    QQuickRow *m_systemButtonsRow = nullptr;
    QuickStandardSystemButton *m_minimizeButton = nullptr;
    QuickStandardSystemButton *m_maximizeButton = nullptr;
    QuickStandardSystemButton *m_closeButton = nullptr;
#endif // Q_OS_MACOS
    QMetaObject::Connection m_windowStateChangeConnection = {};
    QMetaObject::Connection m_windowActiveChangeConnection = {};
    QMetaObject::Connection m_windowTitleChangeConnection = {};
    bool m_extended = false;
    bool m_hideWhenClose = false;
    QuickChromePalette *m_chromePalette = nullptr;
    bool m_closeTriggered = false;
};

FRAMELESSHELPER_END_NAMESPACE

#endif
