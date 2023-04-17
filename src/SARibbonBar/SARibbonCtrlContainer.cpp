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
class SARibbonCtrlContainer::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCtrlContainer)
public:
    QWidget* containerWidget { nullptr };
    QHBoxLayout* horizontalLayout { nullptr };
    QLabel* labelPixmap { nullptr };
    QLabel* labelText { nullptr };
    QSize iconSize { 24, 24 };
    QIcon icon;
    PrivateData(SARibbonCtrlContainer* p) : q_ptr(p)
    {
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

//===================================================
// SARibbonCtrlContainer
//===================================================
SARibbonCtrlContainer::SARibbonCtrlContainer(QWidget* parent)
    : QWidget(parent), d_ptr(new SARibbonCtrlContainer::PrivateData(this))
{
}

SARibbonCtrlContainer::~SARibbonCtrlContainer()
{
}

QWidget* SARibbonCtrlContainer::getContainerWidget()
{
    return (d_ptr->containerWidget);
}

const QWidget* SARibbonCtrlContainer::getContainerWidget() const
{
    return (d_ptr->containerWidget);
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    d_ptr->labelPixmap->setVisible(b);
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    d_ptr->labelText->setVisible(b);
}

/**
 * @brief 判断是否存在容器窗口
 * @return
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (d_ptr->containerWidget != nullptr);
}

/**
 * @brief 设置图标
 * @param i
 */
void SARibbonCtrlContainer::setIcon(const QIcon& i)
{
    d_ptr->icon = i;
    d_ptr->labelPixmap->setPixmap(i.pixmap(d_ptr->iconSize));
}

/**
 * @brief 获取图标
 * @return
 */
QIcon SARibbonCtrlContainer::getIcon() const
{
    return d_ptr->icon;
}

/**
 * @brief 设置文字
 * @param t
 */
void SARibbonCtrlContainer::setText(const QString& t)
{
    d_ptr->labelText->setText(t);
}

/**
 * @brief 获取文字
 * @return
 */
QString SARibbonCtrlContainer::getText() const
{
    return d_ptr->labelText->text();
}

void SARibbonCtrlContainer::setContainerWidget(QWidget* w)
{
    d_ptr->setContainerWidget(w);
}
