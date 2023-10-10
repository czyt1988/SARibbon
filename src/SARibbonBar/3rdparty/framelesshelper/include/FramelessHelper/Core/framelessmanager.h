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

FRAMELESSHELPER_BEGIN_NAMESPACE

struct SystemParameters;
class FramelessManagerPrivate;

class FRAMELESSHELPER_CORE_API FramelessManager : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_CLASS_INFO
    Q_DECLARE_PRIVATE(FramelessManager)
    Q_DISABLE_COPY_MOVE(FramelessManager)
    Q_PROPERTY(Global::SystemTheme systemTheme READ systemTheme WRITE setOverrideTheme NOTIFY systemThemeChanged FINAL)
    Q_PROPERTY(QColor systemAccentColor READ systemAccentColor NOTIFY systemThemeChanged FINAL)
    Q_PROPERTY(QString wallpaper READ wallpaper NOTIFY wallpaperChanged FINAL)
    Q_PROPERTY(Global::WallpaperAspectStyle wallpaperAspectStyle READ wallpaperAspectStyle NOTIFY wallpaperChanged FINAL)

public:
    Q_NODISCARD static FramelessManager *instance();

    Q_NODISCARD Global::SystemTheme systemTheme() const;
    Q_NODISCARD QColor systemAccentColor() const;
    Q_NODISCARD QString wallpaper() const;
    Q_NODISCARD Global::WallpaperAspectStyle wallpaperAspectStyle() const;

public Q_SLOTS:
    void addWindow(const SystemParameters *params);
    void removeWindow(const WId windowId);
    void setOverrideTheme(const Global::SystemTheme theme);

Q_SIGNALS:
    void systemThemeChanged();
    void wallpaperChanged();

private:
    explicit FramelessManager(QObject *parent = nullptr);
    ~FramelessManager() override;

private:
    QScopedPointer<FramelessManagerPrivate> d_ptr;
};

FRAMELESSHELPER_END_NAMESPACE
