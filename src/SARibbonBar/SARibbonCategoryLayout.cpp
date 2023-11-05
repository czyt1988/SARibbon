﻿#include "SARibbonCategoryLayout.h"
#include <QLayoutItem>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include "SARibbonElementManager.h"
#include "SARibbonSeparatorWidget.h"
#include <QApplication>
#include <QDebug>

#ifndef SARibbonCategoryLayout_DEBUG_PRINT
#define SARibbonCategoryLayout_DEBUG_PRINT 0
#endif
/**
 * @brief The SARibbonCategoryLayoutPrivate class
 */
class SARibbonCategoryLayout::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonCategoryLayout)
public:
    PrivateData(SARibbonCategoryLayout* p);
    //计算所有元素的sizehint总宽度
    int totalSizeHintWidth() const;

public:
    bool mDirty { true };
    bool mIsRightScrollBtnShow { false };                       ///< 标记右滚动按钮是否需要显示
    bool mIsLeftScrollBtnShow { false };                        ///< 标记左滚动按钮是否需要显示
    SARibbonCategoryScrollButton* mLeftScrollBtn { nullptr };   ///< 在区域无法显示时显示的按钮
    SARibbonCategoryScrollButton* mRightScrollBtn { nullptr };  ///< 在区域无法显示时显示的按钮
    int mTotalWidth { 0 };
    int mXBase { 0 };
    QSize mSizeHint { 50, 50 };
    QList< SARibbonCategoryLayoutItem* > mItemList;
};

//=============================================================
// SARibbonCategoryLayoutPrivate
//=============================================================

SARibbonCategoryLayout::PrivateData::PrivateData(SARibbonCategoryLayout* p) : q_ptr(p)
{
}

/**
 * @brief 计算所有元素的SizeHint宽度总和
 * @return
 */
int SARibbonCategoryLayout::PrivateData::totalSizeHintWidth() const
{
    int total    = 0;
    QMargins mag = q_ptr->contentsMargins();

    if (!mag.isNull()) {
        total += (mag.left() + mag.right());
    }
    //先计算总长
    for (SARibbonCategoryLayoutItem* item : qAsConst(mItemList)) {
        if (item->isEmpty()) {
            //如果是hide就直接跳过
            continue;
        }
        QSize pannelSize = item->widget()->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        total += pannelSize.width();
        total += SeparatorSize.width();
    }
    return (total);
}

//=============================================================
// SARibbonCategoryLayout
//=============================================================

SARibbonCategoryLayout::SARibbonCategoryLayout(SARibbonCategory* parent)
    : QLayout(parent), d_ptr(new SARibbonCategoryLayout::PrivateData(this))
{
    setContentsMargins(1, 1, 1, 1);
    d_ptr->mLeftScrollBtn  = new SARibbonCategoryScrollButton(Qt::LeftArrow, parent);
    d_ptr->mRightScrollBtn = new SARibbonCategoryScrollButton(Qt::RightArrow, parent);
    d_ptr->mLeftScrollBtn->setVisible(false);
    d_ptr->mRightScrollBtn->setVisible(false);
    connect(d_ptr->mLeftScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onLeftScrollButtonClicked);
    connect(d_ptr->mRightScrollBtn, &QToolButton::clicked, this, &SARibbonCategoryLayout::onRightScrollButtonClicked);
}

SARibbonCategoryLayout::~SARibbonCategoryLayout()
{
    while (QLayoutItem* item = takeAt(0)) {
        delete item;
    }
}

SARibbonCategory* SARibbonCategoryLayout::ribbonCategory() const
{
    return (qobject_cast< SARibbonCategory* >(parentWidget()));
}

void SARibbonCategoryLayout::addItem(QLayoutItem* item)
{
    Q_UNUSED(item);
    qWarning() << tr("in SARibbonCategoryLayout cannot addItem,use addPannel instead");
    invalidate();
}

/**
 * @brief 返回pannel的layout
 * @param index 索引
 * @return
 * @note 注意，pannel是和分割线一起的，但这个只返回一个pannel对应的layout
 */
