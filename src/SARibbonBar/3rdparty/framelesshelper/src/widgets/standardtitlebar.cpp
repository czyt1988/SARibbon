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

#include "standardtitlebar.h"
#include "standardtitlebar_p.h"

#if FRAMELESSHELPER_CONFIG(titlebar)

#if FRAMELESSHELPER_CONFIG(system_button)
#  include "standardsystembutton.h"
#endif
#include "framelesswidgetshelper.h"
#include <FramelessHelper/Core/utils.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qtimer.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qscreen.h>
#include <QtGui/qguiapplication.h>
#include <QtWidgets/qboxlayout.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcStandardTitleBar, "wangwenx190.framelesshelper.widgets.standardtitlebar")
#  define INFO qCInfo(lcStandardTitleBar)
#  define DEBUG qCDebug(lcStandardTitleBar)
#  define WARNING qCWarning(lcStandardTitleBar)
#  define CRITICAL qCCritical(lcStandardTitleBar)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static inline void emulateLeaveEvent(QWidget *widget)
{
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    QTimer::singleShot(0, widget, [widget](){
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen *screen = widget->screen();
#else
        const QScreen *screen = widget->windowHandle()->screen();
#endif
        const QPoint globalPos = QCursor::pos(screen);
        if (!QRect(widget->mapToGlobal(QPoint{ 0, 0 }), widget->size()).contains(globalPos)) {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
            if (widget->testAttribute(Qt::WA_Hover)) {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
}

StandardTitleBarPrivate::StandardTitleBarPrivate(StandardTitleBar *q) : QObject(q)
{
    Q_ASSERT(q);
    if (!q) {
        return;
    }
    q_ptr = q;
    initialize();
}

StandardTitleBarPrivate::~StandardTitleBarPrivate() = default;

StandardTitleBarPrivate *StandardTitleBarPrivate::get(StandardTitleBar *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

const StandardTitleBarPrivate *StandardTitleBarPrivate::get(const StandardTitleBar *pub)
{
    Q_ASSERT(pub);
    if (!pub) {
        return nullptr;
    }
    return pub->d_func();
}

QFont StandardTitleBarPrivate::defaultFont() const
{
    Q_Q(const StandardTitleBar);
    QFont font = q->font();
    font.setPointSize(kDefaultTitleBarFontPointSize);
    return font;
}

StandardTitleBarPrivate::FontMetrics StandardTitleBarPrivate::titleLabelSize() const
{
    if (!window) {
        return {};
    }
    const QString text = window->windowTitle();
    if (text.isEmpty()) {
        return {};
    }
    const QFont font = titleFont.value_or(defaultFont());
    const QFontMetrics fontMetrics(font);
    const int textWidth = Utils::horizontalAdvance(fontMetrics, text);
    return {
        /* .width */ std::min(textWidth, titleLabelMaxWidth()),
        /* .height */ fontMetrics.height(),
        /* .ascent */ fontMetrics.ascent()
    };
}

int StandardTitleBarPrivate::titleLabelMaxWidth() const
{
#if (FRAMELESSHELPER_CONFIG(system_button) && !defined(Q_OS_MACOS))
    const int chromeButtonAreaWidth = (closeButton->x() + closeButton->width() - minimizeButton->x());
#else
    static constexpr const int chromeButtonAreaWidth = 70;
#endif
    Q_Q(const StandardTitleBar);
    int textMaxWidth = q->width();
    if ((labelAlignment & Qt::AlignLeft) || (labelAlignment & Qt::AlignRight)) {
        textMaxWidth -= (windowIconRect().width() + chromeButtonAreaWidth + (kDefaultTitleBarContentsMargin * 2));
    } else if (labelAlignment & Qt::AlignHCenter) {
        textMaxWidth -= ((chromeButtonAreaWidth + kDefaultTitleBarContentsMargin) * 2);
    } else {
        textMaxWidth = std::round(qreal(textMaxWidth) * qreal(0.8));
    }
    return std::max(textMaxWidth, 0);
}

bool StandardTitleBarPrivate::mouseEventHandler(QMouseEvent *event)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(event);
    return false;
#else // !Q_OS_MACOS
    Q_ASSERT(event);
    if (!event) {
        return false;
    }
    Q_Q(const StandardTitleBar);
    const Qt::MouseButton button = event->button();
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    const QPoint scenePos = event->scenePosition().toPoint();
#  else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    const QPoint scenePos = event->windowPos().toPoint();
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    const bool interestArea = isInTitleBarIconArea(scenePos);
    switch (event->type()) {
    case QEvent::MouseButtonRelease:
        // We need a valid top level widget here.
        if (window && interestArea) {
            // Sadly the mouse release events are always triggered before the
            // mouse double click events, and if we intercept the mouse release
            // events here, we'll never get the double click events afterwards,
            // so we have to wait for a little bit to make sure the double
            // click events are handled first, before we actually handle the
            // mouse release events here.
            // We need to wait long enough because the time interval between these
            // events is really really short, if the delay time is not long enough,
            // we still can't trigger the double click event due to we have handled
            // the mouse release events here already. But we also can't wait too
            // long, otherwise the system menu will show up too late, which is not
            // a good user experience. In my experiments, I found that 150ms is
            // the minimum value we can use here.
            // We need a copy of the "scenePos" variable here, otherwise it will
            // soon fall out of scope when the lambda function actually runs.
            QTimer::singleShot(150, this, [this, button, q, scenePos](){
                // The close event is already triggered, don't try to show the
                // system menu anymore, otherwise it will prevent our window
                // from closing.
                if (closeTriggered) {
                    return;
                }
                // Please refer to the comments in StandardTitleBarPrivate::setWindowIconVisible().
                FramelessWidgetsHelper::get(window)->showSystemMenu([button, q, &scenePos, this]() -> QPoint {
                    QPoint pos = scenePos;
                    if (button == Qt::LeftButton) {
                        pos = {0, q->height()};
                    }
                    return window->mapToGlobal(pos);
                }());
            });
            // Don't eat this event, we have not handled it yet.
        }
        break;
    case QEvent::MouseButtonDblClick:
        // We need a valid top level widget here.
        if (window && (button == Qt::LeftButton) && interestArea) {
            closeTriggered = true;
            window->close();
            // Eat this event, we have handled it here.
            event->accept();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
#endif // Q_OS_MACOS
}

QRect StandardTitleBarPrivate::windowIconRect() const
{
    Q_Q(const StandardTitleBar);
    if (!windowIconVisible_real()) {
        return {};
    }
    const QSize size = q->windowIconSize();
#ifdef Q_OS_MACOS
    const auto x = [this, q, &size]() -> int {
        if (labelAlignment & Qt::AlignLeft) {
            return (kMacOSChromeButtonAreaWidth + kDefaultTitleBarContentsMargin);
        }
        const int titleBarWidth = q->width();
        const int labelWidth = titleLabelSize().width;
        if (labelAlignment & Qt::AlignRight) {
            // We need two spacer here, one is on the right edge of the title bar,
            // the other one is between the window icon and the window label.
            return (titleBarWidth - kDefaultTitleBarContentsMargin
                    - labelWidth - kDefaultTitleBarContentsMargin - size.width());
        }
        if (labelAlignment & Qt::AlignHCenter) {
            const int centeredX = std::round(qreal(titleBarWidth - labelWidth) / qreal(2));
            return (centeredX - kDefaultTitleBarContentsMargin - size.width());
        }
        WARNING << "The alignment for the title label is not set!";
        return 0;
    }();
#else // !Q_OS_MACOS
    const int x = kDefaultTitleBarContentsMargin;
#endif // Q_OS_MACOS
    const int y = std::round(qreal(q->height() - size.height()) / qreal(2));
    return {QPoint(x, y), size};
}

bool StandardTitleBarPrivate::windowIconVisible_real() const
{
    return (windowIconVisible && !window->windowIcon().isNull());
}

bool StandardTitleBarPrivate::isInTitleBarIconArea(const QPoint &pos) const
{
    if (!windowIconVisible_real()) {
        return false;
    }
    return windowIconRect().contains(pos);
}

void StandardTitleBarPrivate::updateMaximizeButton()
{
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    const bool max = window->isMaximized();
    maximizeButton->setButtonType(max ? SystemButtonType::Restore : SystemButtonType::Maximize);
    maximizeButton->setToolTip(max ? tr("Restore") : tr("Maximize"));
#endif
}

void StandardTitleBarPrivate::updateTitleBarColor()
{
    Q_Q(StandardTitleBar);
    q->update();
}

void StandardTitleBarPrivate::updateChromeButtonColor()
{
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    const bool active = window->isActiveWindow();
    const QColor activeForeground = chromePalette->titleBarActiveForegroundColor();
    const QColor inactiveForeground = chromePalette->titleBarInactiveForegroundColor();
    const QColor normal = chromePalette->chromeButtonNormalColor();
    const QColor hover = chromePalette->chromeButtonHoverColor();
    const QColor press = chromePalette->chromeButtonPressColor();
    minimizeButton->setActiveForegroundColor(activeForeground);
    minimizeButton->setInactiveForegroundColor(inactiveForeground);
    minimizeButton->setNormalColor(normal);
    minimizeButton->setHoverColor(hover);
    minimizeButton->setPressColor(press);
    minimizeButton->setActive(active);
    maximizeButton->setActiveForegroundColor(activeForeground);
    maximizeButton->setInactiveForegroundColor(inactiveForeground);
    maximizeButton->setNormalColor(normal);
    maximizeButton->setHoverColor(hover);
    maximizeButton->setPressColor(press);
    maximizeButton->setActive(active);
    closeButton->setActiveForegroundColor(activeForeground);
    closeButton->setInactiveForegroundColor(inactiveForeground);
    closeButton->setNormalColor(chromePalette->closeButtonNormalColor());
    closeButton->setHoverColor(chromePalette->closeButtonHoverColor());
    closeButton->setPressColor(chromePalette->closeButtonPressColor());
    closeButton->setActive(active);
#endif
}

void StandardTitleBarPrivate::retranslateUi()
{
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
    minimizeButton->setToolTip(tr("Minimize"));
    maximizeButton->setToolTip(window->isMaximized() ? tr("Restore") : tr("Maximize"));
    closeButton->setToolTip(tr("Close"));
#endif
}

bool StandardTitleBarPrivate::eventFilter(QObject *object, QEvent *event)
{
    Q_ASSERT(object);
    Q_ASSERT(event);
    if (!object || !event) {
        return false;
    }
    if (!object->isWidgetType()) {
        return QObject::eventFilter(object, event);
    }
    const auto widget = qobject_cast<QWidget *>(object);
    if (!widget->isWindow() || (widget != window)) {
        return QObject::eventFilter(object, event);
    }
    switch (event->type()) {
    case QEvent::WindowStateChange:
        updateMaximizeButton();
        break;
    case QEvent::ActivationChange: {
        updateTitleBarColor();
        updateChromeButtonColor();
    } break;
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void StandardTitleBarPrivate::initialize()
{
    Q_Q(StandardTitleBar);
    window = q->window();
    chromePalette = new ChromePalette(this);
    connect(chromePalette, &ChromePalette::titleBarColorChanged,
        this, &StandardTitleBarPrivate::updateTitleBarColor);
    connect(chromePalette, &ChromePalette::chromeButtonColorChanged,
        this, &StandardTitleBarPrivate::updateChromeButtonColor);
    connect(window, &QWidget::windowIconChanged, this, [q](const QIcon &icon){
        Q_UNUSED(icon);
        q->update();
    });
    connect(window, &QWidget::windowTitleChanged, this, [q](const QString &title){
        Q_UNUSED(title);
        q->update();
    });
#ifdef Q_OS_MACOS
    const auto titleBarLayout = new QHBoxLayout(q);
    titleBarLayout->setSpacing(0);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
#elif FRAMELESSHELPER_CONFIG(system_button)
    minimizeButton = new StandardSystemButton(SystemButtonType::Minimize, q);
    connect(minimizeButton, &StandardSystemButton::clicked, window, &QWidget::showMinimized);
    maximizeButton = new StandardSystemButton(SystemButtonType::Maximize, q);
    updateMaximizeButton();
    connect(maximizeButton, &StandardSystemButton::clicked, this, [this](){
        if (window->isMaximized()) {
            window->showNormal();
        } else {
            window->showMaximized();
        }

        // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
        // the button remains to be hovered until the mouse move. As a result, we need to
        // manully send leave events to the button.
        emulateLeaveEvent(maximizeButton);
    });
    closeButton = new StandardSystemButton(SystemButtonType::Close, q);
    connect(closeButton, &StandardSystemButton::clicked, this, [this](){
        if (hideWhenClose) {
            window->hide();
        } else {
            window->close();
        }
    });
    // According to the title bar design guidance, the system buttons should always be
    // placed on the top-right corner of the window, so we need the following additional
    // layouts to ensure this.
    const auto systemButtonsInnerLayout = new QHBoxLayout;
    systemButtonsInnerLayout->setSpacing(0);
    systemButtonsInnerLayout->setContentsMargins(0, 0, 0, 0);
    systemButtonsInnerLayout->addWidget(minimizeButton);
    systemButtonsInnerLayout->addWidget(maximizeButton);
    systemButtonsInnerLayout->addWidget(closeButton);
    const auto systemButtonsOuterLayout = new QVBoxLayout;
    systemButtonsOuterLayout->setSpacing(0);
    systemButtonsOuterLayout->setContentsMargins(0, 0, 0, 0);
    systemButtonsOuterLayout->addLayout(systemButtonsInnerLayout);
    systemButtonsOuterLayout->addStretch();
    const auto titleBarLayout = new QHBoxLayout(q);
    titleBarLayout->setSpacing(0);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
    titleBarLayout->addStretch();
    titleBarLayout->addLayout(systemButtonsOuterLayout);
#endif
    retranslateUi();
    updateTitleBarColor();
    updateChromeButtonColor();
    window->installEventFilter(this);
}

StandardTitleBar::StandardTitleBar(QWidget *parent)
    : QWidget(parent), d_ptr(std::make_unique<StandardTitleBarPrivate>(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(kDefaultTitleBarHeight);
#ifdef Q_OS_MACOS
    setTitleLabelAlignment(Qt::AlignCenter);
#else
    setTitleLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
#endif
}

StandardTitleBar::~StandardTitleBar() = default;

#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
StandardSystemButton *StandardTitleBar::minimizeButton() const
{
    Q_D(const StandardTitleBar);
    return d->minimizeButton;
}

StandardSystemButton *StandardTitleBar::maximizeButton() const
{
    Q_D(const StandardTitleBar);
    return d->maximizeButton;
}

StandardSystemButton *StandardTitleBar::closeButton() const
{
    Q_D(const StandardTitleBar);
    return d->closeButton;
}
#endif

void StandardTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    Q_D(StandardTitleBar);
    std::ignore = d->mouseEventHandler(event);
}

void StandardTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
    Q_D(StandardTitleBar);
    std::ignore = d->mouseEventHandler(event);
}

Qt::Alignment StandardTitleBar::titleLabelAlignment() const
{
    Q_D(const StandardTitleBar);
    return d->labelAlignment;
}

void StandardTitleBar::setTitleLabelAlignment(const Qt::Alignment value)
{
    Q_D(StandardTitleBar);
    if (d->labelAlignment == value) {
        return;
    }
    d->labelAlignment = value;
    update();
    Q_EMIT titleLabelAlignmentChanged();
}

bool StandardTitleBar::isExtended() const
{
    Q_D(const StandardTitleBar);
    return d->extended;
}

void StandardTitleBar::setExtended(const bool value)
{
    Q_D(StandardTitleBar);
    if (d->extended == value) {
        return;
    }
    d->extended = value;
    setFixedHeight(d->extended ? kDefaultExtendedTitleBarHeight : kDefaultTitleBarHeight);
    Q_EMIT extendedChanged();
}

bool StandardTitleBar::isHideWhenClose() const
{
    Q_D(const StandardTitleBar);
    return d->hideWhenClose;
}

void StandardTitleBar::setHideWhenClose(const bool value)
{
    Q_D(StandardTitleBar);
    if (d->hideWhenClose == value) {
        return;
    }
    d->hideWhenClose = value;
    Q_EMIT hideWhenCloseChanged();
}

ChromePalette *StandardTitleBar::chromePalette() const
{
    Q_D(const StandardTitleBar);
    return d->chromePalette;
}

void StandardTitleBar::paintEvent(QPaintEvent *event)
{
    Q_ASSERT(event);
    if (!event) {
        return;
    }
    Q_D(StandardTitleBar);
    if (!d->window) {
        return;
    }
    const bool active = d->window->isActiveWindow();
    const QColor backgroundColor = (active ?
        d->chromePalette->titleBarActiveBackgroundColor() :
        d->chromePalette->titleBarInactiveBackgroundColor());
    const QColor foregroundColor = (active ?
        d->chromePalette->titleBarActiveForegroundColor() :
        d->chromePalette->titleBarInactiveForegroundColor());
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.fillRect(QRect(QPoint(0, 0), size()), backgroundColor);
    if (d->titleLabelVisible) {
        const QString text = d->window->windowTitle();
        if (!text.isEmpty()) {
            painter.setPen(foregroundColor);
            painter.setFont(d->titleFont.value_or(d->defaultFont()));
            const auto pos = [this, d]() -> QPoint {
                const StandardTitleBarPrivate::FontMetrics labelSize = d->titleLabelSize();
                const int titleBarWidth = width();
                int x = 0;
                if (d->labelAlignment & Qt::AlignLeft) {
                    x = (d->windowIconRect().right() + kDefaultTitleBarContentsMargin);
                } else if (d->labelAlignment & Qt::AlignRight) {
                    x = (titleBarWidth - kDefaultTitleBarContentsMargin - labelSize.width);
#if (!defined(Q_OS_MACOS) && FRAMELESSHELPER_CONFIG(system_button))
                    x -= (titleBarWidth - d->minimizeButton->x());
#endif
                } else if (d->labelAlignment & Qt::AlignHCenter) {
                    x = std::round(qreal(titleBarWidth - labelSize.width) / qreal(2));
                } else {
                    WARNING << "The alignment for the title label is not set!";
                }
                const int y = std::round((qreal(height() - labelSize.height) / qreal(2)) + qreal(labelSize.ascent));
                return {x, y};
            }();
            const int textMaxWidth = d->titleLabelMaxWidth();
            const QString elidedText = painter.fontMetrics().elidedText(text, Qt::ElideRight, textMaxWidth, Qt::TextShowMnemonic);
            // No need to draw the text if there's only the elide mark left (or even less).
            if (elidedText.size() > 3) {
                painter.drawText(pos, elidedText);
            }
        }
    }
    if (d->windowIconVisible) {
        const QIcon icon = d->window->windowIcon();
        if (!icon.isNull()) {
            icon.paint(&painter, d->windowIconRect());
        }
    }
    painter.restore();
    event->accept();
}

bool StandardTitleBar::titleLabelVisible() const
{
    Q_D(const StandardTitleBar);
    return d->titleLabelVisible;
}

void StandardTitleBar::setTitleLabelVisible(const bool value)
{
    Q_D(StandardTitleBar);
    if (d->titleLabelVisible == value) {
        return;
    }
    d->titleLabelVisible = value;
    update();
    Q_EMIT titleLabelVisibleChanged();
}

QSize StandardTitleBar::windowIconSize() const
{
    Q_D(const StandardTitleBar);
    return d->windowIconSize.value_or(kDefaultWindowIconSize);
}

void StandardTitleBar::setWindowIconSize(const QSize &value)
{
    Q_ASSERT(!value.isEmpty());
    if (value.isEmpty()) {
        return;
    }
    if (windowIconSize() == value) {
        return;
    }
    Q_D(StandardTitleBar);
    d->windowIconSize = value;
    update();
    Q_EMIT windowIconSizeChanged();
}

bool StandardTitleBar::windowIconVisible() const
{
    Q_D(const StandardTitleBar);
    return d->windowIconVisible;
}

void StandardTitleBar::setWindowIconVisible(const bool value)
{
    Q_D(StandardTitleBar);
    if (d->windowIconVisible == value) {
        return;
    }
    d->windowIconVisible = value;
    update();
    Q_EMIT windowIconVisibleChanged();
#ifndef Q_OS_MACOS
    // Ideally we should use FramelessWidgetsHelper::get(this) everywhere, but sadly when
    // we call it here, it may be too early that FramelessWidgetsHelper has not attached
    // to the top level widget yet, and thus it will trigger an assert error (the assert
    // should not be suppressed, because it usually indicates there's something really
    // wrong). So here we have to use the top level widget directly, as a special case.
    // NOTE: In your own code, you should always use FramelessWidgetsHelper::get(this)
    // if possible.
    FramelessWidgetsHelper::get(d->window)->setHitTestVisible(d->windowIconRect(), d->windowIconVisible_real());
#endif // Q_OS_MACOS
}

QFont StandardTitleBar::titleFont() const
{
    Q_D(const StandardTitleBar);
    return d->titleFont.value_or(QFont());
}

void StandardTitleBar::setTitleFont(const QFont &value)
{
    if (titleFont() == value) {
        return;
    }
    Q_D(StandardTitleBar);
    d->titleFont = value;
    update();
    Q_EMIT titleFontChanged();
}

FRAMELESSHELPER_END_NAMESPACE

#endif
