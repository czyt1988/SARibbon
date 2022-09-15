#include "SARibbonCtrlContainer.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include "SARibbonDrawHelper.h"

/**
 * @brief The SARibbonCtrlContainerPrivate class
 */
class SARibbonCtrlContainerPrivate
{
public:
    SARibbonCtrlContainer* Parent;
    QWidget* containerWidget;
    bool enableDrawIcon;
    bool enableDrawTitle;
    SARibbonCtrlContainerPrivate(SARibbonCtrlContainer* p)
        : containerWidget(Q_NULLPTR), enableDrawIcon(true), enableDrawTitle(true)
    {
        Parent = p;
    }

    void init(QWidget* w)
    {
        containerWidget = w;
        if (w) {
            containerWidget->setParent(Parent);
        }
        Parent->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    }
};

SARibbonCtrlContainer::SARibbonCtrlContainer(QWidget* containerWidget, QWidget* parent)
    : QWidget(parent), m_d(new SARibbonCtrlContainerPrivate(this))
{
    m_d->init(containerWidget);
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
}

SARibbonCtrlContainer::~SARibbonCtrlContainer()
{
    delete m_d;
}

QSize SARibbonCtrlContainer::sizeHint() const
{
    QSize containerSizeHint(0, 0);
    if (hasContainerWidget()) {
        containerSizeHint = containerWidget()->sizeHint();
    }

    if (m_d->enableDrawIcon) {
        QIcon icon = windowIcon();
        // 如果hasContainerWidget，但containerWidget的sizehint=0,0
        // 同时又存在图标，要把图标显示，那么给定一个默认的大小
        if (containerSizeHint.isEmpty()) {
            containerSizeHint.setHeight(30);
        }
        if (!icon.isNull()) {
            //有图标就把宽度加长一个高度
            containerSizeHint.setWidth(containerSizeHint.width() + containerSizeHint.height());
        }
    }
    if (m_d->enableDrawTitle) {
        QString text = windowTitle();
        if (!text.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            int textWidth = fontMetrics().horizontalAdvance(text);
#else
            int textWidth = fontMetrics().width(text);
#endif
            containerSizeHint.setWidth(containerSizeHint.width() + textWidth);
        }
    }
    return (containerSizeHint);
}

QSize SARibbonCtrlContainer::minimumSizeHint() const
{
    QSize containerHint(0, 0);
    if (hasContainerWidget()) {
        containerHint = m_d->containerWidget->minimumSizeHint();
    }

    if (m_d->enableDrawIcon) {
        QIcon icon = windowIcon();
        if (!icon.isNull()) {
            containerHint.setWidth(containerHint.width() + containerHint.height());
        }
    }
    if (m_d->enableDrawTitle) {
        QString text = windowTitle();
        if (!text.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            int textWidth = fontMetrics().horizontalAdvance(text.at(0));
#else
            int textWidth = fontMetrics().width(text.at(0));
#endif
            containerHint.setWidth(containerHint.width() + (2 * textWidth));
        }
    }

    return (containerHint.expandedTo(minimumSize()));
}

QWidget* SARibbonCtrlContainer::containerWidget()
{
    return (m_d->containerWidget);
}

const QWidget* SARibbonCtrlContainer::containerWidget() const
{
    return (m_d->containerWidget);
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    m_d->enableDrawIcon = b;
    update();
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    m_d->enableDrawTitle = b;
    update();
}

/**
 * @brief 判断是否存在容器窗口
 * @return
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (m_d->containerWidget != nullptr);
}

void SARibbonCtrlContainer::setContainerWidget(QWidget* w)
{
    if (hasContainerWidget()) {
        m_d->containerWidget->hide();
        m_d->containerWidget->deleteLater();
    }
    if (w) {
        w->setParent(this);
    }
    m_d->containerWidget = w;
}

void SARibbonCtrlContainer::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter painter(this);

    // draw the combobox frame, focusrect and selected etc.
    QStyleOption opt;

    initStyleOption(&opt);
    const int widgetHeight = height();
    int x                  = 0;

    //绘制图标
    if (m_d->enableDrawIcon) {
        QIcon icon = windowIcon();
        if (!icon.isNull()) {
            //绘制图标
            int spacing = 2;
            QSize iconSize = SARibbonDrawHelper::iconActualSize(icon, &opt, QSize(widgetHeight - 2 * spacing, widgetHeight - 2 * spacing));
            x += spacing;
            SARibbonDrawHelper::drawIcon(icon, &painter, &opt, x, spacing, iconSize.width(), iconSize.height());
            x += (iconSize.width() + spacing);
        }
    }
    //绘制文字
    if (m_d->enableDrawTitle) {
        QString text = windowTitle();
        if (!text.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            int textWidth = opt.fontMetrics.horizontalAdvance(text);
#else
            int textWidth = opt.fontMetrics.width(text);
#endif
            if (textWidth > (opt.rect.width() - widgetHeight - x)) {
                textWidth = opt.rect.width() - widgetHeight - x;
                text      = opt.fontMetrics.elidedText(text, Qt::ElideRight, textWidth);
            }
            if (textWidth > 0) {
                SARibbonDrawHelper::drawText(text, &painter, &opt, Qt::AlignLeft | Qt::AlignVCenter, QRect(x, 0, textWidth, opt.rect.height()));
            }
        }
    }
    // QWidget::paintEvent(e);
}

void SARibbonCtrlContainer::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    QStyleOption opt;

    initStyleOption(&opt);
    int x                  = 0;
    const int widgetHeight = height();

    if (m_d->enableDrawIcon) {
        QIcon icon = windowIcon();
        if (!icon.isNull()) {
            QSize iconSize = SARibbonDrawHelper::iconActualSize(icon, &opt, QSize(widgetHeight, widgetHeight));
            x += iconSize.width();
            x += 4;
        }
    }
    //绘制文字
    if (m_d->enableDrawTitle) {
        QString text = windowTitle();
        if (!text.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            int textWidth = opt.fontMetrics.horizontalAdvance(text);
#else
            int textWidth = opt.fontMetrics.width(text);
#endif
            if (textWidth > (opt.rect.width() - widgetHeight - x)) {
                textWidth = opt.rect.width() - widgetHeight - x;
                text      = opt.fontMetrics.elidedText(text, Qt::ElideRight, textWidth);
            }
            if (textWidth > 0) {
                x += textWidth;
                x += 2;
            }
        }
    }
    if (m_d->containerWidget) {
        m_d->containerWidget->setGeometry(x, 0, width() - x, height());
    }
}

void SARibbonCtrlContainer::initStyleOption(QStyleOption* opt)
{
    opt->initFrom(this);
}
