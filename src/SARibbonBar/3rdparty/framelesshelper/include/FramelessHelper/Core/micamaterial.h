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

#include <FramelessHelper/Core/framelesshelpercore_global.h>

#if FRAMELESSHELPER_CONFIG(mica_material)

FRAMELESSHELPER_BEGIN_NAMESPACE

class MicaMaterialPrivate;
class FRAMELESSHELPER_CORE_API MicaMaterial : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(MicaMaterial)

    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor NOTIFY tintColorChanged FINAL)
    Q_PROPERTY(qreal tintOpacity READ tintOpacity WRITE setTintOpacity NOTIFY tintOpacityChanged FINAL)
    Q_PROPERTY(QColor fallbackColor READ fallbackColor WRITE setFallbackColor NOTIFY fallbackColorChanged FINAL)
    Q_PROPERTY(qreal noiseOpacity READ noiseOpacity WRITE setNoiseOpacity NOTIFY noiseOpacityChanged FINAL)
    Q_PROPERTY(bool fallbackEnabled READ isFallbackEnabled WRITE setFallbackEnabled NOTIFY fallbackEnabledChanged FINAL)

public:
    explicit MicaMaterial(QObject *parent = nullptr);
    ~MicaMaterial() override;

    Q_NODISCARD QColor tintColor() const;
    void setTintColor(const QColor &value);

    Q_NODISCARD qreal tintOpacity() const;
    void setTintOpacity(const qreal value);

    Q_NODISCARD QColor fallbackColor() const;
    void setFallbackColor(const QColor &value);

    Q_NODISCARD qreal noiseOpacity() const;
    void setNoiseOpacity(const qreal value);

    Q_NODISCARD bool isFallbackEnabled() const;
    void setFallbackEnabled(const bool value);

public Q_SLOTS:
    void paint(QPainter *painter, const QRect &rect, const bool active = true);

    [[deprecated("Use another overload instead.")]]
    void paint(QPainter *painter, const QSize &size, const QPoint &pos, const bool active = true)
    {
        paint(painter, QRect{ pos, size }, active);
    }

Q_SIGNALS:
    void tintColorChanged();
    void tintOpacityChanged();
    void fallbackColorChanged();
    void noiseOpacityChanged();
    void fallbackEnabledChanged();
    void shouldRedraw();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
