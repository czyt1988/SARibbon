#include "SARibbonCtrlContainer.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include "SARibbonUtil.h"
/**
 * @brief The SARibbonCtrlContainerPrivate class
 */
class SARibbonCtrlContainer::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCtrlContainer)
public:
    QWidget* mContainerWidget { nullptr };
    QHBoxLayout* mHorizontalLayout { nullptr };
    QLabel* mLabelPixmap { nullptr };
    QLabel* mLabelText { nullptr };
    QSize mIconSize { 24, 24 };
    QIcon mIcon;
    PrivateData(SARibbonCtrlContainer* p) : q_ptr(p)
    {
        mHorizontalLayout = new QHBoxLayout(p);
        mHorizontalLayout->setSpacing(0);
        mHorizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mHorizontalLayout->setContentsMargins(0, 0, 0, 0);

        mLabelPixmap = new QLabel(p);
        mLabelPixmap->setObjectName(QString::fromUtf8("labelPixmap"));
        mLabelPixmap->setAlignment(Qt::AlignCenter);
        mHorizontalLayout->addWidget(mLabelPixmap);

        mLabelText = new QLabel(p);
        mLabelText->setObjectName(QString::fromUtf8("labelText"));
        mHorizontalLayout->addWidget(mLabelText);
    }

    void setContainerWidget(QWidget* w)
    {
        if (mContainerWidget) {
            // 原来有widget
            QWidget* oldwidget = mContainerWidget;
            takeContainerWidget(oldwidget);
            oldwidget->deleteLater();
        }
        mContainerWidget = w;
        if (!w) {
            return;
        }
        if (w->parent() != q_ptr) {
            w->setParent(q_ptr);
        }
        // ContainerWidget永远在最右边
        mHorizontalLayout->insertWidget(mHorizontalLayout->count(), w);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        w->setSizePolicy(sizePolicy);
    }

    void takeContainerWidget(QWidget* w)
    {
        int i = mHorizontalLayout->indexOf(w);
        if (i >= 0) {
            QLayoutItem* item = mHorizontalLayout->takeAt(i);
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

QWidget* SARibbonCtrlContainer::containerWidget()
{
    return (d_ptr->mContainerWidget);
}

const QWidget* SARibbonCtrlContainer::containerWidget() const
{
    return (d_ptr->mContainerWidget);
}

void SARibbonCtrlContainer::setEnableShowIcon(bool b)
{
    d_ptr->mLabelPixmap->setVisible(b);
}

void SARibbonCtrlContainer::setEnableShowTitle(bool b)
{
    d_ptr->mLabelText->setVisible(b);
}

/**
 * @brief 判断是否存在容器窗口
 * @return
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (d_ptr->mContainerWidget != nullptr);
}

/**
 * @brief 设置图标
 * @param i
 */
void SARibbonCtrlContainer::setIcon(const QIcon& i)
{
    d_ptr->mIcon = i;
    // QPixmap pixmap = i.pixmap(d_ptr->mIconSize);
    QPixmap pixmap = SA::iconToPixmap(i, d_ptr->mIconSize, SA::widgetDevicePixelRatio(this));
    d_ptr->mLabelPixmap->setPixmap(pixmap);
}

void SARibbonCtrlContainer::setIcon(const QPixmap& pixmap)
{
    d_ptr->mLabelPixmap->setPixmap(pixmap);
}

/**
 * @brief 获取图标
 * @return
 */
QIcon SARibbonCtrlContainer::icon() const
{
    return d_ptr->mIcon;
}

/**
 * @brief 设置文字
 * @param t
 */
void SARibbonCtrlContainer::setText(const QString& t)
{
    d_ptr->mLabelText->setText(t);
}

/**
 * @brief 获取文字
 * @return
 */
QString SARibbonCtrlContainer::text() const
{
    return d_ptr->mLabelText->text();
}

void SARibbonCtrlContainer::setContainerWidget(QWidget* w)
{
    d_ptr->setContainerWidget(w);
}

/**
 * @brief 获取显示icon的窗口
 * @return
 */
QWidget* SARibbonCtrlContainer::iconWidget() const
{
    return d_ptr->mLabelPixmap;
}
