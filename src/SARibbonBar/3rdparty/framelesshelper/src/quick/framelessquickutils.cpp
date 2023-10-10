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

#include "framelessquickutils.h"
#include <FramelessHelper/Core/framelessmanager.h>
#include <FramelessHelper/Core/utils.h>
#ifdef Q_OS_WINDOWS
#  include <FramelessHelper/Core/private/winverhelper_p.h>
#endif // Q_OS_WINDOWS
#include <QtCore/qloggingcategory.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessQuickUtils, "wangwenx190.framelesshelper.quick.framelessquickutils")
#  define INFO qCInfo(lcFramelessQuickUtils)
#  define DEBUG qCDebug(lcFramelessQuickUtils)
#  define WARNING qCWarning(lcFramelessQuickUtils)
#  define CRITICAL qCCritical(lcFramelessQuickUtils)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

FramelessQuickUtils::FramelessQuickUtils(QObject *parent) : QObject(parent)
{
    connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, [this](){
        Q_EMIT systemThemeChanged();
        Q_EMIT systemAccentColorChanged();
        Q_EMIT titleBarColorizedChanged();
    });
}

FramelessQuickUtils::~FramelessQuickUtils() = default;

qreal FramelessQuickUtils::titleBarHeight() const
{
    return kDefaultTitleBarHeight;
}

bool FramelessQuickUtils::frameBorderVisible() const
{
#ifdef Q_OS_WINDOWS
    return (Utils::isWindowFrameBorderVisible() && !WindowsVersionHelper::isWin11OrGreater());
#else
    return false;
#endif
}

qreal FramelessQuickUtils::frameBorderThickness() const
{
#ifdef Q_OS_WINDOWS
    return kDefaultWindowFrameBorderThickness;
#else
    return 0;
#endif
}

QuickGlobal::SystemTheme FramelessQuickUtils::systemTheme() const
{
    return FRAMELESSHELPER_ENUM_CORE_TO_QUICK(SystemTheme, FramelessManager::instance()->systemTheme());
}

void FramelessQuickUtils::setOverrideTheme(const QuickGlobal::SystemTheme theme)
{
    FramelessManager::instance()->setOverrideTheme(FRAMELESSHELPER_ENUM_QUICK_TO_CORE(SystemTheme, theme));
}

QColor FramelessQuickUtils::systemAccentColor() const
{
    return Utils::getAccentColor();
}

bool FramelessQuickUtils::titleBarColorized() const
{
    return Utils::isTitleBarColorized();
}

QColor FramelessQuickUtils::defaultSystemLightColor() const
{
    return kDefaultSystemLightColor;
}

QColor FramelessQuickUtils::defaultSystemDarkColor() const
{
    return kDefaultSystemDarkColor;
}

QSizeF FramelessQuickUtils::defaultSystemButtonSize() const
{
    return kDefaultSystemButtonSize;
}

QSizeF FramelessQuickUtils::defaultSystemButtonIconSize() const
{
    return kDefaultSystemButtonIconSize;
}

QColor FramelessQuickUtils::defaultSystemButtonBackgroundColor() const
{
    return kDefaultSystemButtonBackgroundColor;
}

QColor FramelessQuickUtils::defaultSystemCloseButtonBackgroundColor() const
{
    return kDefaultSystemCloseButtonBackgroundColor;
}

QColor FramelessQuickUtils::getSystemButtonBackgroundColor(const QuickGlobal::SystemButtonType button,
                                                           const QuickGlobal::ButtonState state)
{
    return Utils::calculateSystemButtonBackgroundColor(
        FRAMELESSHELPER_ENUM_QUICK_TO_CORE(SystemButtonType, button),
        FRAMELESSHELPER_ENUM_QUICK_TO_CORE(ButtonState, state));
}

void FramelessQuickUtils::classBegin()
{
}

void FramelessQuickUtils::componentComplete()
{
}

FRAMELESSHELPER_END_NAMESPACE