QLayoutItem* SARibbonCategoryLayout::itemAt(int index) const
{
    SARibbonCategoryLayoutItem* item = d_ptr->mItemList.value(index, nullptr);

    return (item);
}

/**
 * @brief 提取layout
 * @param index
 * @return
 */
QLayoutItem* SARibbonCategoryLayout::takeAt(int index)
{
    return (takePannelItem(index));
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePannelItem(int index)
{
    if ((index >= 0) && (index < d_ptr->mItemList.size())) {
        invalidate();
        SARibbonCategoryLayoutItem* item = d_ptr->mItemList.takeAt(index);
        if (item->widget()) {
            item->widget()->hide();
        }
        if (item->separatorWidget) {
            item->separatorWidget->hide();
        }
        return (item);
    }
    return (nullptr);
}

SARibbonCategoryLayoutItem* SARibbonCategoryLayout::takePannelItem(SARibbonPannel* pannel)
{
    for (int i = 0; i < d_ptr->mItemList.size(); ++i) {
        SARibbonCategoryLayoutItem* item = d_ptr->mItemList[ i ];
        if (item->widget() == pannel) {
            return (takePannelItem(i));
        }
    }
    return (nullptr);
}

/**
 * @brief 移除pannel，对应的分割线也会删除
 * @param pannel
 * @return
 */
bool SARibbonCategoryLayout::takePannel(SARibbonPannel* pannel)
{
    SARibbonCategoryLayoutItem* i = takePannelItem(pannel);
    if (i) {
        SARibbonSeparatorWidget* sp = i->separatorWidget;
        if (sp) {
            sp->deleteLater();
        }
        delete i;
        return true;
    }
    return false;
}

int SARibbonCategoryLayout::count() const
{
    return (d_ptr->mItemList.size());
}

void SARibbonCategoryLayout::addPannel(SARibbonPannel* pannel)
{
    insertPannel(d_ptr->mItemList.count(), pannel);
}

/**
 * @brief 插入一个pannel
 * @param index 索引
 * @param pannel
 * @return 返回对应的分割线SARibbonSeparatorWidget
 * @note 在SARibbonCategoryLayout的布局中，一个pannel会携带一个分割线
 */
void SARibbonCategoryLayout::insertPannel(int index, SARibbonPannel* pannel)
{
    index                            = qMax(0, index);
    index                            = qMin(d_ptr->mItemList.count(), index);
    SARibbonCategoryLayoutItem* item = new SARibbonCategoryLayoutItem(pannel);

    //分割线
    item->separatorWidget = RibbonSubElementDelegate->createRibbonSeparatorWidget(parentWidget());
    //插入list中
    d_ptr->mItemList.insert(index, item);
    //标记需要重新计算尺寸
    invalidate();
}

QSize SARibbonCategoryLayout::sizeHint() const
{
    return (d_ptr->mSizeHint);
}

QSize SARibbonCategoryLayout::minimumSize() const
{
    return (d_ptr->mSizeHint);
}

/**
 * @brief SARibbonCategory充满整个stacked widget
 * @return
 */
Qt::Orientations SARibbonCategoryLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}

void SARibbonCategoryLayout::invalidate()
{
    d_ptr->mDirty = true;
    QLayout::invalidate();
}
/**
 * @brief category的内容尺寸（把margins减去）
 * @return
 */
QSize SARibbonCategoryLayout::categoryContentSize() const
{
    SARibbonCategory* category = ribbonCategory();
    QSize s                    = category->size();
    QMargins mag               = contentsMargins();
    if (!mag.isNull()) {
        s.rheight() -= (mag.top() + mag.bottom());
        s.rwidth() -= (mag.right() + mag.left());
    }
    return (s);
}

/**
 * @brief 更新尺寸
 */
