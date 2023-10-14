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
#include <QtQml/qqmlparserstatus.h>

QT_BEGIN_NAMESPACE
class QQuickWindow;
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE

class FRAMELESSHELPER_QUICK_API FramelessQuickUtils : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(FramelessQuickUtils)
    Q_INTERFACES(QQmlParserStatus)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(FramelessUtils)
#endif
#ifdef QML_SINGLETON
    QML_SINGLETON
#endif
    Q_PROPERTY(qreal titleBarHeight READ titleBarHeight CONSTANT FINAL)
    Q_PROPERTY(bool frameBorderVisible READ frameBorderVisible CONSTANT FINAL)
    Q_PROPERTY(qreal frameBorderThickness READ frameBorderThickness CONSTANT FINAL)
    Q_PROPERTY(QuickGlobal::SystemTheme systemTheme READ systemTheme WRITE setOverrideTheme NOTIFY systemThemeChanged FINAL)
    Q_PROPERTY(QColor systemAccentColor READ systemAccentColor NOTIFY systemAccentColorChanged FINAL)
    Q_PROPERTY(bool titleBarColorized READ titleBarColorized NOTIFY titleBarColorizedChanged FINAL)
    Q_PROPERTY(QColor defaultSystemLightColor READ defaultSystemLightColor CONSTANT FINAL)
    Q_PROPERTY(QColor defaultSystemDarkColor READ defaultSystemDarkColor CONSTANT FINAL)
    Q_PROPERTY(QSizeF defaultSystemButtonSize READ defaultSystemButtonSize CONSTANT FINAL)
    Q_PROPERTY(QSizeF defaultSystemButtonIconSize READ defaultSystemButtonIconSize CONSTANT FINAL)
    Q_PROPERTY(QColor defaultSystemButtonBackgroundColor READ defaultSystemButtonBackgroundColor CONSTANT FINAL)
    Q_PROPERTY(QColor defaultSystemCloseButtonBackgroundColor READ defaultSystemCloseButtonBackgroundColor CONSTANT FINAL)

public:
    explicit FramelessQuickUtils(QObject *parent = nullptr);
    ~FramelessQuickUtils() override;

    Q_NODISCARD qreal titleBarHeight() const;
    Q_NODISCARD bool frameBorderVisible() const;
    Q_NODISCARD qreal frameBorderThickness() const;
    Q_NODISCARD QuickGlobal::SystemTheme systemTheme() const;
    void setOverrideTheme(const QuickGlobal::SystemTheme theme);
    Q_NODISCARD QColor systemAccentColor() const;
    Q_NODISCARD bool titleBarColorized() const;
    Q_NODISCARD QColor defaultSystemLightColor() const;
    Q_NODISCARD QColor defaultSystemDarkColor() const;
    Q_NODISCARD QSizeF defaultSystemButtonSize() const;
    Q_NODISCARD QSizeF defaultSystemButtonIconSize() const;
    Q_NODISCARD QColor defaultSystemButtonBackgroundColor() const;
    Q_NODISCARD QColor defaultSystemCloseButtonBackgroundColor() const;

    Q_NODISCARD Q_SLOT QColor getSystemButtonBackgroundColor(
        const QuickGlobal::SystemButtonType button, const QuickGlobal::ButtonState state);

protected:
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void systemThemeChanged();
    void systemAccentColorChanged();
    void titleBarColorizedChanged();
};

FRAMELESSHELPER_END_NAMESPACE
