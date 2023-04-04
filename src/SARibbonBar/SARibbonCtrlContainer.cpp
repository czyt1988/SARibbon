#include "SARibbonCtrlContainer.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include <QLabel>
#include "SARibbonDrawHelper.h"

/**
 * @brief The SARibbonCtrlContainerPrivate class
 */
class SARibbonCtrlContainerPrivate
{
public:
    SARibbonCtrlContainer* q_ptr;
    QWidget* containerWidget;
    QHBoxLayout* horizontalLayout;
    QLabel* labelPixmap;
    QLabel* labelText;
    QIcon icon;
    QSize iconSize;
    SARibbonCtrlContainerPrivate(SARibbonCtrlContainer* p) : containerWidget(Q_NULLPTR), iconSize(24, 24)
    {
        q_ptr            = p;
        horizontalLayout = new QHBoxLayout(p);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);

        labelPixmap = new QLabel(p);
        labelPixmap->setObjectName(QString::fromUtf8("labelPixmap"));
        labelPixmap->setAlignment(Qt::AlignCenter);
        horizontalLayout->addWidget(labelPixmap);

        labelText = new QLabel(p);
        labelText->setObjectName(QString::fromUtf8("labelText"));
        horizontalLayout->addWidget(labelText);
    }

    void setContainerWidget(QWidget* w)
    {
        if (containerWidget) {
            //原来有widget
            QWidget* oldwidget = containerWidget;
            takeContainerWidget(oldwidget);
            oldwidget->deleteLater();
        }
        containerWidget = w;
        if (!w) {
            return;
        }
        if (w->parent() != q_ptr) {
            w->setParent(q_ptr);
        }
        // ContainerWidget永远在最右边
        horizontalLayout->insertWidget(horizontalLayout->count(), w);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        w->setSizePolicy(sizePolicy);
    }

    void takeContainerWidget(QWidget* w)
    {
        int i = horizontalLayout->indexOf(w);
        if (i >= 0) {
            QLayoutItem* item = horizontalLayout->takeAt(i);
            delete item;
        }
    }
};

SARibbonCtrlContainer::SARibbonCtrlContainer(QWidget* parent)
    : QWidget(parent), m_d(new SARibbonCtrlContainerPrivate(this))
{
}

SARibbonCtrlContainer::~SARibbonCtrlContainer()
{
    delete m_d;
}

QWidget* SARibbonCtrlContainer::getContainerWidget()
{
    return (m_d->containerWidget);
}

const QWidget* SARibbonCtrlContainer::getContainerWidget() const
{
    return (m_d->containerWidget);
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    m_d->labelPixmap->setVisible(b);
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    m_d->labelText->setVisible(b);
}

/**
 * @brief 判断是否存在容器窗口
 * @return
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (m_d->containerWidget != nullptr);
}

/**
 * @brief 设置图标
 * @param i
 */
void SARibbonCtrlContainer::setIcon(const QIcon& i)
{
    m_d->icon = i;
    m_d->labelPixmap->setPixmap(i.pixmap(m_d->iconSize));
}

/**
 * @brief 获取图标
 * @return
 */
QIcon SARibbonCtrlContainer::getIcon() const
{
    return m_d->icon;
}

/**
 * @brief 设置文字
 * @param t
 */
void SARibbonCtrlContainer::setText(const QString& t)
{
    m_d->labelText->setText(t);
}

/**
 * @brief 获取文字
 * @return
 */
QString SARibbonCtrlContainer::getText() const
{
    return m_d->labelText->text();
}

void SARibbonCtrlContainer::setContainerWidget(QWidget* w)
{
    m_d->setContainerWidget(w);
}
