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

#include "standardsystembutton.h"
#include "standardsystembutton_p.h"

#if FRAMELESSHELPER_CONFIG(system_button)

#include <FramelessHelper/Core/utils.h>
#include <FramelessHelper/Core/private/framelessmanager_p.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qtooltip.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcStandardSystemButton, "wangwenx190.framelesshelper.widgets.standardsystembutton")
#  define INFO qCInfo(lcStandardSystemButton)
#  define DEBUG qCDebug(lcStandardSystemButton)
#  define WARNING qCWarning(lcStandardSystemButton)
#  define CRITICAL qCCritical(lcStandardSystemButton)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

StandardSystemButtonPrivate::StandardSystemButtonPrivate(StandardSystemButton *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
}

StandardSystemButtonPrivate::~StandardSystemButtonPrivate() = default;

StandardSystemButtonPrivate *StandardSystemButtonPrivate::get(StandardSystemButton *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const StandardSystemButtonPrivate *StandardSystemButtonPrivate::get(const StandardSystemButton *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

QSize StandardSystemButtonPrivate::getRecommendedButtonSize()
{
    return kDefaultSystemButtonSize;
}

StandardSystemButton::StandardSystemButton(QWidget *parent)
    : QPushButton(parent), d_ptr(std::make_unique<StandardSystemButtonPrivate>(this))
{
    FramelessManagerPrivate::initializeIconFont();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(StandardSystemButtonPrivate::getRecommendedButtonSize());
    setIconSize(kDefaultSystemButtonIconSize);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
}

StandardSystemButton::StandardSystemButton(const SystemButtonType type, QWidget *parent)
    : StandardSystemButton(parent)
{
    setButtonType(type);
}

StandardSystemButton::~StandardSystemButton() = default;

QSize StandardSystemButton::sizeHint() const
{
    return StandardSystemButtonPrivate::getRecommendedButtonSize();
}

QString StandardSystemButton::glyph() const
{
    Q_D(const StandardSystemButton);
    return d->glyph;
}

void StandardSystemButton::setGlyph(const QString &value)
{
    Q_ASSERT(!value.isEmpty());
    if (value.isEmpty()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->glyph == value) {
        return;
    }
    d->glyph = value;
    update();
    Q_EMIT glyphChanged();
}

SystemButtonType StandardSystemButton::buttonType() const
{
    Q_D(const StandardSystemButton);
    return d->buttonType;
}

void StandardSystemButton::setButtonType(const SystemButtonType type)
{
    Q_ASSERT(type != SystemButtonType::Unknown);
    if (type == SystemButtonType::Unknown) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->buttonType == type) {
        return;
    }
    d->buttonType = type;
    setGlyph(Utils::getSystemButtonGlyph(d->buttonType));
    update();
}

QColor StandardSystemButton::hoverColor() const
{
    Q_D(const StandardSystemButton);
    return d->hoverColor;
}

QColor StandardSystemButton::pressColor() const
{
    Q_D(const StandardSystemButton);
    return d->pressColor;
}

QColor StandardSystemButton::normalColor() const
{
    Q_D(const StandardSystemButton);
    return d->normalColor;
}

QColor StandardSystemButton::activeForegroundColor() const
{
    Q_D(const StandardSystemButton);
    return d->activeForegroundColor;
}

QColor StandardSystemButton::inactiveForegroundColor() const
{
    Q_D(const StandardSystemButton);
    return d->inactiveForegroundColor;
}

bool StandardSystemButton::isActive() const
{
    Q_D(const StandardSystemButton);
    return d->active;
}

int StandardSystemButton::glyphSize() const
{
    Q_D(const StandardSystemButton);
    return d->glyphSize.value_or(FramelessManagerPrivate::getIconFont().pointSize());
}

void StandardSystemButton::setHoverColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->hoverColor == value) {
        return;
    }
    d->hoverColor = value;
    update();
    Q_EMIT hoverColorChanged();
}

void StandardSystemButton::setPressColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->pressColor == value) {
        return;
    }
    d->pressColor = value;
    update();
    Q_EMIT pressColorChanged();
}

void StandardSystemButton::setNormalColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->normalColor == value) {
        return;
    }
    d->normalColor = value;
    update();
    Q_EMIT normalColorChanged();
}

void StandardSystemButton::setActiveForegroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->activeForegroundColor == value) {
        return;
    }
    d->activeForegroundColor = value;
    update();
    Q_EMIT activeForegroundColorChanged();
}

void StandardSystemButton::setInactiveForegroundColor(const QColor &value)
{
    Q_ASSERT(value.isValid());
    if (!value.isValid()) {
        return;
    }
    Q_D(StandardSystemButton);
    if (d->inactiveForegroundColor == value) {
        return;
    }
    d->inactiveForegroundColor = value;
    update();
    Q_EMIT inactiveForegroundColorChanged();
}

void StandardSystemButton::setActive(const bool value)
{
    Q_D(StandardSystemButton);
    if (d->active == value) {
        return;
    }
    d->active = value;
    update();
    Q_EMIT activeChanged();
}

void StandardSystemButton::setGlyphSize(const int value)
{
    Q_ASSERT(value > 0);
    if (value <= 0) {
        return;
    }
    if (glyphSize() == value) {
        return;
    }
    Q_D(StandardSystemButton);
    d->glyphSize = value;
    update();
    Q_EMIT glyphSizeChanged();
}

void StandardSystemButton::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(event);
    if (!event) {
        return;
    }
    Q_D(StandardSystemButton);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing
                           | QPainter::SmoothPixmapTransform);
    const auto backgroundColor = [this, d]() -> QColor {
        // The pressed state has higher priority than the hovered state.
        if (isDown() && d->pressColor.isValid()) {
            return d->pressColor;
        }
        if (underMouse() && d->hoverColor.isValid()) {
            return d->hoverColor;
        }
        if (d->normalColor.isValid()) {
            return d->normalColor;
        }
        return {};
    }();
    const QRect buttonRect = {QPoint(0, 0), size()};
    if (backgroundColor.isValid()) {
        painter.fillRect(buttonRect, backgroundColor);
    }
    if (!d->glyph.isEmpty()) {
        painter.setPen([this, d]() -> QColor {
            if (!underMouse() && !d->active && d->inactiveForegroundColor.isValid()) {
                return d->inactiveForegroundColor;
            }
            if (d->activeForegroundColor.isValid()) {
                return d->activeForegroundColor;
            }
            return kDefaultBlackColor;
        }());
        painter.setFont([d]() -> QFont {
            QFont font = FramelessManagerPrivate::getIconFont();
            if (d->glyphSize.has_value()) {
                font.setPointSize(d->glyphSize.value());
            }
            return font;
        }());
        painter.drawText(buttonRect, Qt::AlignCenter, d->glyph);
    }
    painter.restore();
    event->accept();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
