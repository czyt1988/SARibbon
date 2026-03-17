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
 * \if ENGLISH
 * @brief Private data class for SARibbonCtrlContainer
 * @details This class holds private data for SARibbonCtrlContainer to implement the PIMPL idiom.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCtrlContainerPrivate 类
 * @details 此类持有 SARibbonCtrlContainer 的私有数据，实现 PIMPL 设计模式。
 * \endif
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

    /**
     * \if ENGLISH
     * @brief Set the container widget
     * @param w Widget to set as container
     * \endif
     *
     * \if CHINESE
     * @brief 设置容器窗口
     * @param w 要设置为容器的窗口
     * \endif
     */
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

    /**
     * \if ENGLISH
     * @brief Take the container widget from layout
     * @param w Widget to remove from layout
     * \endif
     *
     * \if CHINESE
     * @brief 从布局中移除容器窗口
     * @param w 要从布局中移除的窗口
     * \endif
     */
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
 * \if ENGLISH
 * @brief Check if container widget exists
 * @return true if container widget exists
 * \endif
 *
 * \if CHINESE
 * @brief 判断是否存在容器窗口
 * @return 如果容器窗口存在返回true
 * \endif
 */
bool SARibbonCtrlContainer::hasContainerWidget() const
{
    return (d_ptr->mContainerWidget != nullptr);
}

/**
 * \if ENGLISH
 * @brief Set the icon
 * @param i Icon to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置图标
 * @param i 要设置的图标
 * \endif
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
 * \if ENGLISH
 * @brief Get the icon
 * @return Current icon
 * \endif
 *
 * \if CHINESE
 * @brief 获取图标
 * @return 当前图标
 * \endif
 */
QIcon SARibbonCtrlContainer::icon() const
{
    return d_ptr->mIcon;
}

/**
 * \if ENGLISH
 * @brief Set the text
 * @param t Text to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置文字
 * @param t 要设置的文字
 * \endif
 */
void SARibbonCtrlContainer::setText(const QString& t)
{
    d_ptr->mLabelText->setText(t);
}

/**
 * \if ENGLISH
 * @brief Get the text
 * @return Current text
 * \endif
 *
 * \if CHINESE
 * @brief 获取文字
 * @return 当前文字
 * \endif
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
 * \if ENGLISH
 * @brief Get the widget displaying the icon
 * @return Widget displaying the icon
 * \endif
 *
 * \if CHINESE
 * @brief 获取显示icon的窗口
 * @return 显示图标的窗口
 * \endif
 */
QWidget* SARibbonCtrlContainer::iconWidget() const
{
    return d_ptr->mLabelPixmap;
}
