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

#include "chromepalette.h"
#include "chromepalette_p.h"

#if FRAMELESSHELPER_CONFIG(titlebar)

#include "framelessmanager.h"
#include "utils.h"
#include <QtCore/qloggingcategory.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcChromePalette, "wangwenx190.framelesshelper.core.chromepalette")
#  define INFO qCInfo(lcChromePalette)
#  define DEBUG qCDebug(lcChromePalette)
#  define WARNING qCWarning(lcChromePalette)
#  define CRITICAL qCCritical(lcChromePalette)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

ChromePalettePrivate::ChromePalettePrivate(ChromePalette *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, &ChromePalettePrivate::refresh);
    refresh();
}

ChromePalettePrivate::~ChromePalettePrivate() = default;

ChromePalettePrivate *ChromePalettePrivate::get(ChromePalette *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

const ChromePalettePrivate *ChromePalettePrivate::get(const ChromePalette *q)
{
    Q_ASSERT(q);
    if (!q) {
        return nullptr;
    }
    return q->d_func();
}

void ChromePalettePrivate::refresh()
{
    const bool colorized = Utils::isTitleBarColorized();
    const bool dark = (FramelessManager::instance()->systemTheme() == SystemTheme::Dark);
    titleBarActiveBackgroundColor_sys = [colorized, dark]() -> QColor {
        if (colorized) {
            return Utils::getAccentColor();
        } else {
            return (dark ? kDefaultBlackColor : kDefaultWhiteColor);
        }
    }();
    titleBarInactiveBackgroundColor_sys = (dark ? kDefaultSystemDarkColor : kDefaultWhiteColor);
    titleBarActiveForegroundColor_sys = [this, dark, colorized]() -> QColor {
        if (dark || colorized) {
            // Calculate the most appropriate foreground color, based on the
            // current background color.
            const qreal grayF = (
                (qreal(0.299) * titleBarActiveBackgroundColor_sys.redF()) +
                (qreal(0.587) * titleBarActiveBackgroundColor_sys.greenF()) +
                (qreal(0.114) * titleBarActiveBackgroundColor_sys.blueF()));
            static constexpr const auto kFlag = qreal(0.5);
            if ((grayF < kFlag) || qFuzzyCompare(grayF, kFlag)) {
                return kDefaultWhiteColor;
            }
        }
        return kDefaultBlackColor;
    }();
    titleBarInactiveForegroundColor_sys = kDefaultDarkGrayColor;
    chromeButtonNormalColor_sys = kDefaultTransparentColor;
    chromeButtonHoverColor_sys =
        Utils::calculateSystemButtonBackgroundColor(SystemButtonType::Minimize, ButtonState::Hovered);
    chromeButtonPressColor_sys =
        Utils::calculateSystemButtonBackgroundColor(SystemButtonType::Minimize, ButtonState::Pressed);
    closeButtonNormalColor_sys = kDefaultTransparentColor;
    closeButtonHoverColor_sys =
        Utils::calculateSystemButtonBackgroundColor(SystemButtonType::Close, ButtonState::Hovered);
    closeButtonPressColor_sys =
        Utils::calculateSystemButtonBackgroundColor(SystemButtonType::Close, ButtonState::Pressed);
    Q_Q(ChromePalette);
    Q_EMIT q->titleBarActiveBackgroundColorChanged();
    Q_EMIT q->titleBarInactiveBackgroundColorChanged();
    Q_EMIT q->titleBarActiveForegroundColorChanged();
    Q_EMIT q->titleBarInactiveForegroundColorChanged();
    Q_EMIT q->chromeButtonNormalColorChanged();
    Q_EMIT q->chromeButtonHoverColorChanged();
    Q_EMIT q->chromeButtonPressColorChanged();
    Q_EMIT q->closeButtonNormalColorChanged();
    Q_EMIT q->closeButtonHoverColorChanged();
    Q_EMIT q->closeButtonPressColorChanged();
    Q_EMIT q->titleBarColorChanged();
    Q_EMIT q->chromeButtonColorChanged();
}

ChromePalette::ChromePalette(QObject *parent) :
    QObject(parent), d_ptr(std::make_unique<ChromePalettePrivate>(this))
{
}

ChromePalette::~ChromePalette() = default;

QColor ChromePalette::titleBarActiveBackgroundColor() const
{
    Q_D(const ChromePalette);
    return d->titleBarActiveBackgroundColor.value_or(d->titleBarActiveBackgroundColor_sys);
}

QColor ChromePalette::titleBarInactiveBackgroundColor() const
{
    Q_D(const ChromePalette);
    return d->titleBarInactiveBackgroundColor.value_or(d->titleBarInactiveBackgroundColor_sys);
}

QColor ChromePalette::titleBarActiveForegroundColor() const
{
    Q_D(const ChromePalette);
    return d->titleBarActiveForegroundColor.value_or(d->titleBarActiveForegroundColor_sys);
}

QColor ChromePalette::titleBarInactiveForegroundColor() const
{
    Q_D(const ChromePalette);
    return d->titleBarInactiveForegroundColor.value_or(d->titleBarInactiveForegroundColor_sys);
}

QColor ChromePalette::chromeButtonNormalColor() const
{
    Q_D(const ChromePalette);
    return d->chromeButtonNormalColor.value_or(d->chromeButtonNormalColor_sys);
}

QColor ChromePalette::chromeButtonHoverColor() const
{
    Q_D(const ChromePalette);
    return d->chromeButtonHoverColor.value_or(d->chromeButtonHoverColor_sys);
}

QColor ChromePalette::chromeButtonPressColor() const
{
    Q_D(const ChromePalette);
    return d->chromeButtonPressColor.value_or(d->chromeButtonPressColor_sys);
}

QColor ChromePalette::closeButtonNormalColor() const
{
    Q_D(const ChromePalette);
    return d->closeButtonNormalColor.value_or(d->closeButtonNormalColor_sys);
}

QColor ChromePalette::closeButtonHoverColor() const
{
    Q_D(const ChromePalette);
    return d->closeButtonHoverColor.value_or(d->closeButtonHoverColor_sys);
}

QColor ChromePalette::closeButtonPressColor() const
{
    Q_D(const ChromePalette);
    return d->closeButtonPressColor.value_or(d->closeButtonPressColor_sys);
}

void ChromePalette::setTitleBarActiveBackgroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->titleBarActiveBackgroundColor.value_or(QColor()) == value) {
        return;
    }
    d->titleBarActiveBackgroundColor = value;
    Q_EMIT titleBarActiveBackgroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::resetTitleBarActiveBackgroundColor()
{
    Q_D(ChromePalette);
    d->titleBarActiveBackgroundColor = std::nullopt;
    Q_EMIT titleBarActiveBackgroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::setTitleBarInactiveBackgroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->titleBarInactiveBackgroundColor.value_or(QColor()) == value) {
        return;
    }
    d->titleBarInactiveBackgroundColor = value;
    Q_EMIT titleBarInactiveBackgroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::resetTitleBarInactiveBackgroundColor()
{
    Q_D(ChromePalette);
    d->titleBarInactiveBackgroundColor = std::nullopt;
    Q_EMIT titleBarInactiveBackgroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::setTitleBarActiveForegroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->titleBarActiveForegroundColor.value_or(QColor()) == value) {
        return;
    }
    d->titleBarActiveForegroundColor = value;
    Q_EMIT titleBarActiveForegroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::resetTitleBarActiveForegroundColor()
{
    Q_D(ChromePalette);
    d->titleBarActiveForegroundColor = std::nullopt;
    Q_EMIT titleBarActiveForegroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::setTitleBarInactiveForegroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->titleBarInactiveForegroundColor.value_or(QColor()) == value) {
        return;
    }
    d->titleBarInactiveForegroundColor = value;
    Q_EMIT titleBarInactiveForegroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::resetTitleBarInactiveForegroundColor()
{
    Q_D(ChromePalette);
    d->titleBarInactiveForegroundColor = std::nullopt;
    Q_EMIT titleBarInactiveForegroundColorChanged();
    Q_EMIT titleBarColorChanged();
}

