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

#if FRAMELESSHELPER_CONFIG(titlebar)

FRAMELESSHELPER_BEGIN_NAMESPACE

class ChromePalettePrivate;
class FRAMELESSHELPER_CORE_API ChromePalette : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_PUBLIC_QT_CLASS(ChromePalette)

    Q_PROPERTY(QColor titleBarActiveBackgroundColor READ titleBarActiveBackgroundColor
               WRITE setTitleBarActiveBackgroundColor RESET resetTitleBarActiveBackgroundColor
               NOTIFY titleBarActiveBackgroundColorChanged FINAL)
    Q_PROPERTY(QColor titleBarInactiveBackgroundColor READ titleBarInactiveBackgroundColor
               WRITE setTitleBarInactiveBackgroundColor RESET resetTitleBarInactiveBackgroundColor
               NOTIFY titleBarInactiveBackgroundColorChanged FINAL)
    Q_PROPERTY(QColor titleBarActiveForegroundColor READ titleBarActiveForegroundColor
               WRITE setTitleBarActiveForegroundColor RESET resetTitleBarActiveForegroundColor
               NOTIFY titleBarActiveForegroundColorChanged FINAL)
    Q_PROPERTY(QColor titleBarInactiveForegroundColor READ titleBarInactiveForegroundColor
               WRITE setTitleBarInactiveForegroundColor RESET resetTitleBarInactiveForegroundColor
               NOTIFY titleBarInactiveForegroundColorChanged FINAL)
    Q_PROPERTY(QColor chromeButtonNormalColor READ chromeButtonNormalColor
               WRITE setChromeButtonNormalColor RESET resetChromeButtonNormalColor
               NOTIFY chromeButtonNormalColorChanged FINAL)
    Q_PROPERTY(QColor chromeButtonHoverColor READ chromeButtonHoverColor
               WRITE setChromeButtonHoverColor RESET resetChromeButtonHoverColor
               NOTIFY chromeButtonHoverColorChanged FINAL)
    Q_PROPERTY(QColor chromeButtonPressColor READ chromeButtonPressColor
               WRITE setChromeButtonPressColor RESET resetChromeButtonPressColor
               NOTIFY chromeButtonPressColorChanged FINAL)
    Q_PROPERTY(QColor closeButtonNormalColor READ closeButtonNormalColor
               WRITE setCloseButtonNormalColor RESET resetCloseButtonNormalColor
               NOTIFY closeButtonNormalColorChanged FINAL)
    Q_PROPERTY(QColor closeButtonHoverColor READ closeButtonHoverColor
               WRITE setCloseButtonHoverColor RESET resetCloseButtonHoverColor
               NOTIFY closeButtonHoverColorChanged FINAL)
    Q_PROPERTY(QColor closeButtonPressColor READ closeButtonPressColor
               WRITE setCloseButtonPressColor RESET resetCloseButtonPressColor
               NOTIFY closeButtonPressColorChanged FINAL)

public:
    explicit ChromePalette(QObject *parent = nullptr);
    ~ChromePalette() override;

    Q_NODISCARD QColor titleBarActiveBackgroundColor() const;
    Q_NODISCARD QColor titleBarInactiveBackgroundColor() const;
    Q_NODISCARD QColor titleBarActiveForegroundColor() const;
    Q_NODISCARD QColor titleBarInactiveForegroundColor() const;
    Q_NODISCARD QColor chromeButtonNormalColor() const;
    Q_NODISCARD QColor chromeButtonHoverColor() const;
    Q_NODISCARD QColor chromeButtonPressColor() const;
    Q_NODISCARD QColor closeButtonNormalColor() const;
    Q_NODISCARD QColor closeButtonHoverColor() const;
    Q_NODISCARD QColor closeButtonPressColor() const;

public Q_SLOTS:
    void setTitleBarActiveBackgroundColor(const QColor &value);
    void resetTitleBarActiveBackgroundColor();
    void setTitleBarInactiveBackgroundColor(const QColor &value);
    void resetTitleBarInactiveBackgroundColor();
    void setTitleBarActiveForegroundColor(const QColor &value);
    void resetTitleBarActiveForegroundColor();
    void setTitleBarInactiveForegroundColor(const QColor &value);
    void resetTitleBarInactiveForegroundColor();
    void setChromeButtonNormalColor(const QColor &value);
    void resetChromeButtonNormalColor();
    void setChromeButtonHoverColor(const QColor &value);
    void resetChromeButtonHoverColor();
    void setChromeButtonPressColor(const QColor &value);
    void resetChromeButtonPressColor();
    void setCloseButtonNormalColor(const QColor &value);
    void resetCloseButtonNormalColor();
    void setCloseButtonHoverColor(const QColor &value);
    void resetCloseButtonHoverColor();
    void setCloseButtonPressColor(const QColor &value);
    void resetCloseButtonPressColor();

Q_SIGNALS:
    void titleBarActiveBackgroundColorChanged();
    void titleBarInactiveBackgroundColorChanged();
    void titleBarActiveForegroundColorChanged();
    void titleBarInactiveForegroundColorChanged();
    void chromeButtonNormalColorChanged();
    void chromeButtonHoverColorChanged();
    void chromeButtonPressColorChanged();
    void closeButtonNormalColorChanged();
    void closeButtonHoverColorChanged();
    void closeButtonPressColorChanged();
    void titleBarColorChanged();
    void chromeButtonColorChanged();
};

FRAMELESSHELPER_END_NAMESPACE

#endif
