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

#if (FRAMELESSHELPER_CONFIG(private_qt) && FRAMELESSHELPER_CONFIG(system_button) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)))

#include <QtQuickTemplates2/private/qquickbutton_p.h>

QT_BEGIN_NAMESPACE
class QQuickText;
class QQuickRectangle;
QT_END_NAMESPACE

FRAMELESSHELPER_BEGIN_NAMESPACE

class FRAMELESSHELPER_QUICK_API QuickStandardSystemButton : public QQuickButton
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(QuickStandardSystemButton)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(StandardSystemButton)
#endif
    Q_PROPERTY(QuickGlobal::SystemButtonType buttonType READ buttonType WRITE setButtonType NOTIFY buttonTypeChanged FINAL)
    Q_PROPERTY(QString glyph READ glyph WRITE setGlyph NOTIFY glyphChanged FINAL)
    Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor NOTIFY hoverColorChanged FINAL)
    Q_PROPERTY(QColor pressColor READ pressColor WRITE setPressColor NOTIFY pressColorChanged FINAL)
    Q_PROPERTY(QColor normalColor READ normalColor WRITE setNormalColor NOTIFY normalColorChanged FINAL)
    Q_PROPERTY(QColor activeForegroundColor READ activeForegroundColor WRITE setActiveForegroundColor NOTIFY activeForegroundColorChanged FINAL)
    Q_PROPERTY(QColor inactiveForegroundColor READ inactiveForegroundColor WRITE setInactiveForegroundColor NOTIFY inactiveForegroundColorChanged FINAL)
    Q_PROPERTY(qreal glyphSize READ glyphSize WRITE setGlyphSize NOTIFY glyphChanged FINAL)

public:
    explicit QuickStandardSystemButton(QQuickItem *parent = nullptr);
    explicit QuickStandardSystemButton(const QuickGlobal::SystemButtonType type, QQuickItem *parent = nullptr);
    ~QuickStandardSystemButton() override;

    Q_NODISCARD QuickGlobal::SystemButtonType buttonType() const;
    Q_NODISCARD QString glyph() const;
    Q_NODISCARD QColor normalColor() const;
    Q_NODISCARD QColor hoverColor() const;
    Q_NODISCARD QColor pressColor() const;
    Q_NODISCARD QColor activeForegroundColor() const;
    Q_NODISCARD QColor inactiveForegroundColor() const;
    Q_NODISCARD qreal glyphSize() const;

public Q_SLOTS:
    void updateColor();
    void setButtonType(const QuickGlobal::SystemButtonType type);
    void setGlyph(const QString &value);
    void setNormalColor(const QColor &value);
    void setHoverColor(const QColor &value);
    void setPressColor(const QColor &value);
    void setActiveForegroundColor(const QColor &value);
    void setInactiveForegroundColor(const QColor &value);
    void setGlyphSize(const qreal value);

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    void initialize();

Q_SIGNALS:
    void buttonTypeChanged();
    void glyphChanged();
    void normalColorChanged();
    void hoverColorChanged();
    void pressColorChanged();
    void activeForegroundColorChanged();
    void inactiveForegroundColorChanged();
    void glyphSizeChanged();

private:
    QQuickText *m_contentItem = nullptr;
    QQuickRectangle *m_backgroundItem = nullptr;
    QuickGlobal::SystemButtonType m_buttonType = QuickGlobal::SystemButtonType::Unknown;
    QString m_glyph = {};
    QColor m_normalColor = {};
    QColor m_hoverColor = {};
    QColor m_pressColor = {};
    QColor m_activeForegroundColor = {};
    QColor m_inactiveForegroundColor = {};
};

FRAMELESSHELPER_END_NAMESPACE

#endif