void SARibbonCategoryLayout::updateGeometryArr()
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    if (nullptr == category) {
        return;
    }

    int categoryWidth = category->width();
    QMargins mag      = contentsMargins();
    int height        = category->height();
    int y             = 0;

    if (!mag.isNull()) {
        y = mag.top();
        height -= (mag.top() + mag.bottom());
        categoryWidth -= (mag.right() + mag.left());
    }
    // total 是总宽，不是x坐标系，x才是坐标系
    int total = d_ptr->totalSizeHintWidth();

    //扩展的宽度
    int expandWidth = 0;

//如果total < categoryWidth,m_d->mXBase可以设置为0
//判断是否超过总长度
#if SARibbonCategoryLayout_DEBUG_PRINT
    qDebug() << "\r\n\r\n==========================(" << category->categoryName() << ")======="
             << "\r\nSARibbonCategoryLayout::updateGeometryArr"
             << "\r\npannel name:" << category->windowTitle() << "\r\n height:" << height
             << "\r\n first total:" << total << "\r\n y:" << y << "\r\n expandWidth:" << expandWidth;
#endif
    if (total > categoryWidth) {
        //超过总长度，需要显示滚动按钮
        if (0 == d_ptr->mXBase) {
            //已经移动到最左，需要可以向右移动
            d_ptr->mIsRightScrollBtnShow = true;
            d_ptr->mIsLeftScrollBtnShow  = false;
        } else if (d_ptr->mXBase <= (categoryWidth - total)) {
            //已经移动到最右，需要可以向左移动
            d_ptr->mIsRightScrollBtnShow = false;
            d_ptr->mIsLeftScrollBtnShow  = true;
        } else {
            //移动到中间两边都可以动
            d_ptr->mIsRightScrollBtnShow = true;
            d_ptr->mIsLeftScrollBtnShow  = true;
        }
    } else {
        //记录可以扩展的数量
        int canExpandingCount = 0;
        //说明total 小于 categoryWidth
        d_ptr->mIsRightScrollBtnShow = false;
        d_ptr->mIsLeftScrollBtnShow  = false;
        //这个是避免一开始totalWidth > categorySize.width()，通过滚动按钮调整了m_d->mBaseX
        //随之调整了窗体尺寸，调整后totalWidth < categorySize.width()导致category在原来位置
        //无法显示，必须这里把mBaseX设置为0
        d_ptr->mXBase = 0;
        //

        for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
            if (SARibbonPannel* p = qobject_cast< SARibbonPannel* >(item->widget())) {
                if (p->isExpanding()) {
                    // pannel可扩展
                    ++canExpandingCount;
                }
            }
        }
        //计算可扩展的宽度
        if (canExpandingCount > 0) {
            expandWidth = (categoryWidth - total) / canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    total = 0;  // total重新计算
    int x = d_ptr->mXBase;

    //先按照sizeHint设置所有的尺寸
    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (item->isEmpty()) {
            //如果是hide就直接跳过
            if (item->separatorWidget) {
                // pannel hide分割线也要hide
                item->separatorWidget->hide();
            }
            item->mWillSetGeometry          = QRect(0, 0, 0, 0);
            item->mWillSetSeparatorGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        SARibbonPannel* p = item->toPannelWidget();
        if (nullptr == p) {
            qDebug() << "unknow widget in SARibbonCategoryLayout";
            continue;
        }
        p->layout()->update();
        QSize pannelSize = p->sizeHint();
        QSize SeparatorSize(0, 0);
        if (item->separatorWidget) {
            SeparatorSize = item->separatorWidget->sizeHint();
        }
        if (p->isExpanding()) {
            //可扩展，就把pannel扩展到最大
            pannelSize.setWidth(pannelSize.width() + expandWidth);
        }
        int w                  = pannelSize.width();
        item->mWillSetGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
        w                               = SeparatorSize.width();
        item->mWillSetSeparatorGeometry = QRect(x, y, w, height);
        x += w;
        total += w;
    }
    d_ptr->mTotalWidth = total;
    QWidget* cp        = category->parentWidget();
    int parentHeight   = (nullptr == cp) ? height : cp->height();
    int parentWidth    = (nullptr == cp) ? total : cp->width();
#if SARibbonCategoryLayout_DEBUG_PRINT
    qDebug() << "\r\n mSizeHint:[ " << parentHeight << "," << parentWidth << "]";
    for (int i = 0; i < d_ptr->mItemList.size(); ++i) {
        qDebug() << "\r\n [ " << i << "] (pannel name=" << d_ptr->mItemList[ i ]->toPannelWidget()->pannelName()
                 << "),sizeHint=" << d_ptr->mItemList[ i ]->toPannelWidget()->sizeHint()
                 << "\r\n geo:" << d_ptr->mItemList[ i ]->mWillSetGeometry
                 << ", Separator geo:" << d_ptr->mItemList[ i ]->mWillSetSeparatorGeometry;
    }
#endif
    d_ptr->mSizeHint = QSize(parentWidth, parentHeight);
}

