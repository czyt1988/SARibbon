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
#include <QtCore/qvariant.h>
#include <QtQuick/qquickpainteditem.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

class FRAMELESSHELPER_QUICK_API QuickImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(QuickImageItem)
#ifdef QML_NAMED_ELEMENT
    QML_NAMED_ELEMENT(ImageItem)
#endif

    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged FINAL)

public:
    explicit QuickImageItem(QQuickItem *parent = nullptr);
    ~QuickImageItem() override;

    void paint(QPainter *painter) override;

    Q_NODISCARD QVariant source() const;
    void setSource(const QVariant &value);

Q_SIGNALS:
    void sourceChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    void fromUrl(const QUrl &value, QPainter *painter) const;
    void fromString(const QString &value, QPainter *painter) const;
    void fromImage(const QImage &value, QPainter *painter) const;
    void fromPixmap(const QPixmap &value, QPainter *painter) const;
    void fromIcon(const QIcon &value, QPainter *painter) const;
    Q_NODISCARD QRectF paintArea() const;

private:
    QVariant m_source = {};
};

FRAMELESSHELPER_END_NAMESPACE
