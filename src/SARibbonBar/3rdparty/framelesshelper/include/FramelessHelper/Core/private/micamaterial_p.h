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
#include <QtGui/qbrush.h>

#if FRAMELESSHELPER_CONFIG(mica_material)

FRAMELESSHELPER_BEGIN_NAMESPACE

class MicaMaterial;
class FRAMELESSHELPER_CORE_API MicaMaterialPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PRIVATE_QT_CLASS(MicaMaterial)

public:
    explicit MicaMaterialPrivate(MicaMaterial *q);
    ~MicaMaterialPrivate() override;

    Q_NODISCARD static QColor systemFallbackColor();

    Q_NODISCARD QPoint mapToWallpaper(const QPoint &pos) const;
    Q_NODISCARD QSize mapToWallpaper(const QSize &size) const;
    Q_NODISCARD QRect mapToWallpaper(const QRect &rect) const;

    Q_SLOT void maybeGenerateBlurredWallpaper(const bool force = false);
    Q_SLOT void updateMaterialBrush();
    Q_SLOT void forceRebuildWallpaper();

    void initialize();
    void prepareGraphicsResources();

    QColor tintColor = {};
    qreal tintOpacity = qreal(0);
    QColor fallbackColor = {};
    qreal noiseOpacity = qreal(0);
    bool fallbackEnabled = true;
    QBrush micaBrush = {};
    bool initialized = false;
    QSize wallpaperSize = {};
};

FRAMELESSHELPER_END_NAMESPACE

#endif