/**
 * @brief 执行layout调整
 */
void SARibbonCategoryLayout::doLayout()
{
    if (d_ptr->mDirty) {
        updateGeometryArr();
    }
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    //两个滚动按钮的位置永远不变
    d_ptr->mLeftScrollBtn->setGeometry(0, 0, 12, category->height());
    d_ptr->mRightScrollBtn->setGeometry(category->width() - 12, 0, 12, category->height());
    QList< QWidget* > showWidgets, hideWidgets;

    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
            if (item->separatorWidget) {
                hideWidgets << item->separatorWidget;
            }
        } else {
            item->widget()->setFixedSize(item->mWillSetGeometry.size());
            item->widget()->move(item->mWillSetGeometry.topLeft());
            //            item->setGeometry(item->mWillSetGeometry);
            showWidgets << item->widget();
            if (item->separatorWidget) {
                item->separatorWidget->setGeometry(item->mWillSetSeparatorGeometry);
                showWidgets << item->separatorWidget;
            }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            qDebug() << "SARibbonCategoryLayout::doLayout() =";
            qDebug() << "\r\n     pannel:" << item->widget()->windowTitle() << "\r\n     pannel geo:" << item->mWillSetGeometry
                     << "\r\n     sep geo:" << item->mWillSetSeparatorGeometry;
#endif
        }
    }

    d_ptr->mRightScrollBtn->setVisible(d_ptr->mIsRightScrollBtnShow);
    d_ptr->mLeftScrollBtn->setVisible(d_ptr->mIsLeftScrollBtnShow);
    if (d_ptr->mIsRightScrollBtnShow) {
        d_ptr->mRightScrollBtn->raise();
    }
    if (d_ptr->mIsLeftScrollBtnShow) {
        d_ptr->mLeftScrollBtn->raise();
    }
    // 不在上面那里进行show和hide因为这会触发SARibbonPannelLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget* w : qAsConst(showWidgets)) {
        w->show();
    }
    for (QWidget* w : qAsConst(hideWidgets)) {
        w->hide();
    }
}

/**
 * @brief 返回所有pannels
 * @return
 */
QList< SARibbonPannel* > SARibbonCategoryLayout::pannels() const
{
    QList< SARibbonPannel* > res;

    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        SARibbonPannel* p = item->toPannelWidget();
        res.append(p);
    }
    return (res);
}

/**
 * @brief 通过ObjectName查找pannel
 * @param objname
 * @return
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByObjectName(const QString& objname) const
{
    for (SARibbonCategoryLayoutItem* item : d_ptr->mItemList) {
        if (SARibbonPannel* pannel = item->toPannelWidget()) {
            if (pannel->objectName() == objname) {
                return pannel;
            }
        }
    }
    return nullptr;
}

/**
 * @brief 通过名字查找pannel
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByName(const QString& pannelname) const
{
    for (SARibbonCategoryLayoutItem* item : qAsConst(d_ptr->mItemList)) {
        if (SARibbonPannel* pannel = item->toPannelWidget()) {
            if (pannel->pannelName() == pannelname) {
                return (pannel);
            }
        }
    }
    return (nullptr);
}

/**
 * @brief 通过索引找到pannel，如果超过索引范围，会返回nullptr
 * @param i
 * @return
 */