void ChromePalette::setChromeButtonNormalColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->chromeButtonNormalColor.value_or(QColor()) == value) {
        return;
    }
    d->chromeButtonNormalColor = value;
    Q_EMIT chromeButtonNormalColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetChromeButtonNormalColor()
{
    Q_D(ChromePalette);
    d->chromeButtonNormalColor = std::nullopt;
    Q_EMIT chromeButtonNormalColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::setChromeButtonHoverColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->chromeButtonHoverColor.value_or(QColor()) == value) {
        return;
    }
    d->chromeButtonHoverColor = value;
    Q_EMIT chromeButtonHoverColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetChromeButtonHoverColor()
{
    Q_D(ChromePalette);
    d->chromeButtonHoverColor = std::nullopt;
    Q_EMIT chromeButtonHoverColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::setChromeButtonPressColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->chromeButtonPressColor.value_or(QColor()) == value) {
        return;
    }
    d->chromeButtonPressColor = value;
    Q_EMIT chromeButtonPressColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetChromeButtonPressColor()
{
    Q_D(ChromePalette);
    d->chromeButtonPressColor = std::nullopt;
    Q_EMIT chromeButtonPressColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::setCloseButtonNormalColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->closeButtonNormalColor.value_or(QColor()) == value) {
        return;
    }
    d->closeButtonNormalColor = value;
    Q_EMIT closeButtonNormalColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetCloseButtonNormalColor()
{
    Q_D(ChromePalette);
    d->closeButtonNormalColor = std::nullopt;
    Q_EMIT closeButtonNormalColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::setCloseButtonHoverColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->closeButtonHoverColor.value_or(QColor()) == value) {
        return;
    }
    d->closeButtonHoverColor = value;
    Q_EMIT closeButtonHoverColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetCloseButtonHoverColor()
{
    Q_D(ChromePalette);
    d->closeButtonHoverColor = std::nullopt;
    Q_EMIT closeButtonHoverColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::setCloseButtonPressColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(ChromePalette);
    if (d->closeButtonPressColor.value_or(QColor()) == value) {
        return;
    }
    d->closeButtonPressColor = value;
    Q_EMIT closeButtonPressColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

void ChromePalette::resetCloseButtonPressColor()
{
    Q_D(ChromePalette);
    d->closeButtonPressColor = std::nullopt;
    Q_EMIT closeButtonPressColorChanged();
    Q_EMIT chromeButtonColorChanged();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
