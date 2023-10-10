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
#include <QtCore/qtimer.h>
#include <optional>

FRAMELESSHELPER_BEGIN_NAMESPACE

struct SystemParameters;
class FramelessManager;

class FRAMELESSHELPER_CORE_API FramelessManagerPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_CLASS_INFO
    Q_DECLARE_PUBLIC(FramelessManager)
    Q_DISABLE_COPY_MOVE(FramelessManagerPrivate)

public:
    explicit FramelessManagerPrivate(FramelessManager *q);
    ~FramelessManagerPrivate() override;

    Q_NODISCARD static FramelessManagerPrivate *get(FramelessManager *pub);
    Q_NODISCARD static const FramelessManagerPrivate *get(const FramelessManager *pub);

    static void initializeIconFont();
    Q_NODISCARD static QFont getIconFont();

    Q_SLOT void notifySystemThemeHasChangedOrNot();
    Q_SLOT void notifyWallpaperHasChangedOrNot();

    Q_NODISCARD bool isThemeOverrided() const;

    void initialize();

    void doNotifySystemThemeHasChangedOrNot();
    void doNotifyWallpaperHasChangedOrNot();

    FramelessManager *q_ptr = nullptr;
    Global::SystemTheme systemTheme = Global::SystemTheme::Unknown;
    std::optional<Global::SystemTheme> overrideTheme = std::nullopt;
    QColor accentColor = {};
#ifdef Q_OS_WINDOWS
    Global::DwmColorizationArea colorizationArea = Global::DwmColorizationArea::None;
#endif
    QString wallpaper = {};
    Global::WallpaperAspectStyle wallpaperAspectStyle = Global::WallpaperAspectStyle::Fill;
    QTimer themeTimer{};
    QTimer wallpaperTimer{};
};

FRAMELESSHELPER_END_NAMESPACE