SARibbonPannel* SARibbonCategoryLayout::pannelByIndex(int i) const
{
    if (i >= 0 && i < d_ptr->mItemList.size()) {
        return d_ptr->mItemList[ i ]->toPannelWidget();
    }
    return nullptr;
}

/**
 * @brief 移动pannel
 * @param from
 * @param to
 */
void SARibbonCategoryLayout::movePannel(int from, int to)
{
    d_ptr->mItemList.move(from, to);
    doLayout();
}

/**
 * @brief 返回pannel的个数
 * @return
 */
int SARibbonCategoryLayout::pannelCount() const
{
    return d_ptr->mItemList.size();
}

/**
 * @brief 查找pannel对应的索引
 * @param p
 * @return 如果找不到，返回-1
 */
int SARibbonCategoryLayout::pannelIndex(SARibbonPannel* p) const
{
    int c = pannelCount();

    for (int i = 0; i < c; ++i) {
        if (d_ptr->mItemList[ i ]->toPannelWidget() == p) {
            return (i);
        }
    }
    return (-1);
}

/**
 * @brief 获取所有的pannel
 * @return
 */
QList< SARibbonPannel* > SARibbonCategoryLayout::pannelList() const
{
    QList< SARibbonPannel* > res;

    for (SARibbonCategoryLayoutItem* i : qAsConst(d_ptr->mItemList)) {
        if (SARibbonPannel* p = i->toPannelWidget()) {
            res.append(p);
        }
    }
    return (res);
}

/**
 * @brief 执行滚动
 * @param px
 */
void SARibbonCategoryLayout::scroll(int px)
{
    QSize contentSize = categoryContentSize();
    d_ptr->mXBase += px;
    if (d_ptr->mXBase > 0) {
        d_ptr->mXBase = 0;
    } else if ((d_ptr->mXBase + d_ptr->mTotalWidth) < contentSize.width()) {
        d_ptr->mXBase = contentSize.width() - d_ptr->mTotalWidth;
    }
    invalidate();
}

/**
 * @brief 判断是否滚动过
 * @return
 */
bool SARibbonCategoryLayout::isScrolled() const
{
    return (d_ptr->mXBase != 0);
}

/**
 * @brief 这个宽度是实际内容的宽度，有可能大于size.width，也有可能小于
 * @return
 */
int SARibbonCategoryLayout::categoryTotalWidth() const
{
    return d_ptr->mTotalWidth;
}

void SARibbonCategoryLayout::onLeftScrollButtonClicked()
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    int width                  = category->width();
    //求总宽
    int totalWidth = d_ptr->mTotalWidth;

    if (totalWidth > width) {
        int tmp = d_ptr->mXBase + width;
        if (tmp > 0) {
            tmp = 0;
        }
        d_ptr->mXBase = tmp;
    } else {
        d_ptr->mXBase = 0;
    }
    invalidate();
}

void SARibbonCategoryLayout::onRightScrollButtonClicked()
{
    SARibbonCategory* category = qobject_cast< SARibbonCategory* >(parentWidget());
    int width                  = category->width();
    //求总宽
    int totalWidth = d_ptr->mTotalWidth;

    if (totalWidth > width) {
        int tmp = d_ptr->mXBase - width;
        if (tmp < (width - totalWidth)) {
            tmp = width - totalWidth;
        }
        d_ptr->mXBase = tmp;
    } else {
        d_ptr->mXBase = 0;
    }
    invalidate();
}

void SARibbonCategoryLayout::setGeometry(const QRect& rect)
{
    d_ptr->mDirty = false;
    updateGeometryArr();
    QLayout::setGeometry(rect);
    doLayout();
}
//=============================================================
// SARibbonCategoryLayoutItem
//=============================================================

SARibbonCategoryLayoutItem::SARibbonCategoryLayoutItem(SARibbonPannel* w) : QWidgetItem(w)
{
    separatorWidget = nullptr;
}

SARibbonPannel* SARibbonCategoryLayoutItem::toPannelWidget()
{
    return qobject_cast< SARibbonPannel* >(widget());
}
