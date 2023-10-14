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
#include <optional>

#if FRAMELESSHELPER_CONFIG(system_button)

FRAMELESSHELPER_BEGIN_NAMESPACE

class StandardSystemButton;
class FRAMELESSHELPER_WIDGETS_API StandardSystemButtonPrivate : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PRIVATE_QT_CLASS(StandardSystemButton)

public:
    explicit StandardSystemButtonPrivate(StandardSystemButton *q);
    ~StandardSystemButtonPrivate() override;

    Q_NODISCARD static QSize getRecommendedButtonSize();

    Global::SystemButtonType buttonType = Global::SystemButtonType::Unknown;
    QString glyph = {};
    QColor hoverColor = {};
    QColor pressColor = {};
    QColor normalColor = {};
    QColor activeForegroundColor = {};
    QColor inactiveForegroundColor = {};
    bool active = false;
    std::optional<int> glyphSize = std::nullopt;
};

FRAMELESSHELPER_END_NAMESPACE

#endif
